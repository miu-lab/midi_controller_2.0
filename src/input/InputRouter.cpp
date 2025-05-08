#include "input/InputRouter.hpp"
#include "input/InputEvent.hpp"
#include "utils/EventBus.hpp"
#include <map>

InputRouter::InputRouter(IMidiOut& midiOut, IProfileManager& profileManager)
    : midiOut_(midiOut), profileManager_(profileManager) {}

void InputRouter::init() {
    // Initialisation sans service de navigation
    navigationService_ = nullptr;
    subscribeToEvents();
}

void InputRouter::init(NavigationConfigService& navService) {
    // Initialisation avec service de navigation
    navigationService_ = &navService;
    subscribeToEvents();
}

void InputRouter::subscribeToEvents() {
    // Tables pour mémoriser les états des encodeurs
    static std::map<ControlId, int32_t> lastEncoderPos;  // Dernière position absolue
    static std::map<ControlId, int16_t> lastMidiValues;  // Dernière valeur MIDI envoyée
    
    EventBus<EncoderTurnedEvent>::subscribe([this](const EncoderTurnedEvent& e) {
        // Vérifier si l'encodeur est utilisé pour la navigation
        if (navigationService_ && navigationService_->isNavigationControl(e.id)) {
            // Ne pas envoyer d'événement MIDI pour les contrôles de navigation
            return;
        }
        
        auto binding = profileManager_.getBinding(e.id);
        if (binding) {
            // Récupérer la dernière position connue de cet encodeur
            int32_t prevPos = 0;
            if (lastEncoderPos.count(e.id) > 0) {
                prevPos = lastEncoderPos[e.id];
            }
            
            // Mémoriser la position actuelle
            lastEncoderPos[e.id] = e.absolutePosition;
            
            // Calculer le delta de mouvement
            int32_t delta = e.absolutePosition - prevPos;
            
            // Récupérer la dernière valeur MIDI envoyée
            int16_t currentValue = 0;
            if (lastMidiValues.count(e.id) > 0) {
                currentValue = lastMidiValues[e.id];
            }
            
            // Calculer la nouvelle valeur MIDI en fonction du delta
            int16_t newValue;
            if (binding->relative) {
                // En mode relatif, on ajoute le delta mis à l'échelle
                newValue = currentValue + (delta * 2);
            } else {
                // En mode absolu, on modifie la valeur actuelle en fonction du delta
                newValue = currentValue + delta;
            }
            
            // Clamper entre 0 et 127
            if (newValue < 0) newValue = 0;
            if (newValue > 127) newValue = 127;
            
            // Mémoriser et envoyer uniquement si la valeur a changé
            if (newValue != currentValue) {
                lastMidiValues[e.id] = newValue;
                midiOut_.sendCc(binding->channel, binding->control, static_cast<uint8_t>(newValue));
            }
        }
    });

    EventBus<ButtonPressed>::subscribe([this](const ButtonPressed& e) {
        // Vérifier si le bouton est utilisé pour la navigation
        if (navigationService_ && navigationService_->isNavigationControl(e.id)) {
            // Ne pas envoyer d'événement MIDI pour les contrôles de navigation
            return;
        }
        
        auto binding = profileManager_.getBinding(e.id);
        if (binding) {
            midiOut_.sendNoteOn(binding->channel, binding->control, 127);
        }
    });

    EventBus<ButtonReleased>::subscribe([this](const ButtonReleased& e) {
        // Vérifier si le bouton est utilisé pour la navigation
        if (navigationService_ && navigationService_->isNavigationControl(e.id)) {
            // Ne pas envoyer d'événement MIDI pour les contrôles de navigation
            return;
        }
        
        auto binding = profileManager_.getBinding(e.id);
        if (binding) {
            midiOut_.sendNoteOff(binding->channel, binding->control, 0);
        }
    });
}
