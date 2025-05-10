#include "adapters/secondary/midi/MidiMapper.hpp"

#include <Arduino.h>  // Pour la fonction constrain

#include "config/GlobalSettings.hpp"
#include "tools/Diagnostics.hpp"

MidiMapper::MidiMapper(IMidiOut& midiOut, CommandManager& commandManager)
    : midiOut_(midiOut),
      commandManager_(commandManager),
      defaultControl_({0, 0, false})  // Canal 1, CC 0, mode absolu
{}

void MidiMapper::setMapping(ControlId controlId, const MidiControl& midiControl,
                            std::unique_ptr<IMidiMappingStrategy> strategy) {
    // Créer une nouvelle info de mapping
    MappingInfo info;
    info.control = midiControl;
    info.strategy = std::move(strategy);
    info.lastMidiValue = 0;
    info.lastEncoderPosition = 0;  // Initialisation à 0
    info.midiOffset = 0;

    // Supprimer l'ancien mapping s'il existe
    auto it = mappings_.find(controlId);
    if (it != mappings_.end()) {
        mappings_.erase(it);
    }

    // Ajouter le nouveau mapping
    mappings_[controlId] = std::move(info);
    
    // Diagnostic d'ajout de mapping
    char eventName[40];
    snprintf(eventName, sizeof(eventName), "Mapping ajouté: ID=%d CH=%d CC=%d",
             controlId, midiControl.channel, midiControl.control);
    DIAG_ON_EVENT(eventName);
}

bool MidiMapper::removeMapping(ControlId controlId) {
    auto it = mappings_.find(controlId);
    if (it != mappings_.end()) {
        // Diagnostic de suppression de mapping
        char eventName[40];
        snprintf(eventName, sizeof(eventName), "Mapping supprimé: ID=%d", controlId);
        DIAG_ON_EVENT(eventName);
        
        mappings_.erase(it);
        return true;
    }
    return false;
}

bool MidiMapper::hasMapping(ControlId controlId) const {
    return mappings_.find(controlId) != mappings_.end();
}

const MidiControl& MidiMapper::getMidiControl(ControlId controlId) const {
    auto it = mappings_.find(controlId);
    if (it != mappings_.end()) {
        return it->second.control;
    }
    return defaultControl_;
}

void MidiMapper::processEncoderChange(EncoderId encoderId, int32_t position) {
    // On utilise une variable statique pour suivre les derniers encodeurs traités
    // afin d'éviter les duplications
    static EncoderId lastEncoderId = 255;  // Valeur impossible
    static int32_t lastPosition = -9999;   // Valeur impossible
    static unsigned long lastProcessTime = 0;

    // Si le même encodeur avec la même position a été traité récemment (dans les 20ms),
    // on ignore ce traitement pour éviter les duplications
    unsigned long currentTime = millis();
    if (encoderId == lastEncoderId && position == lastPosition &&
        currentTime - lastProcessTime < 20) {
        return;  // Ignorer ce traitement car c'est un doublon
    }

    // Mettre à jour les variables de tracking
    lastEncoderId = encoderId;
    lastPosition = position;
    lastProcessTime = currentTime;

    // Continuer avec le traitement normal
    auto it = mappings_.find(encoderId);
    if (it == mappings_.end()) {
        return;  // Pas de mapping pour cet encodeur
    }

    MappingInfo& info = it->second;
    const MidiControl& control = info.control;

    // Calculer le delta de mouvement
    int32_t delta = position - info.lastEncoderPosition;
    if (delta == 0) return;  // Pas de changement

    // Diagnostic avant application de la sensibilité
    char preSensitivityEvent[50];
    snprintf(preSensitivityEvent, sizeof(preSensitivityEvent), 
             "Pre-sensibilité: Enc=%d Delta=%ld", encoderId, delta);
    DIAG_ON_EVENT(preSensitivityEvent);

    // Appliquer le facteur de sensibilité global si ce n'est pas un contrôle de navigation
    if (!isNavigationControl(encoderId) && delta != 0) {
        float sensitivity = GlobalSettings::getInstance().getEncoderSensitivity();
        if (sensitivity != 1.0f) {
            // Appliquer la sensibilité tout en préservant le signe
            int32_t delta_sign = (delta > 0) ? 1 : -1;
            int32_t delta_abs = abs(delta);

            // Appliquer la sensibilité et assurer qu'un mouvement réel produit au moins 1 delta
            int32_t scaled_delta_abs = static_cast<int32_t>(delta_abs * sensitivity);
            if (scaled_delta_abs == 0 && delta_abs > 0) scaled_delta_abs = 1;

            // Reconstruire delta avec son signe
            int32_t new_delta = delta_sign * scaled_delta_abs;
            
            // Diagnostic après application de la sensibilité
            char sensitivityEvent[60];
            snprintf(sensitivityEvent, sizeof(sensitivityEvent), 
                     "Post-sensibilité: Enc=%d Delta=%ld->%ld Sens=%.2f", 
                     encoderId, delta, new_delta, sensitivity);
            DIAG_ON_EVENT(sensitivityEvent);
            
            delta = new_delta;
        }
    }

    // Mettre à jour la dernière position
    info.lastEncoderPosition = position;

    // Calcul de la nouvelle valeur MIDI
    int16_t newValue;

    if (control.relative) {
        // Mode relatif: chaque pas d'encodeur = un pas MIDI (sensibilité constante)
        newValue = info.lastMidiValue + delta;
    } else {
        // Mode absolu avec référentiel flottant
        int32_t adjustedPosition = position - info.midiOffset;

        // Si la position ajustée sort des limites MIDI, mettre à jour l'offset
        if (adjustedPosition < 0) {
            // Mettre à jour l'offset pour ramener la position à 0
            info.midiOffset += adjustedPosition;  // adjustedPosition est négatif
            adjustedPosition = 0;
        } else if (adjustedPosition > 127) {
            // Mettre à jour l'offset pour ramener la position à 127
            info.midiOffset += (adjustedPosition - 127);
            adjustedPosition = 127;
        }

        newValue = adjustedPosition;
    }

    // Garantir que la valeur est dans la plage MIDI valide (0-127)
    newValue = constrain(newValue, 0, 127);

    // Ne rien faire si la valeur n'a pas changé
    if (newValue == info.lastMidiValue) return;

    // Diagnostic de la valeur MIDI envoyée
    char midiEvent[60];
    snprintf(midiEvent, sizeof(midiEvent), 
             "Envoi MIDI: Enc=%d CH=%d CC=%d Val=%d (mode %s)", 
             encoderId, control.channel, control.control, newValue,
             control.relative ? "relatif" : "absolu");
    DIAG_ON_EVENT(midiEvent);

    // Mettre à jour et envoyer la nouvelle valeur
    info.lastMidiValue = static_cast<uint8_t>(newValue);

    auto command = std::make_unique<SendMidiCCCommand>(midiOut_,
                                                       control.channel,
                                                       control.control,
                                                       static_cast<uint8_t>(newValue));
    commandManager_.execute(std::move(command));
}

