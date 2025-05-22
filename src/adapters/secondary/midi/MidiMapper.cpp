#include "adapters/secondary/midi/MidiMapper.hpp"

#include <stdarg.h>

#include <config/ConfigDefaults.hpp>

#include "config/GlobalSettings.hpp"
#include "config/debug/DebugMacros.hpp"  // Pour avoir accès à PERFORMANCE_MODE
#include "core/domain/events/core/EventTypes.hpp"
#include "tools/Diagnostics.hpp"

//=============================================================================
// Construction et initialisation
//=============================================================================

MidiMapper::MidiMapper(MidiOutputPort& midiOut, CommandManager& commandManager)
    : midiOut_(midiOut),
      commandManager_(commandManager),
      defaultControl_(
          {0, 0, false, true, ControlType::ENCODER_ROTATION})  // Canal 1, CC 0, mode absolu
{
    // Initialisation préalable des pool d'objets
    for (auto& cmd : midiCCCommandPool_) {
        cmd.reset(midiOut_, 0, 0, 0, 0);
    }

    for (auto& cmd : midiNoteCommandPool_) {
        cmd.reset(midiOut_, 0, 0, 0, 0);
    }
}

//=============================================================================
// Implémentation des méthodes utilitaires
//=============================================================================

uint32_t MidiMapper::makeCompositeKey(ControlId controlId, ControlType type) {
    return static_cast<uint32_t>(controlId) << 8 | static_cast<uint8_t>(type);
}

SendMidiCCCommand& MidiMapper::getNextCCCommand() {
    SendMidiCCCommand& cmd = midiCCCommandPool_[nextCCCommandIndex_];
    nextCCCommandIndex_ = (nextCCCommandIndex_ + 1) % COMMAND_POOL_SIZE;
    return cmd;
}

SendMidiNoteCommand& MidiMapper::getNextNoteCommand() {
    SendMidiNoteCommand& cmd = midiNoteCommandPool_[nextNoteCommandIndex_];
    nextNoteCommandIndex_ = (nextNoteCommandIndex_ + 1) % COMMAND_POOL_SIZE;
    return cmd;
}

void MidiMapper::logDiagnostic(const char* format, ...) const {
#ifndef PERFORMANCE_MODE
    char buffer[100];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    DIAG_ON_EVENT(buffer);
#endif
}

bool MidiMapper::isNavigationControl(ControlId controlId) const {
    // Utiliser MappingConfiguration pour savoir si c'est un contrôle de navigation
    static MappingConfiguration mappingConfig;
    return mappingConfig.isNavigationControl(controlId);
}

//=============================================================================
// Gestion des mappings
//=============================================================================

void MidiMapper::setMapping(ControlId controlId, const MidiControl& midiControl,
                            std::unique_ptr<IMidiMappingStrategy> strategy) {
    // Créer une nouvelle info de mapping
    MappingInfo info;
    info.control = midiControl;
    info.strategy = std::move(strategy);
    info.lastMidiValue = 0;
    info.lastEncoderPosition = 0;  // Initialisation à 0
    info.midiOffset = 0;

    // Créer une clé composite qui inclut le type de contrôle
    uint32_t compositeKey = makeCompositeKey(controlId, midiControl.controlType);

    // Supprimer l'ancien mapping s'il existe
    auto it = mappings_.find(compositeKey);
    if (it != mappings_.end()) {
        mappings_.erase(it);
    }

    // Ajouter le nouveau mapping
    mappings_[compositeKey] = std::move(info);

    logDiagnostic("Mapping ajouté: ID=%d CH=%d CC=%d Type=%d",
                  controlId,
                  midiControl.channel,
                  midiControl.control,
                  static_cast<int>(midiControl.controlType));
}

bool MidiMapper::removeMapping(ControlId controlId) {
    // Nous devons vérifier tous les types de contrôle possibles
    bool removed = false;

    // Essayer de supprimer le mapping pour l'encodeur
    uint32_t encoderKey = makeCompositeKey(controlId, ControlType::ENCODER_ROTATION);
    auto encoderIt = mappings_.find(encoderKey);
    if (encoderIt != mappings_.end()) {
        mappings_.erase(encoderIt);
        removed = true;
    }

    // Essayer de supprimer le mapping pour le bouton d'encodeur
    uint32_t buttonKey = makeCompositeKey(controlId, ControlType::ENCODER_BUTTON);
    auto buttonIt = mappings_.find(buttonKey);
    if (buttonIt != mappings_.end()) {
        mappings_.erase(buttonIt);
        removed = true;
    }

    if (removed) {
        logDiagnostic("Mapping supprimé: ID=%d", controlId);
    }

    return removed;
}

