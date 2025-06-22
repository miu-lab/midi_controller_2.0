#pragma once

#include <Arduino.h>

#include <memory>
#include <vector>

#include "LvglMenuView.hpp"
#include "LvglModalView.hpp"
#include "LvglParameterView.hpp"
#include "LvglSplashScreenView.hpp"
#include "ViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/unified/UnifiedConfiguration.hpp"

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
     * @brief Constructeur
     * @param lvglBridge Bridge LVGL
     * @param unifiedConfig Configuration unifiée
     */
    explicit DefaultViewManager(std::shared_ptr<Ili9341LvglBridge> lvglBridge,
                                std::shared_ptr<UnifiedConfiguration> unifiedConfig);

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
    // Dépendances
    std::shared_ptr<Ili9341LvglBridge> lvglBridge_;
    std::shared_ptr<UnifiedConfiguration> unifiedConfig_;

    // Les vues principales (100% LVGL)
    std::shared_ptr<LvglSplashScreenView> splashView_;
    std::shared_ptr<LvglParameterView> parameterView_;
    std::shared_ptr<LvglMenuView> menuView_;
    std::shared_ptr<LvglModalView> modalView_;

    // État actuel
    ViewType currentView_ = ViewType::SplashScreen;
    bool initialized_ = false;
    bool needsDisplayUpdate_ = false;  // Flag pour indiquer qu'une mise à jour est nécessaire
    
    // Méthodes privées
    void activateView(ViewType type);
    void deactivateAllViews();
};