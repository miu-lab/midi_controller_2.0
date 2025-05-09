#pragma once

#include "config/common/CommonIncludes.hpp"

/**
 * @brief Identifiants des catégories d'événements
 */
enum class EventCategory : uint8_t {
    None = 0,
    Input = 1,   // Événements d'entrée (encodeurs, boutons)
    UI = 2,      // Événements d'interface utilisateur
    MIDI = 3,    // Événements MIDI
    System = 4   // Événements système
};

/**
 * @brief Type de base pour les identifiants d'événements
 */
using EventType = uint16_t;

/**
 * @brief Classe de base pour tous les événements
 */
class Event {
public:
    /**
     * @brief Constructeur
     * @param type Type d'événement
     * @param category Catégorie d'événement
     */
    Event(EventType type, EventCategory category)
        : type_(type), category_(static_cast<uint8_t>(category)), handled_(false), propagate_(true) {}
    
    /**
     * @brief Destructeur virtuel
     */
    virtual ~Event() = default;
    
    /**
     * @brief Obtient le type d'événement
     * @return Type d'événement
     */
    EventType getType() const { return type_; }
    
    /**
     * @brief Obtient la catégorie d'événement
     * @return Catégorie d'événement
     */
    EventCategory getCategory() const { return static_cast<EventCategory>(category_); }
    
    /**
     * @brief Vérifie si l'événement a été traité
     * @return true si l'événement a été traité, false sinon
     */
    bool isHandled() const { return handled_; }
    
    /**
     * @brief Marque l'événement comme traité
     */
    void setHandled() { handled_ = true; }
    
    /**
     * @brief Vérifie si l'événement doit continuer à être propagé
     * @return true si l'événement doit être propagé, false sinon
     */
    bool shouldPropagate() const { return propagate_; }
    
    /**
     * @brief Arrête la propagation de l'événement aux autres écouteurs
     */
    void stopPropagation() { propagate_ = false; }
    
    /**
     * @brief Obtient le nom de l'événement (pour le débogage)
     * @return Nom de l'événement
     */
    virtual const char* getEventName() const { return "Event"; }

protected:
    EventType type_;   // Type d'événement
    uint8_t category_; // Catégorie d'événement (stockée comme uint8_t pour économiser de la mémoire)
    bool handled_;     // Indique si l'événement a été traité
    bool propagate_;   // Indique si l'événement doit continuer à être propagé
};