bool MidiMapper::hasMapping(ControlId controlId) const {
    // Vérifier tous les types de contrôle possibles
    uint32_t encoderKey = makeCompositeKey(controlId, ControlType::ENCODER_ROTATION);
    if (mappings_.find(encoderKey) != mappings_.end()) {
        return true;
    }

    uint32_t buttonKey = makeCompositeKey(controlId, ControlType::ENCODER_BUTTON);
    if (mappings_.find(buttonKey) != mappings_.end()) {
        return true;
    }

    return false;
}

const MidiControl& MidiMapper::getMidiControl(ControlId controlId) const {
    // Vérifier pour encoder en premier
    uint32_t encoderKey = makeCompositeKey(controlId, ControlType::ENCODER_ROTATION);
    auto encoderIt = mappings_.find(encoderKey);
    if (encoderIt != mappings_.end()) {
        return encoderIt->second.control;
    }

    // Puis vérifier pour bouton d'encodeur
    uint32_t buttonKey = makeCompositeKey(controlId, ControlType::ENCODER_BUTTON);
    auto buttonIt = mappings_.find(buttonKey);
    if (buttonIt != mappings_.end()) {
        return buttonIt->second.control;
    }

    return defaultControl_;
}

//=============================================================================
// Méthodes de traitement des encodeurs
//=============================================================================

bool MidiMapper::shouldProcessEncoder(EncoderId encoderId, int32_t position) {
    // Cette méthode centralise désormais toute la logique de limitation de taux
    // et de détection de doublons pour les encodeurs dans le système.

    // Limiteur de taux pour les messages d'encodeur
    static unsigned long lastSendTimePerEncoder[256] = {0};  // Temps du dernier envoi par encodeur
    unsigned long currentTime = millis();
    lastSendTimePerEncoder[encoderId] = currentTime;

    // Diagnostic: Message MIDI accepté
    Serial.printf("MIDI Rate: Accepting EncoderId=%d Position=%ld (waited %lu ms)\n",
                  encoderId,
                  position,
                  currentTime - lastSendTimePerEncoder[encoderId] + ENCODER_RATE_LIMIT_MS);
    return true;
}

int32_t MidiMapper::applyEncoderSensitivity(int32_t delta, EncoderId encoderId) {
    // Appliquer le facteur de sensibilité global
    float sensitivity = GlobalSettings::getInstance().getEncoderSensitivity();

    logDiagnostic("Pre-sensibilité: Enc=%d Delta=%ld", encoderId, delta);

    if (sensitivity != 1.0f) {
        // Appliquer la sensibilité tout en préservant le signe
        int32_t delta_sign = (delta > 0) ? 1 : -1;
        int32_t delta_abs = abs(delta);

        // Appliquer la sensibilité et assurer qu'un mouvement réel produit au moins 1 delta
        int32_t scaled_delta_abs = static_cast<int32_t>(delta_abs * sensitivity);
        if (scaled_delta_abs == 0 && delta_abs > 0) scaled_delta_abs = 1;

        // Reconstruire delta avec son signe
        int32_t new_delta = delta_sign * scaled_delta_abs;

        logDiagnostic("Post-sensibilité: Enc=%d Delta=%ld->%ld Sens=%.2f",
                      encoderId,
                      delta,
                      new_delta,
                      sensitivity);

        return new_delta;
    }

    return delta;
}

int16_t MidiMapper::calculateMidiValue(MappingInfo& info, int32_t delta, int32_t position) {
    const MidiControl& control = info.control;
    int16_t newValue;

    if (control.isRelative) {
        // Mode relatif avec sensibilité configurable
        float sensitivity =
            ConfigDefaults::DEFAULT_ENCODER_SENSITIVITY;  // Ajustez cette valeur selon vos besoins
        newValue = info.lastMidiValue + (int)(delta * sensitivity);
    } else {
        // Mode absolu avec référentiel flottant et sensibilité
        int32_t adjustedPosition = position - info.midiOffset;

        // Si la position ajustée sort des limites MIDI, mettre à jour l'offset
        if (adjustedPosition < 0) {
            info.midiOffset += adjustedPosition;
            adjustedPosition = 0;
        } else if (adjustedPosition > 127) {
            info.midiOffset += (adjustedPosition - 127);
            adjustedPosition = 127;
        }

        newValue = adjustedPosition;
    }

    // Garantir que la valeur est dans la plage MIDI valide (0-127)
    return constrain(newValue, 0, 127);
}

