#include "MidiSubsystem.hpp"

#include <Arduino.h>
#include <set>


#include "core/utils/Error.hpp"

#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/strategies/AbsoluteMappingStrategy.hpp"
#include "core/domain/strategies/MidiMappingFactory.hpp"
#include "core/domain/strategies/RelativeMappingStrategy.hpp"

MidiSubsystem::MidiSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {}

Result<bool> MidiSubsystem::init() {
    if (initialized_) {
        return Result<bool>::success(true);
    }

    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Failed to resolve IConfiguration"});
    }

    // Récupérer ou créer le gestionnaire de commandes
    commandManager_ = container_->resolve<CommandManager>();
    if (!commandManager_) {
        commandManager_ = std::make_shared<CommandManager>();
        container_->registerDependency<CommandManager>(commandManager_);
    } else {
        // TODO DEBUG MSG
    }

    // Créer l'interface MIDI de base (TeensyUsbMidiOut)
    std::shared_ptr<TeensyUsbMidiOut> baseMidiOut;
    if (configuration_->isHardwareInitEnabled()) {
        baseMidiOut = std::make_shared<TeensyUsbMidiOut>();
        if (!baseMidiOut) {
            return Result<bool>::error({ErrorCode::InitializationFailed, "Failed to create TeensyUsbMidiOut"});
        }
    } else {
        // TODO DEBUG MSG
        baseMidiOut = std::make_shared<TeensyUsbMidiOut>();
        if (!baseMidiOut) {
            return Result<bool>::error({ErrorCode::InitializationFailed, "Failed to create TeensyUsbMidiOut"});
        }
    }

    // Créer l'EventEnabledMidiOut qui va décorer directement TeensyUsbMidiOut
    auto eventEnabledMidiOut = std::make_shared<EventEnabledMidiOut>(*baseMidiOut);
    if (!eventEnabledMidiOut) {
        return Result<bool>::error({ErrorCode::InitializationFailed, "Failed to create EventEnabledMidiOut"});
    }
    // TODO DEBUG MSG

    // Utiliser EventEnabledMidiOut comme interface MidiOutputPort
    midiOut_ = eventEnabledMidiOut;

    // Enregistrer l'implémentation que nous venons de créer
    container_->registerImplementation<MidiOutputPort, MidiOutputPort>(midiOut_);

    // Enregistrer également les objets intermédiaires pour éviter qu'ils soient détruits
    container_->registerDependency<TeensyUsbMidiOut>(baseMidiOut);
    container_->registerDependency<EventEnabledMidiOut>(eventEnabledMidiOut);

    // Créer le MidiMapper et MidiInHandler
    // TODO DEBUG MSG
    midiMapper_ = std::make_unique<MidiMapper>(*midiOut_, *commandManager_);
    if (!midiMapper_) {
        return Result<bool>::error({ErrorCode::InitializationFailed, "Failed to create MidiMapper"});
    }
    // TODO DEBUG MSG

    midiInHandler_ = std::make_unique<MidiInHandler>();
    if (!midiInHandler_) {
        return Result<bool>::error({ErrorCode::InitializationFailed, "Failed to create MidiInHandler"});
    }
    // TODO DEBUG MSG

    // Charger les mappings MIDI depuis les ControlDefinition
    loadMidiMappingsFromControlDefinitions();

    // Enregistrer ce sous-système comme implémentation de IMidiSystem
    container_->registerImplementation<IMidiSystem, MidiSubsystem>(
        std::shared_ptr<MidiSubsystem>(this, [](MidiSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    initialized_ = true;
    return Result<bool>::success(true);
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

Result<bool> MidiSubsystem::sendNoteOn(uint8_t channel, uint8_t note,
                                                    uint8_t velocity) {
    if (!initialized_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "MidiSubsystem: Not initialized"});
    }

    if (!midiOut_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "MidiSubsystem: No MIDI output available"});
    }

    // Conversion des types pour correspondre à la signature attendue
    // Pas d'utilisation d'exceptions en environnement embarqué
    midiOut_->sendNoteOn(MidiChannel(channel), MidiNote(note), velocity);
    return Result<bool>::success(true);
}

Result<bool> MidiSubsystem::sendNoteOff(uint8_t channel, uint8_t note) {
    if (!initialized_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "MidiSubsystem: Not initialized"});
    }

    if (!midiOut_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "MidiSubsystem: No MIDI output available"});
    }

    // sendNoteOff attend un troisième paramètre (velocity)
    midiOut_->sendNoteOff(MidiChannel(channel), MidiNote(note), 0);
    return Result<bool>::success(true);
}

Result<bool> MidiSubsystem::sendControlChange(uint8_t channel, uint8_t controller,
                                                           uint8_t value) {
    if (!initialized_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "MidiSubsystem: Not initialized"});
    }

    if (!midiOut_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "MidiSubsystem: No MIDI output available"});
    }

    // La méthode s'appelle sendCc et non sendControlChange
    midiOut_->sendControlChange(MidiChannel(channel), MidiCC(controller), value);
    return Result<bool>::success(true);
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
        // TODO DEBUG MSG
        return;
    }
    
    // Obtenir toutes les définitions de contrôles depuis le système unifié
    const auto& allControlDefinitions = configuration_->getAllControlDefinitions();
    
    // TODO DEBUG MSG
    
    int mappingCount = 0;
    std::set<InputId> navigationControlIds;
    
    // Utiliser directement les mappings intégrés dans ControlDefinition
    for (const auto& controlDef : allControlDefinitions) {
        if (!controlDef.enabled) continue;
        
        // Vérifier si le contrôle est pour la navigation
        for (const auto& mapping : controlDef.mappings) {
            if (mapping.role == MappingRole::NAVIGATION) {
                navigationControlIds.insert(controlDef.id);
                break; // Un seul mapping de navigation suffit
            }
        }
        
        // Configurer les mappings MIDI depuis cette définition de contrôle
        setupMidiMappingFromControlDefinition(controlDef);
        mappingCount++;
    }
    
    // Configurer les contrôles de navigation dans le mapper
    midiMapper_->setNavigationControls(navigationControlIds);
    
    // TODO DEBUG MSG
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
        
        // TODO DEBUG MSG
    }
}