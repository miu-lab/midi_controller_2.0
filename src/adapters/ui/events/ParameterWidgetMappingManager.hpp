#pragma once

#include <array>
#include <cstdint>
#include <map>

#include "ConfigurationMidiExtractor.hpp"

/**
 * @brief Gestionnaire de mappings entre contrôles MIDI et widgets de paramètres
 * 
 * Cette classe centralise la logique de mapping CC MIDI → index de widget
 * et button ID → index de widget pour simplifier LvglParameterView.
 * Respecte le principe de responsabilité unique.
 */
class ParameterWidgetMappingManager {
public:
    /**
     * @brief Configuration pour le gestionnaire de mappings
     */
    struct MappingConfig {
        uint8_t maxWidgets;           // Nombre maximum de widgets (généralement 8)
        bool enableLogging;           // Activer le logging de debug
        bool enableButtonMapping;     // Activer le mapping des boutons
        
        MappingConfig() : maxWidgets(8), enableLogging(false), enableButtonMapping(true) {}
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration du gestionnaire
     */
    explicit ParameterWidgetMappingManager(const MappingConfig& config = MappingConfig());

    /**
     * @brief Destructeur par défaut
     */
    ~ParameterWidgetMappingManager() = default;

    /**
     * @brief Initialise les mappings depuis la configuration MIDI
     * @param midiControls Liste des contrôles MIDI extraits par ConfigurationMidiExtractor
     * @param buttonInfos Liste des boutons extraits par ConfigurationMidiExtractor
     */
    void initializeMappings(
        const std::vector<ConfigurationMidiExtractor::MidiControlInfo>& midiControls,
        const std::vector<ConfigurationMidiExtractor::ButtonInfo>& buttonInfos);

    /**
     * @brief Obtient l'index du widget pour un CC donné
     * @param cc_number Numéro CC MIDI (0-127)
     * @return Index du widget (0-maxWidgets-1) ou -1 si non mappé
     */
    int8_t getWidgetIndexForCC(uint8_t cc_number) const;

    /**
     * @brief Obtient l'index du widget pour un bouton donné
     * @param button_id ID du bouton
     * @return Index du widget parent (0-maxWidgets-1) ou -1 si non mappé
     */
    int8_t getWidgetIndexForButton(uint16_t button_id) const;

    /**
     * @brief Obtient la liste des boutons mappés vers les widgets
     * @return Map button_id → widget_index
     */
    const std::map<uint16_t, uint8_t>& getButtonToWidgetMapping() const;

    /**
     * @brief Obtient la liste des boutons indépendants (sans parent encodeur)
     * @return Vector des boutons standalone
     */
    const std::vector<ConfigurationMidiExtractor::ButtonInfo>& getStandaloneButtons() const;

    /**
     * @brief Vérifie si les mappings sont initialisés
     * @return true si initialisés
     */
    bool isInitialized() const;

    /**
     * @brief Remet à zéro tous les mappings
     */
    void reset();

    /**
     * @brief Obtient les statistiques de mapping
     * @return Nombre de CC mappés, boutons mappés, boutons standalone
     */
    struct MappingStats {
        uint8_t ccMapped;
        uint8_t buttonsMapped;
        uint8_t buttonsStandalone;
    };
    MappingStats getStats() const;

private:
    MappingConfig config_;
    bool initialized_;

    // Mapping CC → widget index (255 = non mappé)
    std::array<uint8_t, 128> cc_to_widget_mapping_;
    
    // Mapping button_id → widget_index
    std::map<uint16_t, uint8_t> button_to_widget_mapping_;
    
    // Boutons indépendants (sans parent encodeur)
    std::vector<ConfigurationMidiExtractor::ButtonInfo> standalone_buttons_;

    /**
     * @brief Initialise le mapping CC → widget
     * @param midiControls Liste des contrôles MIDI
     */
    void initializeCCMapping(
        const std::vector<ConfigurationMidiExtractor::MidiControlInfo>& midiControls);

    /**
     * @brief Initialise le mapping boutons → widgets
     * @param midiControls Liste des contrôles MIDI (pour trouver les parents)
     * @param buttonInfos Liste des boutons
     */
    void initializeButtonMapping(
        const std::vector<ConfigurationMidiExtractor::MidiControlInfo>& midiControls,
        const std::vector<ConfigurationMidiExtractor::ButtonInfo>& buttonInfos);

    /**
     * @brief Log une information si le logging est activé
     * @param message Message à logger
     */
    void logInfo(const String& message) const;

    /**
     * @brief Log un debug si le logging est activé
     * @param message Message de debug à logger
     */
    void logDebug(const String& message) const;
};