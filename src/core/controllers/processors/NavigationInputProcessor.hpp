#pragma once

#include "BaseInputProcessor.hpp"
#include "core/domain/navigation/NavigationEvent.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Processor spécialisé pour les entrées de navigation
 * 
 * Gère le routage des entrées physiques vers les événements de navigation.
 */
class NavigationInputProcessor : public BaseInputProcessor {
public:
    NavigationInputProcessor(std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                           std::shared_ptr<EventBus> eventBus)
        : BaseInputProcessor(unifiedConfig, eventBus) {}
    
    /**
     * @brief Traite un encodeur pour navigation
     */
    bool processEncoder(EncoderId id, int32_t absolutePosition, int8_t relativeChange) {
        if (!isValidContext()) {
            return false;
        }
        
        auto controlOpt = findControlDefinition(id);
        if (!controlOpt.has_value()) {
            return false;
        }
        
        const auto& control = controlOpt.value();
        if (!hasNavigationMappings(control)) {
            return false;
        }
        
        NavigationAction action = extractNavigationAction(control, true);
        int parameter = calculateEncoderParameter(control, relativeChange);
        
        emitNavigationEvent(action, parameter);
        return true;
    }
    
    /**
     * @brief Traite un bouton pour navigation
     */
    bool processButton(ButtonId id, bool pressed) {
        if (!isValidContext()) {
            return false;
        }
        
        // Traiter seulement les appuis selon configuration
        if (!pressed && SystemConstants::Buttons::PROCESS_PRESS_ONLY) {
            return false;
        }
        
        auto controlOpt = findControlDefinition(id);
        if (!controlOpt.has_value()) {
            return false;
        }
        
        const auto& control = controlOpt.value();
        if (!hasNavigationMappings(control)) {
            return false;
        }
        
        NavigationAction action = extractNavigationAction(control, false);
        int parameter = SystemConstants::Buttons::DEFAULT_PARAMETER;
        
        emitNavigationEvent(action, parameter);
        return true;
    }

private:
    NavigationAction extractNavigationAction(const ControlDefinition& control, bool isEncoder) const {
        auto navigationMappings = control.getMappingsForRole(MappingRole::NAVIGATION);
        
        for (const auto& mapping : navigationMappings) {
            bool mappingForEncoder = (mapping.appliesTo == MappingControlType::ENCODER);
            if (mappingForEncoder == isEncoder) {
                const auto& navConfig = std::get<ControlDefinition::NavigationConfig>(mapping.config);
                return navConfig.action;
            }
        }
        
        // Actions par défaut depuis les constantes
        return isEncoder ? SystemConstants::Encoders::DEFAULT_ACTION 
                        : SystemConstants::Buttons::DEFAULT_ACTION;
    }
    
    int calculateEncoderParameter(const ControlDefinition& control, int8_t relativeChange) const {
        if (control.hardware.type != InputType::ENCODER) {
            return SystemConstants::Encoders::DEFAULT_PARAMETER;
        }
        
        const auto& encoderConfig = std::get<ControlDefinition::EncoderConfig>(control.hardware.config);
        return applySensitivity(relativeChange, encoderConfig.sensitivity);
    }
    
    void emitNavigationEvent(NavigationAction action, int parameter) {
        if (!eventBus_) {
            return;
        }
        
        NavigationEvent event(action, parameter);
        eventBus_->publish(event);
    }
};