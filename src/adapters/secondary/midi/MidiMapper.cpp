#include "adapters/secondary/midi/MidiMapper.hpp"

#include <Arduino.h>  // Pour la fonction constrain

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
    info.rangeMinPosition = 0;
    info.rangeMaxPosition = 127;
    info.rangeMapped = false;
    info.movingUp = false;
    info.movingDown = false;

    // Supprimer l'ancien mapping s'il existe
    auto it = mappings_.find(controlId);
    if (it != mappings_.end()) {
        mappings_.erase(it);
    }

    // Ajouter le nouveau mapping
    mappings_[controlId] = std::move(info);
}

bool MidiMapper::removeMapping(ControlId controlId) {
    auto it = mappings_.find(controlId);
    if (it != mappings_.end()) {
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
    auto it = mappings_.find(encoderId);
    if (it == mappings_.end()) {
        return;  // Pas de mapping pour cet encodeur
    }

    MappingInfo& info = it->second;
    const MidiControl& control = info.control;

    // Calculer le delta de mouvement
    int32_t delta = position - info.lastEncoderPosition;
    if (delta == 0) return;  // Pas de changement

    // Détecter changement de direction
    bool wasMovingUp = info.movingUp;
    bool wasMovingDown = info.movingDown;
    info.movingUp = (delta > 0);
    info.movingDown = (delta < 0);
    bool directionChanged = (wasMovingUp && info.movingDown) || (wasMovingDown && info.movingUp);

    // Remapper la plage si changement de direction hors limites MIDI
    if (directionChanged) {
        if (position < 0) {  // Changement de direction en zone négative
            info.rangeMinPosition = position;
            info.rangeMaxPosition = 127 + (0 - position);
            info.rangeMapped = true;
        } else if (position > 127) {  // Changement de direction au-delà de 127
            info.rangeMaxPosition = position;
            info.rangeMinPosition = 0 - (position - 127);
            info.rangeMapped = true;
        }
    }

    // Mettre à jour la dernière position
    info.lastEncoderPosition = position;

    // Calcul de la nouvelle valeur MIDI
    int16_t newValue;

    if (control.relative) {
        // Mode relatif: ajustement direct par le delta
        newValue = info.lastMidiValue + delta;
    } else {
        // Mode absolu avec gestion des plages
        if (info.rangeMapped) {
            // Utiliser la plage remappée
            if (position <= info.rangeMinPosition) {
                newValue = 0;
            } else if (position >= info.rangeMaxPosition) {
                newValue = 127;
            } else {
                // Conversion proportionnelle
                float ratio = (float)(position - info.rangeMinPosition) /
                              (info.rangeMaxPosition - info.rangeMinPosition);
                newValue = static_cast<int16_t>(ratio * 127.0f);
            }
        } else {
            // Utiliser la plage standard 0-127
            newValue = constrain(position, 0, 127);
        }
    }

    // Garantir que la valeur est dans la plage MIDI valide
    newValue = constrain(newValue, 0, 127);

    // Ne rien faire si la valeur n'a pas changé
    if (newValue == info.lastMidiValue) return;

    // Mettre à jour et envoyer la nouvelle valeur
    info.lastMidiValue = static_cast<uint8_t>(newValue);
    auto command = std::make_unique<SendMidiCCCommand>(midiOut_, control.channel, control.control,
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
            it = activeNotes_.erase(it);
        } else {
            ++it;
        }
    }
}
