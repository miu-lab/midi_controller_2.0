#include "MidiSubsystem.hpp"

#include <Arduino.h>

#include "adapters/secondary/midi/BufferedMidiOut.hpp"
#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
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
    
    // Créer BufferedMidiOut comme premier décorateur avec flush immédiat activé
    auto bufferedMidiOut = std::make_shared<BufferedMidiOut>(*baseMidiOut, 128, true); // Buffer de taille 128
    if (!bufferedMidiOut) {
        return Result<bool, std::string>::error("Failed to create BufferedMidiOut");
    }
    Serial.println(F("MidiSubsystem: Created BufferedMidiOut wrapper (immediate flush mode)"));

    // Créer l'EventEnabledMidiOut qui va décorer BufferedMidiOut
    auto eventEnabledMidiOut = std::make_shared<EventEnabledMidiOut>(*bufferedMidiOut);
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
    container_->registerDependency<BufferedMidiOut>(bufferedMidiOut);
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

    // Configuration des callbacks directs pour le chemin critique MIDI
    // Capture de 'this' par référence pour éviter toute capture inutile d'instance
    inputController->onEncoderChangedDirect = [this](EncoderId id, int32_t position) {
        this->getMidiMapper().processEncoderChange(id, position);
    };

    inputController->onEncoderButtonDirect = [this](EncoderId id, bool pressed) {
        this->getMidiMapper().processEncoderButton(id, pressed);
    };

    inputController->onButtonDirect = [this](ButtonId id, bool pressed) {
        this->getMidiMapper().processButtonPress(id, pressed);
    };

    Serial.println(F("MidiSubsystem: Callbacks directs configurés"));
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
    
    // Mettre à jour BufferedMidiOut pour envoyer les messages non-immédiats
    auto bufferedMidiOut = container_->resolve<BufferedMidiOut>();
    if (bufferedMidiOut) {
        // Même avec le mode flush immédiat, certains messages pourraient être en attente
        // (par exemple ceux qui étaient en buffer avant de passer en mode immédiat)
        unsigned int sentCount = bufferedMidiOut->update(8); // Max 8 messages par cycle
        
        // Débogage périodique si des messages sont en attente
        if (sentCount > 0 && configuration_->isDebugEnabled()) {
            Serial.printf(F("MidiSubsystem: Sent %u buffered MIDI messages\n"), sentCount);
        }
    }

    // Débogage périodique toutes les 60 secondes
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastDebugTime > 60000) {
        if (configuration_->isDebugEnabled()) {
            auto bufferedMidiOut = container_->resolve<BufferedMidiOut>();
            if (bufferedMidiOut) {
                Serial.printf(F("MidiSubsystem: %u pending MIDI messages (%s mode)\n"), 
                             bufferedMidiOut->getPendingCount(),
                             bufferedMidiOut->isImmediateFlush() ? "immediate" : "buffered");
            }
        }
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