#include "MidiSubsystem.hpp"

#include <Arduino.h>

#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/strategies/AbsoluteMappingStrategy.hpp"
#include "core/domain/strategies/MidiMappingFactory.hpp"
#include "core/domain/strategies/RelativeMappingStrategy.hpp"

MidiSubsystem::MidiSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {}

Result<bool, std::string> MidiSubsystem::init() {
    if (initialized_) {
        return Result<bool, std::string>::success(true);
    }

    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        return Result<bool, std::string>::error("Failed to resolve IConfiguration");
    }

    // Récupérer ou créer le gestionnaire de commandes
    commandManager_ = container_->resolve<CommandManager>();
    if (!commandManager_) {
        commandManager_ = std::make_shared<CommandManager>();
        container_->registerDependency<CommandManager>(commandManager_);
    } else {
        Serial.println(F("MidiSubsystem: Using existing CommandManager"));
    }

    // Créer l'interface MIDI de base (TeensyUsbMidiOut)
    std::shared_ptr<TeensyUsbMidiOut> baseMidiOut;
    if (configuration_->isHardwareInitEnabled()) {
        baseMidiOut = std::make_shared<TeensyUsbMidiOut>();
        if (!baseMidiOut) {
            return Result<bool, std::string>::error("Failed to create TeensyUsbMidiOut");
        }
    } else {
        Serial.println(F("MidiSubsystem: Creating TeensyUsbMidiOut (test mode)"));
        baseMidiOut = std::make_shared<TeensyUsbMidiOut>();
        if (!baseMidiOut) {
            return Result<bool, std::string>::error("Failed to create TeensyUsbMidiOut");
        }
    }

    // Créer l'EventEnabledMidiOut qui va décorer directement TeensyUsbMidiOut
    auto eventEnabledMidiOut = std::make_shared<EventEnabledMidiOut>(*baseMidiOut);
    if (!eventEnabledMidiOut) {
        return Result<bool, std::string>::error("Failed to create EventEnabledMidiOut");
    }
    Serial.println(F("MidiSubsystem: Created EventEnabledMidiOut wrapper"));

    // Utiliser EventEnabledMidiOut comme interface MidiOutputPort
    midiOut_ = eventEnabledMidiOut;

    // Enregistrer l'implémentation que nous venons de créer
    container_->registerImplementation<MidiOutputPort, MidiOutputPort>(midiOut_);

    // Enregistrer également les objets intermédiaires pour éviter qu'ils soient détruits
    container_->registerDependency<TeensyUsbMidiOut>(baseMidiOut);
    container_->registerDependency<EventEnabledMidiOut>(eventEnabledMidiOut);

    // Créer le MidiMapper et MidiInHandler
    midiMapper_ = std::make_unique<MidiMapper>(*midiOut_, *commandManager_);
    if (!midiMapper_) {
        return Result<bool, std::string>::error("Failed to create MidiMapper");
    }

    midiInHandler_ = std::make_unique<MidiInHandler>();
    if (!midiInHandler_) {
        return Result<bool, std::string>::error("Failed to create MidiInHandler");
    }
    Serial.println(F("MidiSubsystem: MidiInHandler created"));

    // Charger les mappings MIDI depuis les ControlDefinition
    const auto& midiMappings = loadMidiMappingsFromControlDefinitions();
    if (midiMappings.empty()) {
        Serial.println(F("MidiSubsystem: Warning - No MIDI mappings found"));
    }

    int mappingCount = 0;
    for (const auto& mapping : midiMappings) {
        // Créer une stratégie de mapping appropriée
        std::unique_ptr<IMidiMappingStrategy> strategy;
        if (mapping.midiMapping.isRelative) {
            strategy = MidiMappingFactory::createRelative();
        } else {
            strategy = MidiMappingFactory::createAbsolute(0, 127);
        }

        // Ajouter le mapping au MidiMapper
        midiMapper_->setMapping(mapping, std::move(strategy));

        mappingCount++;
    }

    // Enregistrer ce sous-système comme implémentation de IMidiSystem
    container_->registerImplementation<IMidiSystem, MidiSubsystem>(
        std::shared_ptr<MidiSubsystem>(this, [](MidiSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    initialized_ = true;
    return Result<bool, std::string>::success(true);
}

void MidiSubsystem::update() {
    // Traiter les messages MIDI entrants
    if (midiInHandler_) {
        midiInHandler_->update();
    }

    // Mettre à jour le MidiMapper (pour les commandes temporisées)
    if (midiMapper_) {
        midiMapper_->update();
    }
}

Result<bool, std::string> MidiSubsystem::sendNoteOn(uint8_t channel, uint8_t note,
                                                    uint8_t velocity) {
    if (!initialized_) {
        return Result<bool, std::string>::error("MidiSubsystem: Not initialized");
    }

    if (!midiOut_) {
        return Result<bool, std::string>::error("MidiSubsystem: No MIDI output available");
    }

    // Conversion des types pour correspondre à la signature attendue
    // Pas d'utilisation d'exceptions en environnement embarqué
    midiOut_->sendNoteOn(MidiChannel(channel), MidiNote(note), velocity);
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> MidiSubsystem::sendNoteOff(uint8_t channel, uint8_t note) {
    if (!initialized_) {
        return Result<bool, std::string>::error("MidiSubsystem: Not initialized");
    }

    if (!midiOut_) {
        return Result<bool, std::string>::error("MidiSubsystem: No MIDI output available");
    }

    // sendNoteOff attend un troisième paramètre (velocity)
    midiOut_->sendNoteOff(MidiChannel(channel), MidiNote(note), 0);
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> MidiSubsystem::sendControlChange(uint8_t channel, uint8_t controller,
                                                           uint8_t value) {
    if (!initialized_) {
        return Result<bool, std::string>::error("MidiSubsystem: Not initialized");
    }

    if (!midiOut_) {
        return Result<bool, std::string>::error("MidiSubsystem: No MIDI output available");
    }

    // La méthode s'appelle sendCc et non sendControlChange
    midiOut_->sendControlChange(MidiChannel(channel), MidiCC(controller), value);
    return Result<bool, std::string>::success(true);
}

MidiMapper& MidiSubsystem::getMidiMapper() const {
    if (!midiMapper_) {
        static MidiMapper nullMapper(*midiOut_,
                                     *commandManager_);  // Objet de remplacement sécurisé
        return nullMapper;
    }
    return *midiMapper_;
}

std::vector<InputMapping> MidiSubsystem::loadMidiMappingsFromControlDefinitions() const {
    std::vector<InputMapping> midiMappings;
    
    if (!configuration_) {
        Serial.println(F("MidiSubsystem: No configuration available"));
        return midiMappings;
    }
    
    // Obtenir toutes les définitions de contrôles depuis le système unifié
    const auto& allControlDefinitions = configuration_->getAllControlDefinitions();
    
    Serial.print(F("MidiSubsystem: Processing "));
    Serial.print(allControlDefinitions.size());
    Serial.println(F(" control definitions for MIDI mappings"));
    
    // Utiliser directement les mappings intégrés dans ControlDefinition
    for (const auto& controlDef : allControlDefinitions) {
        if (!controlDef.enabled) continue;
        
        // Générer les mappings MIDI depuis les mappings intégrés
        auto mappings = generateMidiMappingsFromControlDefinition(controlDef);
        midiMappings.insert(midiMappings.end(), mappings.begin(), mappings.end());
    }
    
    Serial.print(F("MidiSubsystem: Generated "));
    Serial.print(midiMappings.size());
    Serial.println(F(" MIDI mappings from control definitions"));
    
    return midiMappings;
}

std::vector<InputMapping> MidiSubsystem::generateMidiMappingsFromControlDefinition(const ControlDefinition& controlDef) const {
    std::vector<InputMapping> mappings;
    
    // Utiliser directement les mappings intégrés dans ControlDefinition
    for (const auto& mappingSpec : controlDef.mappings) {
        // Ne traiter que les mappings MIDI
        if (mappingSpec.role != MappingRole::MIDI) {
            continue;
        }
        
        // Vérifier que c'est bien un mapping MIDI
        if (auto midiConfig = std::get_if<ControlDefinition::MidiConfig>(&mappingSpec.config)) {
            InputMapping inputMapping;
            inputMapping.controlId = controlDef.id;
            inputMapping.mappingType = mappingSpec.appliesTo;
            inputMapping.roles = {MappingRole::MIDI};
            
            // Créer le mapping MIDI depuis la configuration intégrée
            inputMapping.midiMapping = {
                .channel = static_cast<MidiChannel>(midiConfig->channel),
                .control = static_cast<MidiCC>(midiConfig->control),
                .type = (mappingSpec.appliesTo == MappingControlType::ENCODER) ? 
                        MidiEventType::CONTROL_CHANGE : MidiEventType::NOTE_ON,
                .isRelative = midiConfig->isRelative,
                .isCentered = std::nullopt
            };
            
            mappings.push_back(inputMapping);
        }
    }
    
    // Ajouter les mappings pour le bouton d'encodeur si présent
    if (controlDef.hardware.type == InputType::ENCODER && controlDef.hardware.encoderButtonPin) {
        InputId buttonId = controlDef.getEncoderButtonId();
        
        // Chercher les mappings qui s'appliquent aux boutons
        for (const auto& mappingSpec : controlDef.mappings) {
            if (mappingSpec.role == MappingRole::MIDI && 
                mappingSpec.appliesTo == MappingControlType::BUTTON) {
                
                if (auto midiConfig = std::get_if<ControlDefinition::MidiConfig>(&mappingSpec.config)) {
                    InputMapping buttonMapping;
                    buttonMapping.controlId = buttonId;
                    buttonMapping.mappingType = MappingControlType::BUTTON;
                    buttonMapping.roles = {MappingRole::MIDI};
                    
                    buttonMapping.midiMapping = {
                        .channel = static_cast<MidiChannel>(midiConfig->channel),
                        .control = static_cast<MidiCC>(midiConfig->control),
                        .type = MidiEventType::NOTE_ON,
                        .isRelative = false,
                        .isCentered = std::nullopt
                    };
                    
                    mappings.push_back(buttonMapping);
                }
            }
        }
    }
    
    return mappings;
}

uint8_t MidiSubsystem::extractCCFromInputId(InputId inputId) const {
    // Extraire le numéro CC depuis l'ID (logique de l'ancien système)
    // IDs 71-78 -> CCs 1-8, ID 80 -> CC 10
    if (inputId >= 71 && inputId <= 78) {
        return static_cast<uint8_t>(inputId - 70);  // 71->1, 72->2, etc.
    } else if (inputId == 80) {
        return 10;  // Encodeur optique
    }
    // Fallback pour autres IDs
    return static_cast<uint8_t>(inputId % 128);
}

bool MidiSubsystem::hasEncoderButton(const ControlDefinition& controlDef) const {
    // Vérifier si l'encodeur a un bouton (directement depuis ControlDefinition)
    if (controlDef.hardware.type == InputType::ENCODER) {
        return controlDef.hardware.encoderButtonPin.has_value();
    }
    return false;
}

InputId MidiSubsystem::getEncoderButtonId(InputId encoderId) const {
    // Pattern de l'ancien système : ID encodeur + 100
    return encoderId + 100;
}