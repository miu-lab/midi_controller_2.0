#include "adapters/secondary/storage/ProfileManager.hpp"
#include "config/unified/ControlBuilder.hpp"

std::optional<ControlDefinition> ProfileManager::getControlDefinition(InputId id) const {
    auto it = controlDefinitions_.find(id);
    if (it != controlDefinitions_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ProfileManager::setControlDefinition(const ControlDefinition& controlDef) {
    controlDefinitions_[controlDef.id] = controlDef;
}

std::vector<ControlDefinition> ProfileManager::getAllControlDefinitions() const {
    std::vector<ControlDefinition> result;
    result.reserve(controlDefinitions_.size());

    for (const auto& pair : controlDefinitions_) {
        result.push_back(pair.second);
    }

    return result;
}

bool ProfileManager::saveProfile() {
    // Implémentation simple - à développer avec un vrai stockage persistant
    return true;
}

bool ProfileManager::loadProfile() {
    // Implémentation simple - à développer avec un vrai stockage persistant
    return true;
}

void ProfileManager::resetToDefaults() {
    controlDefinitions_.clear();

    // Recréer les contrôles par défaut avec le ControlBuilder
    // Encodeur 1 avec bouton
    auto encoder1 = ControlBuilder(71, "encoder_1")
        .withLabel("Volume")
        .asEncoder(2, 3, 24)  // pins A=2, B=3, 24 PPR
        .withEncoderButton(4) // pin bouton = 4
        .withMidiCC(1, 0, true)  // CC 1, canal 0, relatif
        .withMidiNote(36, 0)     // Note 36 pour le bouton
        .inGroup("MIDI Controls")
        .build();
    
    setControlDefinition(encoder1);

    // Encodeur 2 avec bouton  
    auto encoder2 = ControlBuilder(72, "encoder_2")
        .withLabel("Pan")
        .asEncoder(5, 6, 24)
        .withEncoderButton(7)
        .withMidiCC(2, 0, true)
        .withMidiNote(37, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder2);

    // Encodeur 3 avec bouton
    auto encoder3 = ControlBuilder(73, "encoder_3")
        .withLabel("Filter")
        .asEncoder(8, 9, 24)
        .withEncoderButton(10)
        .withMidiCC(3, 0, true)
        .withMidiNote(38, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder3);

    // Encodeur 4 avec bouton
    auto encoder4 = ControlBuilder(74, "encoder_4")
        .withLabel("Resonance")  
        .asEncoder(11, 12, 24)
        .withEncoderButton(13)
        .withMidiCC(4, 0, true)
        .withMidiNote(39, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder4);

    // Encodeur 5 avec bouton
    auto encoder5 = ControlBuilder(75, "encoder_5")
        .withLabel("Attack")
        .asEncoder(14, 15, 24)
        .withEncoderButton(16)
        .withMidiCC(5, 0, true)
        .withMidiNote(40, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder5);

    // Encodeur 6 avec bouton
    auto encoder6 = ControlBuilder(76, "encoder_6")
        .withLabel("Decay")
        .asEncoder(17, 18, 24)
        .withEncoderButton(19)
        .withMidiCC(6, 0, true)
        .withMidiNote(41, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder6);

    // Encodeur 7 avec bouton
    auto encoder7 = ControlBuilder(77, "encoder_7")
        .withLabel("Sustain")
        .asEncoder(20, 21, 24)
        .withEncoderButton(22)
        .withMidiCC(7, 0, true)
        .withMidiNote(42, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder7);

    // Encodeur 8 avec bouton
    auto encoder8 = ControlBuilder(78, "encoder_8")
        .withLabel("Release")
        .asEncoder(23, 24, 24)
        .withEncoderButton(25)
        .withMidiCC(8, 0, true)
        .withMidiNote(43, 0)
        .inGroup("MIDI Controls")
        .build();
        
    setControlDefinition(encoder8);

    // Boutons de navigation
    auto menuButton = ControlBuilder(51, "menu_button")
        .withLabel("Menu")
        .asButton(26, ButtonMode::MOMENTARY)
        .withNavigation("MENU_TOGGLE", MappingControlType::BUTTON)
        .inGroup("Navigation")
        .build();
        
    setControlDefinition(menuButton);

    auto okButton = ControlBuilder(52, "ok_button")
        .withLabel("OK")
        .asButton(27, ButtonMode::MOMENTARY)
        .withNavigation("OK_SELECT", MappingControlType::BUTTON)
        .inGroup("Navigation")
        .build();
        
    setControlDefinition(okButton);

    // Encodeur de navigation
    auto navEncoder = ControlBuilder(79, "nav_encoder")
        .withLabel("Navigation")
        .asEncoder(28, 29, 24)
        .withNavigation("ITEM_NAVIGATOR", MappingControlType::ENCODER)
        .inGroup("Navigation")
        .build();
        
    setControlDefinition(navEncoder);
}

bool ProfileManager::removeBinding(InputId id) {
    auto it = controlDefinitions_.find(id);
    if (it != controlDefinitions_.end()) {
        controlDefinitions_.erase(it);
        return true;
    }
    return false;
}
