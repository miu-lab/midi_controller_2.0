#pragma once
#include "domain/IMidiOut.hpp"
#include "storage/ProfileManager.hpp"
#include "app/NavigationConfigService.hpp"

class InputRouter {
public:
    InputRouter(IMidiOut& midiOut, IProfileManager& profileManager);
    
    /**
     * @brief Initialise le routeur sans gestion de la navigation
     */
    void init();
    
    /**
     * @brief Initialise le routeur avec gestion de la navigation
     * @param navService Service de configuration de la navigation
     */
    void init(NavigationConfigService& navService);
    
private:
    IMidiOut& midiOut_;
    IProfileManager& profileManager_;
    NavigationConfigService* navigationService_ = nullptr;
    
    // Implémentation interne de l'initialisation
    void subscribeToEvents();
};