void MidiMapper::processEncoderChange(EncoderId encoderId, int32_t position) {
    // Note: Depuis la refactorisation, MidiMapper est responsable de tout le traitement des
    // encodeurs, y compris la limitation de taux, le suivi des positions et la détection des
    // doublons.

    // Vérifier si l'encodeur doit être traité (limitation de taux)
    if (!shouldProcessEncoder(encoderId, position)) {
        return;
    }

    // Si c'est un contrôle de navigation, ne pas envoyer de MIDI
    if (isNavigationControl(encoderId)) {
        return;
    }

    // Rechercher le mapping avec l'ID de l'encodeur et le type ENCODER_ROTATION
    uint32_t encoderKey = makeCompositeKey(encoderId, ControlType::ENCODER_ROTATION);
    auto it = mappings_.find(encoderKey);
    if (it == mappings_.end()) {
        logDiagnostic("No mapping found for encoder %d", encoderId);
        return;  // Pas de mapping pour cet encodeur
    }

    MappingInfo& info = it->second;
    const MidiControl& control = info.control;

    // Calculer le delta de mouvement
    int32_t delta = position - info.lastEncoderPosition;
    if (delta == 0) {
        return;  // Pas de changement
    }

    // Appliquer la sensibilité aux mouvements d'encodeur
    delta = applyEncoderSensitivity(delta, encoderId);

    // Mettre à jour la dernière position
    info.lastEncoderPosition = position;

    // Calculer la nouvelle valeur MIDI selon le mode
    int16_t newValue = calculateMidiValue(info, delta, position);

    // Ne rien faire si la valeur n'a pas changé
    if (newValue == info.lastMidiValue) {
        return;
    }

#ifndef PERFORMANCE_MODE
    logDiagnostic("Envoi MIDI: Enc=%d CH=%d CC=%d Val=%d (mode %s)",
                  encoderId,
                  control.channel,
                  control.control,
                  newValue,
                  control.isRelative ? "relatif" : "absolu");
    // Débogage pour voir l'ID de l'encodeur
    Serial.print(F("MidiMapper: Sending MIDI for encoderId="));
    Serial.println(encoderId);
#endif

    // Mettre à jour et envoyer la nouvelle valeur
    info.lastMidiValue = static_cast<uint8_t>(newValue);

    // Utiliser le pool de commandes
    SendMidiCCCommand& command = getNextCCCommand();
    command.reset(midiOut_,
                  control.channel,
                  control.control,
                  static_cast<uint8_t>(newValue),
                  encoderId);
    commandManager_.executeShared(command);
}

//=============================================================================
// Méthodes de traitement des boutons
//=============================================================================