void MidiMapper::processEncoderButton(EncoderId encoderId, bool pressed) {
    auto it = mappings_.find(encoderId);
    if (it == mappings_.end()) {
        return;  // Pas de mapping pour ce bouton d'encodeur
    }

    MappingInfo& info = it->second;
    const MidiControl& control = info.control;

    // Pour les boutons, on utilise des notes MIDI au lieu de CC
    uint8_t velocity = pressed ? 127 : 0;

    // Diagnostic du bouton d'encodeur
    char buttonEvent[60];
    snprintf(buttonEvent, sizeof(buttonEvent), 
             "Bouton encodeur MIDI: ID=%d CH=%d Note=%d Vel=%d", 
             encoderId, control.channel, control.control, velocity);
    DIAG_ON_EVENT(buttonEvent);

    // Créer et exécuter la commande
    auto command =
        std::make_unique<SendMidiNoteCommand>(midiOut_, control.channel, control.control, velocity);

    // Si c'est une note on, la stocker pour pouvoir la couper plus tard
    if (pressed) {
        activeNotes_[encoderId] = std::move(command);
        activeNotes_[encoderId]->execute();
    } else {
        // Si c'est une note off, utiliser la commande et la supprimer
        auto noteIt = activeNotes_.find(encoderId);
        if (noteIt != activeNotes_.end()) {
            noteIt->second->undo();  // Undo = Note Off
            activeNotes_.erase(noteIt);
        } else {
            // Si pas de note active, simplement envoyer la commande
            commandManager_.execute(std::move(command));
        }
    }
}

void MidiMapper::processButtonPress(ButtonId buttonId, bool pressed) {
    auto it = mappings_.find(buttonId);
    if (it == mappings_.end()) {
        return;  // Pas de mapping pour ce bouton
    }

    MappingInfo& info = it->second;
    const MidiControl& control = info.control;

    // Pour les boutons, on utilise des notes MIDI au lieu de CC
    uint8_t velocity = pressed ? 127 : 0;

    // Diagnostic du bouton
    char buttonEvent[50];
    snprintf(buttonEvent, sizeof(buttonEvent), 
             "Bouton MIDI: ID=%d CH=%d Note=%d Vel=%d", 
             buttonId, control.channel, control.control, velocity);
    DIAG_ON_EVENT(buttonEvent);

    // Créer et exécuter la commande
    auto command =
        std::make_unique<SendMidiNoteCommand>(midiOut_, control.channel, control.control, velocity);

    // Si c'est une note on, la stocker pour pouvoir la couper plus tard
    if (pressed) {
        activeNotes_[buttonId] = std::move(command);
        activeNotes_[buttonId]->execute();
    } else {
        // Si c'est une note off, utiliser la commande et la supprimer
        auto noteIt = activeNotes_.find(buttonId);
        if (noteIt != activeNotes_.end()) {
            noteIt->second->undo();  // Undo = Note Off
            activeNotes_.erase(noteIt);
        } else {
            // Si pas de note active, simplement envoyer la commande
            commandManager_.execute(std::move(command));
        }
    }
}

void MidiMapper::update() {
    // Mettre à jour les commandes de notes actives
    for (auto& pair : activeNotes_) {
        pair.second->update();
    }

    // Supprimer les notes qui ne sont plus actives
    for (auto it = activeNotes_.begin(); it != activeNotes_.end();) {
        if (!it->second->isNoteActive()) {
            // Diagnostic de suppression de note
            char noteEvent[50];
            snprintf(noteEvent, sizeof(noteEvent), 
                     "Note supprimée: ID=%d", it->first);
            DIAG_ON_EVENT(noteEvent);
            
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}

bool MidiMapper::isNavigationControl(ControlId controlId) const {
    // Utiliser MappingConfiguration pour savoir si c'est un contrôle de navigation
    static MappingConfiguration mappingConfig;
    return mappingConfig.isNavigationControl(controlId);
}