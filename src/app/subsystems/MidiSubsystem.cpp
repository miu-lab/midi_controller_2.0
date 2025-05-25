#include "MidiSubsystem.hpp"

#include <Arduino.h>

#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
// #include "config/MappingConfiguration.hpp"  // SUPPRIMÉ - Migration vers interface unifiée
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

    // === MIGRATION VERS INTERFACE UNIFIÉE ===
    // Charger les mappings MIDI depuis l'interface IConfiguration
    const auto& midiMappings = loadMidiMappingsFromUnifiedSystem();
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

// === MÉTHODES PRIVÉES POUR MIGRATION UNIFIÉE ===

std::vector<InputMapping> MidiSubsystem::loadMidiMappingsFromUnifiedSystem() const {
    std::vector<InputMapping> midiMappings;
    
    if (!configuration_) {
        Serial.println(F("MidiSubsystem: No configuration available"));
        return midiMappings;
    }
    
    // Obtenir toutes les configurations d'entrée depuis le système unifié
    const auto& allInputConfigs = configuration_->getAllInputConfigurations();
    
    Serial.print(F("MidiSubsystem: Processing "));
    Serial.print(allInputConfigs.size());
    Serial.println(F(" input configurations for MIDI mappings"));
    
    // Convertir les InputConfig en InputMapping pour les contrôles avec mappings MIDI
    for (const auto& inputConfig : allInputConfigs) {
        if (!inputConfig.enabled) continue;
        
        // Générer les mappings MIDI en fonction du type et du groupe
        auto mappings = generateMidiMappingsFromInputConfig(inputConfig);
        midiMappings.insert(midiMappings.end(), mappings.begin(), mappings.end());
    }
    
    Serial.print(F("MidiSubsystem: Generated "));
    Serial.print(midiMappings.size());
    Serial.println(F(" MIDI mappings from unified system"));
    
    return midiMappings;
}

std::vector<InputMapping> MidiSubsystem::generateMidiMappingsFromInputConfig(const InputConfig& inputConfig) const {
    std::vector<InputMapping> mappings;
    
    // Ignorer les contrôles qui ne sont pas dans le groupe MIDI
    if (inputConfig.group != "MIDI" && inputConfig.group != "Precision") {
        return mappings;  // Pas de mappings MIDI pour les autres groupes
    }
    
    // Générer les mappings selon le type de contrôle
    if (inputConfig.type == InputType::ENCODER) {
        // Mapping pour l'encodeur (rotation)
        InputMapping encoderMapping;
        encoderMapping.controlId = inputConfig.id;
        encoderMapping.mappingType = MappingControlType::ENCODER;
        encoderMapping.roles = {MappingRole::MIDI};
        
        // Générer le CC MIDI basé sur l'ID (comme dans l'ancien système)
        uint8_t ccNumber = extractCCFromInputId(inputConfig.id);
        encoderMapping.midiMapping = {
            .channel = static_cast<MidiChannel>(configuration_->midiChannel()),
            .control = static_cast<MidiCC>(ccNumber),
            .type = MidiEventType::CONTROL_CHANGE,
            .isRelative = true,
            .isCentered = std::nullopt  // Pas centré pour les encodeurs relatifs
        };
        
        mappings.push_back(encoderMapping);
        
        // Vérifier s'il y a un bouton d'encodeur (ID + 100 pattern de l'ancien système)
        if (hasEncoderButton(inputConfig)) {
            InputMapping buttonMapping;
            buttonMapping.controlId = getEncoderButtonId(inputConfig.id);
            buttonMapping.mappingType = MappingControlType::BUTTON;
            buttonMapping.roles = {MappingRole::MIDI};
            
            // Générer la Note MIDI (CC + 35 comme dans l'ancien système)
            uint8_t noteNumber = ccNumber + 35;
            buttonMapping.midiMapping = {
                .channel = static_cast<MidiChannel>(configuration_->midiChannel()),
                .control = static_cast<MidiCC>(noteNumber),  // Note stockée dans control pour simplicité
                .type = MidiEventType::NOTE_ON,
                .isRelative = false,
                .isCentered = std::nullopt
            };
            
            mappings.push_back(buttonMapping);
        }
    } else if (inputConfig.type == InputType::BUTTON) {
        // Mapping pour bouton standalone (seulement si dans groupe MIDI)
        if (inputConfig.group == "MIDI") {
            InputMapping buttonMapping;
            buttonMapping.controlId = inputConfig.id;
            buttonMapping.mappingType = MappingControlType::BUTTON;
            buttonMapping.roles = {MappingRole::MIDI};
            
            // Générer une note MIDI basée sur l'ID
            uint8_t noteNumber = static_cast<uint8_t>(inputConfig.id);  // Simple mapping direct
            buttonMapping.midiMapping = {
                .channel = static_cast<MidiChannel>(configuration_->midiChannel()),
                .control = static_cast<MidiCC>(noteNumber),
                .type = MidiEventType::NOTE_ON,
                .isRelative = false,
                .isCentered = std::nullopt
            };
            
            mappings.push_back(buttonMapping);
        }
    }
    
    return mappings;
}

// === MÉTHODES UTILITAIRES PRIVÉES ===

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

bool MidiSubsystem::hasEncoderButton(const InputConfig& inputConfig) const {
    // Vérifier si l'encodeur a un bouton (basé sur la structure EncoderConfig)
    if (inputConfig.type == InputType::ENCODER) {
        // Assumons que si c'est un encodeur MIDI, il a un bouton
        // (logique simplifiée - dans un vrai système, on vérifierait les pins)
        return inputConfig.group == "MIDI";
    }
    return false;
}

InputId MidiSubsystem::getEncoderButtonId(InputId encoderId) const {
    // Pattern de l'ancien système : ID encodeur + 100
    return encoderId + 100;
}