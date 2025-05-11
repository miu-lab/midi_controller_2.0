#include "MidiSubsystem.hpp"

#include <Arduino.h>

#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "config/MappingConfiguration.hpp"
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

    // Récupérer un MidiOutputPort existant ou en créer un nouveau si nécessaire
    midiOut_ = container_->resolve<MidiOutputPort>();
    if (!midiOut_) {
        // Créer l'interface MIDI appropriée si aucune n'est enregistrée
        if (configuration_->isHardwareInitEnabled()) {
            // Utiliser TeensyUsbMidiOut qui implémente MidiOutputPort
            midiOut_ = std::make_shared<TeensyUsbMidiOut>();
            if (!midiOut_) {
                return Result<bool, std::string>::error("Failed to create TeensyUsbMidiOut");
            }
        } else {
            Serial.println(F("MidiSubsystem: Creating TeensyUsbMidiOut (test mode)"));
            // Pour l'instant, utiliser TeensyUsbMidiOut même en mode test
            midiOut_ = std::make_shared<TeensyUsbMidiOut>();
            if (!midiOut_) {
                return Result<bool, std::string>::error("Failed to create TeensyUsbMidiOut");
            }
        }

        // Enregistrer l'implémentation que nous venons de créer
        container_->registerImplementation<MidiOutputPort, MidiOutputPort>(midiOut_);
    } else {
    }

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

    // Charger les mappings MIDI depuis la configuration
    MappingConfiguration mappingConfig;
    const auto& midiMappings = mappingConfig.getAllMidiMappings();

    int mappingCount = 0;
    for (const auto& mapping : midiMappings) {
        // Créer une stratégie de mapping appropriée
        std::unique_ptr<IMidiMappingStrategy> strategy;
        if (mapping.midiControl.relative) {
            strategy = MidiMappingFactory::createRelative();
        } else {
            strategy = MidiMappingFactory::createAbsolute(0, 127);
        }

        // Ajouter le mapping au MidiMapper
        midiMapper_->setMapping(mapping.controlId, mapping.midiControl, std::move(strategy));

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

bool MidiSubsystem::configureCallbacks(std::shared_ptr<InputController> inputController) {
    if (!initialized_ || !midiMapper_) {
        return false;
    }

    if (!inputController) {
        return false;
    }

    // Capturer une référence au MidiMapper
    auto& mapper = *midiMapper_;

    // Configurer le callback pour les rotations d'encodeur
    inputController->setMidiEncoderCallback(
        [&mapper](EncoderId id, int32_t position, int8_t delta) {
            mapper.processEncoderChange(id, position);
        });

    // Configurer le callback pour les boutons d'encodeur
    inputController->setMidiEncoderButtonCallback(
        [&mapper](EncoderId id, bool pressed) { mapper.processEncoderButton(id, pressed); });

    // Configurer le callback pour les boutons standard
    inputController->setMidiButtonCallback(
        [&mapper](ButtonId id, bool pressed) { mapper.processButtonPress(id, pressed); });

    return true;
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

    // Débogage périodique toutes les 60 secondes
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastDebugTime > 60000) {
        lastDebugTime = currentTime;
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
    midiOut_->sendCc(MidiChannel(channel), MidiCC(controller), value);
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