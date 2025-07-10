#include "InputController.hpp"

#include "core/domain/events/MidiEvents.hpp"

InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                                std::shared_ptr<EventBus> eventBus)
    : navigationConfig_(navigationConfig)
    , unifiedConfig_(unifiedConfig)
    , eventBus_(eventBus) {
}

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    // Vérifier si c'est un contrôle de navigation
    if (navigationConfig_ && navigationConfig_->isNavigationControl(id)) {
        handleNavigationInput(id, true, relativeChange);
        return;
    }
    
    // Vérifier dans la configuration unifiée
    if (isNavigationControlInConfig(id)) {
        handleNavigationInput(id, true, relativeChange);
        return;
    }
    
    // Sinon, émettre événement MIDI
    emitMidiEvent(id, true, absolutePosition, relativeChange);
}

void InputController::processButtonPress(ButtonId id, bool pressed) {
    // Vérifier si c'est un contrôle de navigation via UnifiedConfiguration
    if (unifiedConfig_ && isNavigationControlInConfig(id)) {
        if (pressed) {  // Traiter seulement les appuis, pas les relâchements
            handleNavigationInput(id, pressed);
        }
        return;
    }
    
    // Sinon, émettre événement MIDI
    emitMidiEvent(id, false, 0, 0, pressed);
}

void InputController::handleNavigationInput(InputId id, bool isPressed, int8_t relativeChange) {
    if (!unifiedConfig_) {
        return;
    }
    
    // Trouver la définition du contrôle
    auto controlOpt = unifiedConfig_->findControlById(id);
    if (!controlOpt.has_value()) {
        return;
    }
    
    const ControlDefinition& control = controlOpt.value();
    
    // Extraire l'action de navigation
    bool isEncoder = (control.hardware.type == InputType::ENCODER);
    NavigationAction action = extractNavigationAction(control, isEncoder);
    
    // Déterminer le paramètre
    int parameter = determineNavigationParameter(control, relativeChange);
    
    // Émettre l'événement de navigation
    emitNavigationEvent(action, parameter);
}

NavigationAction InputController::extractNavigationAction(const ControlDefinition& control, bool isEncoder) const {
    // Chercher les mappings de navigation
    auto navigationMappings = control.getMappingsForRole(MappingRole::NAVIGATION);
    
    for (const auto& mapping : navigationMappings) {
        // Vérifier que le mapping s'applique au bon type de contrôle
        bool mappingForEncoder = (mapping.appliesTo == MappingControlType::ENCODER);
        if (mappingForEncoder == isEncoder) {
            const auto& navConfig = std::get<ControlDefinition::NavigationConfig>(mapping.config);
            return navConfig.action;
        }
    }
    
    // Aucune action trouvée, retourner une action par défaut selon le type
    return isEncoder ? NavigationAction::ITEM_NAVIGATOR : NavigationAction::ITEM_VALIDATE;
}

int InputController::determineNavigationParameter(const ControlDefinition& control, int8_t relativeChange) const {
    // Pour les encodeurs, appliquer la sensibilité configurée comme pour MIDI
    if (control.hardware.type == InputType::ENCODER) {
        // Récupérer la configuration encodeur
        const auto& encoderConfig = std::get<ControlDefinition::EncoderConfig>(control.hardware.config);
        
        // Appliquer la sensibilité de la même manière que MidiMapper
        float sensitivity = encoderConfig.sensitivity;
        
        if (sensitivity != 1.0f) {
            // Appliquer la sensibilité tout en préservant le signe
            int32_t delta_sign = (relativeChange > 0) ? 1 : -1;
            int32_t delta_abs = abs(relativeChange);
            
            // Appliquer la sensibilité et assurer qu'un mouvement réel produit au moins 1 delta
            int32_t scaled_delta_abs = static_cast<int32_t>(delta_abs * sensitivity);
            if (scaled_delta_abs == 0 && delta_abs > 0) scaled_delta_abs = 1;
            
            int32_t result = scaled_delta_abs * delta_sign;
            
            // Limiter à la plage d'un int pour éviter les débordements
            return (result > INT_MAX) ? INT_MAX : (result < INT_MIN) ? INT_MIN : static_cast<int>(result);
        }
        
        return static_cast<int>(relativeChange);
    }
    
    // Pour les boutons, pas de paramètre par défaut
    return 0;
}

void InputController::emitNavigationEvent(NavigationAction action, int parameter) {
    if (!eventBus_) {
        return;
    }
    
    // Créer et publier l'événement de navigation
    NavigationEvent event(action, parameter);
    eventBus_->publish(event);
}

bool InputController::isNavigationControlInConfig(InputId id) const {
    if (!unifiedConfig_) {
        return false;
    }
    
    // Trouver la définition du contrôle
    auto controlOpt = unifiedConfig_->findControlById(id);
    if (!controlOpt.has_value()) {
        return false;
    }
    
    const ControlDefinition& control = controlOpt.value();
    
    // Vérifier s'il a des mappings de navigation
    auto navigationMappings = control.getMappingsForRole(MappingRole::NAVIGATION);
    
    return !navigationMappings.empty();
}

void InputController::emitMidiEvent(InputId id, bool isEncoder, int32_t absolutePosition, 
                                   int8_t relativeChange, bool pressed) {
    if (!eventBus_) {
        return;
    }
    
    if (isEncoder) {
        // Événement d'encodeur MIDI
        HighPriorityEncoderChangedEvent event(id, absolutePosition, relativeChange);
        eventBus_->publish(event);
    } else {
        // Événement de bouton MIDI
        HighPriorityButtonPressEvent event(id, pressed);
        eventBus_->publish(event);
    }
}