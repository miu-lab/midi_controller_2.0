#pragma once

#include <array>
#include <memory>
#include <set>
#include <unordered_map>

#include "config/ConfigDefaults.hpp"
#include "config/GlobalSettings.hpp"
#include "config/unified/ControlDefinition.hpp"
#include "core/domain/types.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/commands/midi/SendMidiCCCommand.hpp"
#include "core/domain/commands/midi/SendMidiNoteCommand.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/strategies/MidiMappingStrategy.hpp"
#include "core/domain/types.hpp"
#include "core/ports/output/MidiOutputPort.hpp"

/**
 * @brief Mapper qui gère la transformation des événements en commandes MIDI
 *
 * Cette classe utilise le Command Pattern et le Strategy Pattern
 * pour transformer les événements d'entrée en commandes MIDI.
 * Elle implémente l'interface EventListener pour recevoir des événements optimisés.
 */
class MidiMapper : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param midiOut Interface de sortie MIDI
     * @param commandManager Gestionnaire de commandes
     */
    MidiMapper(MidiOutputPort& midiOut, CommandManager& commandManager);

    /**
     * @brief Traite les événements reçus du bus d'événements
     * @param event Événement à traiter
     * @return true si l'événement a été traité, false sinon
     */
    bool onEvent(const Event& event) override;

    /**
     * @brief Définit le mapping pour un contrôle à partir d'une définition complète
     * @param controlDef Définition complète du contrôle avec mappings intégrés
     * @param strategy Stratégie de mapping à utiliser
     */
    void setMappingFromControlDefinition(const ControlDefinition& controlDef, std::unique_ptr<IMidiMappingStrategy> strategy);

    /**
     * @brief Supprime le mapping pour un contrôle
     * @param controlId ID du contrôle
     * @return true si le mapping a été supprimé, false s'il n'existait pas
     */
    bool removeMapping(InputId controlId);

    /**
     * @brief Vérifie si un contrôle a un mapping
     * @param controlId ID du contrôle
     * @return true si le contrôle a un mapping, false sinon
     */
    bool hasMapping(InputId controlId) const;

    /**
     * @brief Obtient la configuration MIDI pour un contrôle
     * @param controlId ID du contrôle
     * @return Configuration MIDI du contrôle, ou une valeur par défaut si non trouvé
     */
    ControlDefinition::MidiConfig getMidiConfig(InputId controlId) const;

    /**
     * @brief Définit les contrôles qui sont utilisés pour la navigation
     * @param navControlIds Ensemble des IDs des contrôles de navigation
     */
    void setNavigationControls(const std::set<InputId>& navControlIds);

    /**
     * @brief Traite un changement d'encodeur
     *
     * Cette méthode est responsable de :
     * - Limiter le taux d'envoi des messages MIDI (via shouldProcessEncoder)
     * - Détecter et éliminer les doublons
     * - Suivre les positions des encodeurs
     * - Appliquer les stratégies de mapping MIDI appropriées
     *
     * @param encoderId ID de l'encodeur
     * @param position Position absolue de l'encodeur
     */
    void processEncoderChange(EncoderId encoderId, int32_t position);


    /**
     * @brief Traite un appui sur un bouton
     * @param buttonId ID du bouton
     * @param pressed true si le bouton est pressé, false s'il est relâché
     */
    void processButtonPress(ButtonId buttonId, bool pressed);

    /**
     * @brief Met à jour l'état des commandes
     * Appelé régulièrement pour traiter les commandes temporisées
     */
    void update();

private:
    //=============================================================================
    // Constantes
    //=============================================================================

    // Taille du pool d'objets pour les commandes MIDI
    static constexpr size_t COMMAND_POOL_SIZE = 16;

    // Constantes de timing
    static constexpr unsigned long ENCODER_RATE_LIMIT_MS =
        ConfigDefaults::ENCODER_RATE_LIMIT_MS;                                               // 60Hz
    static constexpr unsigned long DUPLICATE_CHECK_MS = ConfigDefaults::DUPLICATE_CHECK_MS;  // 10ms

    //=============================================================================
    // Types et structures
    //=============================================================================

    // Structure pour stocker les informations de mapping
    struct MappingInfo {
        ControlDefinition::MidiConfig midiConfig;
        std::unique_ptr<IMidiMappingStrategy> strategy;
        uint8_t lastMidiValue;
        int32_t lastEncoderPosition;
        int32_t midiOffset = 0;  // Offset pour le référentiel flottant
    };

    //=============================================================================
    // Méthodes utilitaires
    //=============================================================================

    // Crée une clé composite à partir d'un ID de contrôle et d'un type
    static uint32_t makeCompositeKey(InputId controlId, MappingControlType type);

    // Obtient la prochaine commande CC disponible du pool
    SendMidiCCCommand& getNextCCCommand();

    // Obtient la prochaine commande Note disponible du pool
    SendMidiNoteCommand& getNextNoteCommand();

    // Fonction de log diagnostique conditionnelle
    void logDiagnostic(const char* format, ...) const;

    // Vérifie si un contrôle est dédié à la navigation
    bool isNavigationControl(InputId controlId) const;

    // Vérifie si l'encodeur doit être traité en fonction de la limitation de taux
    bool shouldProcessEncoder(EncoderId encoderId, int32_t position);

    // Applique la sensibilité à un delta d'encodeur
    int32_t applyEncoderSensitivity(int32_t delta, EncoderId encoderId);

    // Calcule la nouvelle valeur MIDI en fonction du mode (relatif/absolu)
    int16_t calculateMidiValue(MappingInfo& info, int32_t delta, int32_t position);

    // Traite les événements de type bouton (encodeur ou bouton standard)
    void processButtonEvent(InputId buttonId, bool pressed, MappingControlType type);

    //=============================================================================
    // Membres
    //=============================================================================

    // Pool d'objets pour les commandes MIDI CC
    std::array<SendMidiCCCommand, COMMAND_POOL_SIZE> midiCCCommandPool_;
    uint8_t nextCCCommandIndex_ = 0;

    // Pool d'objets pour les commandes MIDI Note
    std::array<SendMidiNoteCommand, COMMAND_POOL_SIZE> midiNoteCommandPool_;
    uint8_t nextNoteCommandIndex_ = 0;

    MidiOutputPort& midiOut_;
    CommandManager& commandManager_;
    std::unordered_map<uint32_t, MappingInfo> mappings_;  // Clé: (controlId << 8 | controlType)
    std::unordered_map<InputId, std::unique_ptr<SendMidiNoteCommand>> activeNotes_;
    std::set<InputId> navigationControls_;

    ControlDefinition::MidiConfig defaultConfig_;  // Configuration par défaut retournée si non trouvée
};