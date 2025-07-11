#pragma once

#include <memory>
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/types.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Classe de base pour tous les processors d'input
 * 
 * Applique le Template Method Pattern pour factoriser la logique commune
 * de validation et de traitement d'entrées.
 */
class BaseInputProcessor {
public:
    /**
     * @brief Constructeur
     * @param unifiedConfig Configuration unifiée
     * @param eventBus Bus d'événements
     */
    BaseInputProcessor(std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                      std::shared_ptr<EventBus> eventBus)
        : unifiedConfig_(unifiedConfig), eventBus_(eventBus) {}
    
    /**
     * @brief Destructeur virtuel
     */
    virtual ~BaseInputProcessor() = default;

protected:
    /**
     * @brief Vérifie si le contexte est valide (Template Method)
     */
    virtual bool isValidContext() const {
        return unifiedConfig_ != nullptr && eventBus_ != nullptr;
    }
    
    /**
     * @brief Trouve la définition d'un contrôle
     */
    std::optional<ControlDefinition> findControlDefinition(InputId id) const {
        if (!unifiedConfig_) {
            return std::nullopt;
        }
        return unifiedConfig_->findControlById(id);
    }
    
    /**
     * @brief Vérifie si un contrôle a des mappings de navigation
     */
    bool hasNavigationMappings(const ControlDefinition& control) const {
        auto navigationMappings = control.getMappingsForRole(MappingRole::NAVIGATION);
        return !navigationMappings.empty();
    }
    
    /**
     * @brief Applique la sensibilité à un changement relatif
     */
    int applySensitivity(int8_t relativeChange, float sensitivity) const {
        if (sensitivity == SystemConstants::Encoders::DEFAULT_SENSITIVITY) {
            return static_cast<int>(relativeChange);
        }
        
        // Appliquer la sensibilité tout en préservant le signe
        int32_t deltaSign = (relativeChange > 0) ? 1 : -1;
        int32_t deltaAbs = abs(relativeChange);
        
        // Appliquer la sensibilité et assurer qu'un mouvement réel produit au moins 1 delta
        int32_t scaledDeltaAbs = static_cast<int32_t>(deltaAbs * sensitivity);
        if (scaledDeltaAbs == 0 && deltaAbs > 0) {
            scaledDeltaAbs = SystemConstants::Encoders::MIN_DELTA_THRESHOLD;
        }
        
        int32_t result = scaledDeltaAbs * deltaSign;
        
        // Protection contre les débordements
        if (SystemConstants::Validation::ENABLE_OVERFLOW_PROTECTION) {
            if (result > SystemConstants::Encoders::MAX_DELTA_VALUE) {
                return SystemConstants::Encoders::MAX_DELTA_VALUE;
            }
            if (result < SystemConstants::Encoders::MIN_DELTA_VALUE) {
                return SystemConstants::Encoders::MIN_DELTA_VALUE;
            }
        }
        
        return static_cast<int>(result);
    }

protected:
    std::shared_ptr<UnifiedConfiguration> unifiedConfig_;
    std::shared_ptr<EventBus> eventBus_;
};