#pragma once

#include <Arduino.h>
#include <memory>
#include <vector>

#include "ViewManager.hpp"
#include "View.hpp"
#include "MenuView.hpp"
#include "ModalView.hpp"
#include "SplashScreenView.hpp"
#include "ParameterFocusView.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Implémentation simplifiée du gestionnaire de vues
 * 
 * Cette classe gère seulement 3 vues principales :
 * - SplashScreen : Initialisation
 * - ParameterFocus : Vue par défaut (affichage paramètres MIDI)
 * - Menu : Navigation et configuration
 */
class DefaultViewManager : public ViewManager {
public:
    /**
     * @brief Constructeur avec dépendance sur l'affichage
     * @param display Référence à l'adaptateur d'affichage
     */
    explicit DefaultViewManager(std::shared_ptr<DisplayPort> display);
    
    /**
     * @brief Destructeur
     */
    ~DefaultViewManager() override = default;
    
    // Interface ViewManager
    bool init() override;
    void update() override;
    void render() override;
    
    // Navigation principale
    void showParameterFocus(uint8_t ccNumber, uint8_t channel, uint8_t value, const String& parameterName) override;
    void updateParameterValue(uint8_t value) override;
    void showMenu() override;
    void showHome() override;
    
    // Modal
    void showModal(const String& message) override;
    void hideModal() override;
    
    // Navigation menu
    void navigateMenu(int8_t direction) override;
    void selectMenuItem() override;
    
    /**
     * @brief Vérifie si l'affichage a besoin d'être mis à jour
     * @return true si une mise à jour est nécessaire
     */
    bool needsDisplayUpdate() const override { return needsDisplayUpdate_; }
    
    /**
     * @brief Réinitialise le flag de mise à jour
     */
    void clearDisplayUpdateFlag() override { needsDisplayUpdate_ = false; }

private:
    // Affichage
    std::shared_ptr<DisplayPort> display_;
    
    // Les 4 vues seulement
    std::shared_ptr<SplashScreenView> splashView_;
    std::shared_ptr<ParameterFocusView> parameterView_;  // VUE PAR DÉFAUT
    std::shared_ptr<MenuView> menuView_;
    std::shared_ptr<ModalView> modalView_;
    
    // État actuel
    ViewType currentView_ = ViewType::SplashScreen;
    bool initialized_ = false;
    bool needsDisplayUpdate_ = false;  // Flag pour indiquer qu'une mise à jour est nécessaire
    
    // Méthodes privées
    void activateView(ViewType type);
    void deactivateAllViews();
};