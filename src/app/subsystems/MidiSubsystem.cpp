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
    loadMidiMappingsFromControlDefinitions();

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

void MidiSubsystem::loadMidiMappingsFromControlDefinitions() const {
    if (!configuration_) {
        Serial.println(F("MidiSubsystem: No configuration available"));
        return;
    }
    
    // Obtenir toutes les définitions de contrôles depuis le système unifié
    const auto& allControlDefinitions = configuration_->getAllControlDefinitions();
    
    Serial.print(F("MidiSubsystem: Processing "));
    Serial.print(allControlDefinitions.size());
    Serial.println(F(" control definitions for MIDI mappings"));
    
    int mappingCount = 0;
    
    // Utiliser directement les mappings intégrés dans ControlDefinition
    for (const auto& controlDef : allControlDefinitions) {
        if (!controlDef.enabled) continue;
        
        // Configurer les mappings MIDI depuis cette définition de contrôle
        setupMidiMappingFromControlDefinition(controlDef);
        mappingCount++;
    }
    
    Serial.print(F("MidiSubsystem: Configured "));
    Serial.print(mappingCount);
    Serial.println(F(" control definitions with MIDI mappings"));
}

void MidiSubsystem::setupMidiMappingFromControlDefinition(const ControlDefinition& controlDef) const {
    // Vérifier s'il y a des mappings MIDI dans cette définition
    bool hasMidiMappings = false;
    for (const auto& mappingSpec : controlDef.mappings) {
        if (mappingSpec.role == MappingRole::MIDI) {
            hasMidiMappings = true;
            break;
        }
    }
    
    if (!hasMidiMappings) {
        return; // Pas de mappings MIDI pour ce contrôle
    }
    
    // Déterminer le type de stratégie à utiliser en fonction du premier mapping MIDI
    std::unique_ptr<IMidiMappingStrategy> strategy;
    
    for (const auto& mappingSpec : controlDef.mappings) {
        if (mappingSpec.role == MappingRole::MIDI) {
            const auto& midiConfig = std::get<ControlDefinition::MidiConfig>(mappingSpec.config);
            
            if (midiConfig.isRelative) {
                strategy = MidiMappingFactory::createRelative();
            } else {
                strategy = MidiMappingFactory::createAbsolute(0, 127);
            }
            break; // Utiliser le premier mapping trouvé pour déterminer la stratégie
        }
    }
    
    if (strategy) {
        // Configurer le mapping dans MidiMapper en utilisant la nouvelle méthode
        midiMapper_->setMappingFromControlDefinition(controlDef, std::move(strategy));
        
        Serial.print(F("MidiSubsystem: Configured MIDI mapping for control ID "));
        Serial.println(controlDef.id);
    }
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