void MidiMapper::processButtonEvent(ControlId buttonId, bool pressed, ControlType type) {
    // Si c'est un contrôle de navigation, ne pas traiter en MIDI mais laisser passer
    if (isNavigationControl(buttonId)) {
        Serial.print(F("Navigation control detected: "));
        Serial.print(buttonId);
        Serial.print(F(" pressed: "));
        Serial.println(pressed);
        // Ne pas traiter en MIDI - laisser ViewManagerEventListener s'en occuper
        return;
    }

    // Créer une clé composite à partir de l'ID et du type
    uint32_t buttonKey = makeCompositeKey(buttonId, type);

    auto it = mappings_.find(buttonKey);
    if (it == mappings_.end()) {
        const char* typeStr = (type == ControlType::ENCODER_BUTTON) ? "encoder button" : "button";
        logDiagnostic("No mapping found for %s %d", typeStr, buttonId);
        return;  // Pas de mapping pour ce bouton
    }

    MappingInfo& info = it->second;
    const MidiControl& control = info.control;

    // Pour les boutons, on utilise des notes MIDI au lieu de CC
    uint8_t velocity = pressed ? 127 : 0;

    const char* typeStr = (type == ControlType::ENCODER_BUTTON) ? "Bouton encodeur" : "Bouton";
    logDiagnostic("%s MIDI: ID=%d CH=%d Note=%d Vel=%d",
                  typeStr,
                  buttonId,
                  control.channel,
                  control.control,
                  velocity);

    // Utiliser le pool d'objets pour créer la commande
    SendMidiNoteCommand& command = getNextNoteCommand();
    command.reset(midiOut_, control.channel, control.control, velocity);

    // Si c'est une note on, la stocker pour pouvoir la couper plus tard
    if (pressed) {
        // Pour les notes actives, nous gardons toujours des objets gérés par smart pointers
        // car elles doivent persister longtemps
        auto noteCmd = std::make_unique<SendMidiNoteCommand>(midiOut_,
                                                             control.channel,
                                                             control.control,
                                                             velocity);
        activeNotes_[buttonId] = std::move(noteCmd);
        activeNotes_[buttonId]->execute();
    } else {
        // Si c'est une note off, utiliser la commande et la supprimer
        auto noteIt = activeNotes_.find(buttonId);
        if (noteIt != activeNotes_.end()) {
            noteIt->second->undo();  // Undo = Note Off
            activeNotes_.erase(noteIt);
        } else {
            // Si pas de note active, simplement exécuter la commande du pool
            commandManager_.executeShared(command);
        }
    }
}

void MidiMapper::processEncoderButton(EncoderId encoderId, bool pressed) {
    processButtonEvent(encoderId, pressed, ControlType::ENCODER_BUTTON);
}

void MidiMapper::processButtonPress(ButtonId buttonId, bool pressed) {
    processButtonEvent(buttonId, pressed, ControlType::BUTTON);
}

//=============================================================================
// Méthodes de maintenance et mises à jour
//=============================================================================

void MidiMapper::update() {
    // Mettre à jour les commandes de notes actives
    for (auto& pair : activeNotes_) {
        pair.second->update();
    }

    // Supprimer les notes qui ne sont plus actives
    for (auto it = activeNotes_.begin(); it != activeNotes_.end();) {
        if (!it->second->isNoteActive()) {
            logDiagnostic("Note supprimée: ID=%d", it->first);
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}

//=============================================================================
// Gestion des événements
//=============================================================================

bool MidiMapper::onEvent(const Event& event) {
    // Traiter les différents types d'événements
    switch (event.getType()) {
    case EventTypes::HighPriorityEncoderChanged: {
        const HighPriorityEncoderChangedEvent& encEvent =
            static_cast<const HighPriorityEncoderChangedEvent&>(event);
        processEncoderChange(encEvent.encoderId, encEvent.position);
        return true;
    }

    case EventTypes::HighPriorityEncoderButton: {
        const HighPriorityEncoderButtonEvent& buttonEvent =
            static_cast<const HighPriorityEncoderButtonEvent&>(event);
        processEncoderButton(buttonEvent.encoderId, buttonEvent.pressed);
        return true;
    }

    case EventTypes::HighPriorityButtonPress: {
        const HighPriorityButtonPressEvent& buttonEvent =
            static_cast<const HighPriorityButtonPressEvent&>(event);
        processButtonPress(buttonEvent.buttonId, buttonEvent.pressed);
        return true;
    }

    // Gérer aussi les types standard d'événements pour la compatibilité
    case EventTypes::EncoderTurned: {
        const EncoderTurnedEvent& encEvent = static_cast<const EncoderTurnedEvent&>(event);
        processEncoderChange(encEvent.id, encEvent.position);
        return true;
    }

    case EventTypes::EncoderButton: {
        const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
        processEncoderButton(buttonEvent.id, buttonEvent.pressed);
        return true;
    }

    case EventTypes::ButtonPressed: {
        const ButtonPressedEvent& buttonEvent = static_cast<const ButtonPressedEvent&>(event);
        processButtonPress(buttonEvent.id, true);
        return true;
    }

    case EventTypes::ButtonReleased: {
        const ButtonReleasedEvent& buttonEvent = static_cast<const ButtonReleasedEvent&>(event);
        processButtonPress(buttonEvent.id, false);
        return true;
    }
    }

    return false;  // Evénement non traité
}