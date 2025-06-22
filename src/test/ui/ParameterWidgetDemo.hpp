#pragma once

#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include <memory>

/**
 * @brief Démo pour tester visuellement le ParameterWidget avec données MIDI réelles
 */
class ParameterWidgetDemo {
public:
    /**
     * @brief Constructeur
     * @param bridge Le bridge LVGL vers le hardware
     */
    explicit ParameterWidgetDemo(std::shared_ptr<Ili9341LvglBridge> bridge);
    
    /**
     * @brief Destructeur
     */
    ~ParameterWidgetDemo();
    
    /**
     * @brief Initialise la démo
     * @return true si succès
     */
    bool init();
    
    /**
     * @brief Met à jour la démo (à appeler dans la boucle principale)
     */
    void update();
    
    /**
     * @brief Simule un changement de paramètre MIDI
     * @param cc_number Numéro CC MIDI
     * @param channel Canal MIDI
     * @param value Valeur MIDI (0-127)
     * @param parameter_name Nom du paramètre
     */
    void simulateMidiParameter(uint8_t cc_number, uint8_t channel, uint8_t value, const String& parameter_name);
    
    /**
     * @brief Met à jour seulement la valeur du paramètre
     * @param value Nouvelle valeur (0-127)
     */
    void updateParameterValue(uint8_t value);
    
    /**
     * @brief Démarre un cycle de test automatique
     */
    void startAutoDemo();
    
    /**
     * @brief Arrête le cycle de test automatique
     */
    void stopAutoDemo();

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    std::unique_ptr<ParameterWidget> widget_;
    
    // État de la démo
    bool auto_demo_active_;
    unsigned long last_update_time_;
    uint8_t demo_value_;
    int8_t demo_direction_;
    
    // Configuration démo
    static constexpr uint32_t AUTO_DEMO_INTERVAL_MS = 50;  // 20 FPS pour animation fluide
    static constexpr uint8_t DEMO_CC_NUMBER = 74;          // CC Filter Cutoff (standard)
    static constexpr uint8_t DEMO_CHANNEL = 1;
    static constexpr const char* DEMO_PARAM_NAME = "Filter Cutoff";
    
    void setupDemoParameter();
};