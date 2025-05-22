/**
 * @file ParameterFocusViewExample.cpp
 * @brief Exemple d'utilisation de la ParameterFocusView
 * 
 * Ce fichier montre comment utiliser la nouvelle ParameterFocusView
 * pour afficher l'interface de paramètre comme dans l'image de référence.
 */

#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "core/controllers/UIController.hpp"

/**
 * @brief Exemple d'utilisation de la ParameterFocusView
 * 
 * Cette fonction montre comment afficher la vue de focus paramètre
 * avec les informations MIDI appropriées.
 */
void demonstrateParameterFocusView(DefaultViewManager& viewManager) {
    
    // Exemple 1: Afficher un paramètre Cutoff
    // CC 127, Canal 16, Valeur 64 (mi-course), Nom "CUTOFF"
    viewManager.showParameterFocusView(127, 16, 64, "CUTOFF");
    
    // Attendre un peu pour voir l'affichage
    delay(2000);
    
    // Exemple 2: Simuler un changement de valeur (comme si l'utilisateur tournait un encodeur)
    for (uint8_t value = 64; value <= 127; value += 4) {
        viewManager.updateParameterFocusValue(value);
        delay(100); // Animation fluide
    }
    
    delay(1000);
    
    // Exemple 3: Afficher un autre paramètre - Resonance
    viewManager.showParameterFocusView(71, 1, 32, "RESONANCE");
    
    delay(2000);
    
    // Exemple 4: Filter Frequency
    viewManager.showParameterFocusView(74, 1, 96, "FREQ");
    
    delay(2000);
    
    // Exemple 5: Volume
    viewManager.showParameterFocusView(7, 1, 100, "VOLUME");
}

/**
 * @brief Intégrer la ParameterFocusView dans le workflow normal
 * 
 * Cette fonction montre comment intégrer la vue dans le flux
 * normal de l'application MIDI.
 */
void integrateParameterFocusInWorkflow(UIController& uiController, 
                                      DefaultViewManager& viewManager) {
    
    // Scenario: L'utilisateur tourne un encodeur assigné à un CC
    // Le système doit automatiquement afficher la ParameterFocusView
    
    // Simuler l'envoi d'un message MIDI CC
    uint8_t ccNumber = 127;
    uint8_t channel = 16;
    uint8_t value = 64;
    String paramName = "CUTOFF";
    
    // Afficher immédiatement la vue de focus
    viewManager.showParameterFocusView(ccNumber, channel, value, paramName);
    
    // Simuler des changements de valeur en temps réel
    for (int i = 0; i < 20; i++) {
        // Simuler la rotation d'un encodeur
        value = constrain(value + random(-10, 10), 0, 127);
        viewManager.updateParameterFocusValue(value);
        delay(150); // Feedback temps réel
    }
    
    // Après un timeout, revenir à la vue précédente
    delay(3000);
    viewManager.showLastControlView(); // ou autre vue par défaut
}

/**
 * @brief Exemple de mapping de paramètres typiques
 * 
 * Cette fonction montre les paramètres MIDI courants qui 
 * bénéficieraient de la ParameterFocusView.
 */
void demonstrateCommonParameters(DefaultViewManager& viewManager) {
    
    // Paramètres de synthé courants
    struct MidiParameter {
        uint8_t cc;
        uint8_t channel;
        String name;
        uint8_t defaultValue;
    };
    
    MidiParameter params[] = {
        {1,   1, "MOD WHEEL", 0},
        {7,   1, "VOLUME", 100},
        {10,  1, "PAN", 64},
        {11,  1, "EXPRESSION", 127},
        {71,  1, "RESONANCE", 32},
        {72,  1, "RELEASE", 64},
        {73,  1, "ATTACK", 48},
        {74,  1, "FREQUENCY", 80},
        {75,  1, "DECAY", 60},
        {91,  1, "REVERB", 40},
        {93,  1, "CHORUS", 20},
        {127, 16, "CUTOFF", 90}
    };
    
    // Démonstration de chaque paramètre
    for (const auto& param : params) {
        viewManager.showParameterFocusView(
            param.cc, 
            param.channel, 
            param.defaultValue, 
            param.name
        );
        
        // Animation de la valeur pour montrer le cercle en action
        for (uint8_t v = 0; v <= 127; v += 8) {
            viewManager.updateParameterFocusValue(v);
            delay(50);
        }
        
        delay(1000);
    }
}

/**
 * @brief Comment intégrer avec le système d'événements
 * 
 * Pseudo-code montrant comment connecter la vue aux événements MIDI
 */
/*
void connectToMidiEvents(EventBus& eventBus, DefaultViewManager& viewManager) {
    
    // Écouter les événements de changement de CC
    eventBus.subscribe<MidiCCChangedEvent>([&viewManager](const MidiCCChangedEvent& event) {
        
        // Mapper le CC à un nom de paramètre lisible
        String paramName = mapCCToParameterName(event.ccNumber);
        
        // Afficher la vue de focus avec les nouvelles informations
        viewManager.showParameterFocusView(
            event.ccNumber,
            event.channel,
            event.value,
            paramName
        );
        
        // Auto-timeout pour revenir à la vue précédente après inactivité
        scheduleViewTimeout(3000); // 3 secondes
    });
    
    // Écouter les changements de valeur rapides (encodeurs)
    eventBus.subscribe<EncoderChangedEvent>([&viewManager](const EncoderChangedEvent& event) {
        // Si la ParameterFocusView est active, mettre à jour seulement la valeur
        if (viewManager.isParameterFocusViewActive()) {
            viewManager.updateParameterFocusValue(event.newValue);
        }
    });
}

String mapCCToParameterName(uint8_t ccNumber) {
    switch (ccNumber) {
        case 1: return "MOD WHEEL";
        case 7: return "VOLUME";
        case 10: return "PAN";
        case 71: return "RESONANCE";
        case 74: return "FREQUENCY";
        case 127: return "CUTOFF";
        default: return "CC " + String(ccNumber);
    }
}
*/