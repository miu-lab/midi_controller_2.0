#pragma once

#include <vector>
#include <Arduino.h>
#include "config/unified/UnifiedConfiguration.hpp"
#include "config/unified/ControlDefinition.hpp"

/**
 * @brief Parser pour extraire les informations MIDI depuis la configuration
 * 
 * Cette classe centralise la logique d'extraction des contrôles MIDI
 * et des informations de boutons depuis UnifiedConfiguration.
 * Respecte le principe de responsabilité unique.
 */
class MidiConfigurationParser {
public:
    /**
     * @brief Structure pour les informations de contrôle MIDI extraites
     */
    struct MidiControlInfo {
        uint8_t cc_number;
        uint8_t channel;
        String name;
        InputId control_id;
        
        MidiControlInfo() : cc_number(0), channel(1), control_id(0) {}
        MidiControlInfo(uint8_t cc, uint8_t chan, const String& n, InputId id)
            : cc_number(cc), channel(chan), name(n), control_id(id) {}
    };

    /**
     * @brief Structure pour les informations de boutons extraites
     */
    struct ButtonInfo {
        uint16_t button_id;
        uint16_t parent_encoder_id;  // 0 si bouton indépendant
        String name;
        
        ButtonInfo() : button_id(0), parent_encoder_id(0) {}
        ButtonInfo(uint16_t id, uint16_t parent_id, const String& n)
            : button_id(id), parent_encoder_id(parent_id), name(n) {}
            
        bool hasParent() const { return parent_encoder_id != 0; }
    };

    /**
     * @brief Configuration pour le parser
     */
    struct ParserConfig {
        bool enableLogging;
        uint8_t maxCCNumber;
        
        ParserConfig() : enableLogging(false), maxCCNumber(127) {}
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration du parser
     */
    explicit MidiConfigurationParser(const ParserConfig& config = ParserConfig());

    /**
     * @brief Destructeur par défaut
     */
    ~MidiConfigurationParser() = default;

    /**
     * @brief Extrait les contrôles MIDI depuis la configuration
     * @param configuration Configuration unifiée source
     * @return Vecteur des informations de contrôles MIDI
     */
    std::vector<MidiControlInfo> extractMidiControls(const UnifiedConfiguration& configuration) const;

    /**
     * @brief Extrait les informations de boutons depuis la configuration
     * @param configuration Configuration unifiée source
     * @return Vecteur des informations de boutons
     */
    std::vector<ButtonInfo> extractButtonInfo(const UnifiedConfiguration& configuration) const;

    /**
     * @brief Valide une information de contrôle MIDI
     * @param info Information à valider
     * @return true si l'information est valide
     */
    bool validateMidiControlInfo(const MidiControlInfo& info) const;

    /**
     * @brief Valide une information de bouton
     * @param info Information à valider
     * @return true si l'information est valide
     */
    bool validateButtonInfo(const ButtonInfo& info) const;

private:
    ParserConfig config_;

    /**
     * @brief Extrait les informations MIDI d'un contrôle encodeur
     * @param controlDef Définition du contrôle
     * @return Information MIDI extraite ou nullopt si pas applicable
     */
    std::optional<MidiControlInfo> extractFromEncoder(const ControlDefinition& controlDef) const;

    /**
     * @brief Extrait les informations MIDI d'un contrôle bouton
     * @param controlDef Définition du contrôle
     * @return Information MIDI extraite ou nullopt si pas applicable
     */
    std::optional<MidiControlInfo> extractFromButton(const ControlDefinition& controlDef) const;

    /**
     * @brief Extrait les informations de bouton d'une définition de contrôle
     * @param controlDef Définition du contrôle
     * @return Information de bouton extraite ou nullopt si pas applicable
     */
    std::optional<ButtonInfo> extractButtonFromControl(const ControlDefinition& controlDef) const;

    /**
     * @brief Log une information si le logging est activé
     * @param message Message à logger
     */
    void logInfo(const String& message) const;

    /**
     * @brief Log une erreur si le logging est activé
     * @param message Message d'erreur à logger
     */
    void logError(const String& message) const;
};