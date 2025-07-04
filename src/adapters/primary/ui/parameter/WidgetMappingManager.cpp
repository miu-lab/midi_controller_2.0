#include "WidgetMappingManager.hpp"
#include <Arduino.h>

WidgetMappingManager::WidgetMappingManager(const MappingConfig& config)
    : config_(config)
    , initialized_(false) {
    reset();
}

void WidgetMappingManager::initializeMappings(const std::vector<MidiConfigurationParser::MidiControlInfo>& midiControls,
                                             const std::vector<MidiConfigurationParser::ButtonInfo>& buttonInfos) {
    logInfo("Initializing widget mappings");
    
    // Nettoyer les mappings existants
    reset();
    
    // Initialiser les mappings CC → widget
    initializeCCMapping(midiControls);
    
    // Initialiser les mappings boutons → widgets si activé
    if (config_.enableButtonMapping) {
        initializeButtonMapping(midiControls, buttonInfos);
    }
    
    initialized_ = true;
    
    // Logger les statistiques finales
    auto stats = getStats();
    logInfo("Mapping initialization complete - CC mapped: " + String(stats.ccMapped) + 
           ", Buttons mapped: " + String(stats.buttonsMapped) + 
           ", Standalone buttons: " + String(stats.buttonsStandalone));
}

int8_t WidgetMappingManager::getWidgetIndexForCC(uint8_t cc_number) const {
    if (cc_number >= 128) {
        return -1;
    }
    
    uint8_t index = cc_to_widget_mapping_[cc_number];
    return (index == 255) ? -1 : static_cast<int8_t>(index);
}

int8_t WidgetMappingManager::getWidgetIndexForButton(uint16_t button_id) const {
    auto it = button_to_widget_mapping_.find(button_id);
    if (it != button_to_widget_mapping_.end()) {
        return static_cast<int8_t>(it->second);
    }
    return -1;
}

const std::map<uint16_t, uint8_t>& WidgetMappingManager::getButtonToWidgetMapping() const {
    return button_to_widget_mapping_;
}

const std::vector<MidiConfigurationParser::ButtonInfo>& WidgetMappingManager::getStandaloneButtons() const {
    return standalone_buttons_;
}

bool WidgetMappingManager::isInitialized() const {
    return initialized_;
}

void WidgetMappingManager::reset() {
    // Réinitialiser le mapping CC (255 = non mappé)
    cc_to_widget_mapping_.fill(255);
    
    // Nettoyer les mappings de boutons
    button_to_widget_mapping_.clear();
    standalone_buttons_.clear();
    
    initialized_ = false;
    logDebug("Mappings reset");
}

WidgetMappingManager::MappingStats WidgetMappingManager::getStats() const {
    MappingStats stats;
    
    // Compter les CC mappés
    stats.ccMapped = 0;
    for (uint8_t mapping : cc_to_widget_mapping_) {
        if (mapping != 255) {
            stats.ccMapped++;
        }
    }
    
    // Compter les boutons mappés et standalone
    stats.buttonsMapped = button_to_widget_mapping_.size();
    stats.buttonsStandalone = standalone_buttons_.size();
    
    return stats;
}

void WidgetMappingManager::initializeCCMapping(const std::vector<MidiConfigurationParser::MidiControlInfo>& midiControls) {
    logDebug("Initializing CC → widget mapping");
    
    // Mapper les premiers contrôles aux widgets disponibles
    size_t widgetIndex = 0;
    for (const auto& control : midiControls) {
        if (widgetIndex >= config_.maxWidgets) {
            logDebug("Maximum widgets reached (" + String(config_.maxWidgets) + "), skipping remaining controls");
            break;
        }
        
        if (control.cc_number < 128) {
            cc_to_widget_mapping_[control.cc_number] = widgetIndex;
            logDebug("Mapped CC" + String(control.cc_number) + " → widget " + String(widgetIndex) + 
                    " (" + control.name + ")");
            widgetIndex++;
        } else {
            logDebug("Invalid CC number: " + String(control.cc_number) + ", skipping");
        }
    }
    
    logInfo("CC mapping complete - " + String(widgetIndex) + " controls mapped");
}

void WidgetMappingManager::initializeButtonMapping(const std::vector<MidiConfigurationParser::MidiControlInfo>& midiControls,
                                                  const std::vector<MidiConfigurationParser::ButtonInfo>& buttonInfos) {
    logDebug("Initializing button → widget mapping");
    
    for (const auto& buttonInfo : buttonInfos) {
        logDebug("Processing button ID: " + String(buttonInfo.button_id) + 
                ", Parent: " + String(buttonInfo.parent_encoder_id) + 
                ", Name: " + buttonInfo.name);
        
        if (buttonInfo.hasParent()) {
            // Trouver le widget index de l'encodeur parent
            uint16_t encoder_id = buttonInfo.parent_encoder_id;
            
            // Chercher l'index du widget qui correspond à cet encodeur
            bool found = false;
            for (size_t i = 0; i < std::min(midiControls.size(), static_cast<size_t>(config_.maxWidgets)); i++) {
                if (midiControls[i].control_id == encoder_id) {
                    button_to_widget_mapping_[buttonInfo.button_id] = i;
                    logDebug("Mapped button " + String(buttonInfo.button_id) + 
                            " → widget " + String(i) + " (encoder " + String(encoder_id) + ")");
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                logDebug("Parent encoder " + String(encoder_id) + " not found for button " + String(buttonInfo.button_id));
            }
        } else {
            // Bouton indépendant - l'ajouter à la liste des standalone
            standalone_buttons_.push_back(buttonInfo);
            logDebug("Added standalone button: " + String(buttonInfo.button_id) + " (" + buttonInfo.name + ")");
        }
    }
    
    logInfo("Button mapping complete - " + String(button_to_widget_mapping_.size()) + 
           " buttons mapped, " + String(standalone_buttons_.size()) + " standalone buttons");
}

void WidgetMappingManager::logInfo(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[WidgetMappingManager] " + message);
    }
}

void WidgetMappingManager::logDebug(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[WidgetMappingManager DEBUG] " + message);
    }
}