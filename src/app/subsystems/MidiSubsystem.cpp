#include "MidiSubsystem.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"

MidiSubsystem::MidiSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {
}

void MidiSubsystem::init() {
    if (initialized_) {
        return;
    }
    
    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        // Si aucune configuration n'est trouvée, on ne peut pas initialiser correctement
        return;
    }
    
    // Récupérer un IMidiOut existant ou en créer un nouveau si nécessaire
    midiOut_ = container_->resolve<IMidiOut>();
    if (!midiOut_) {
        // Créer l'interface MIDI appropriée si aucune n'est enregistrée
        if (configuration_->isHardwareInitEnabled()) {
            // Utiliser TeensyUsbMidiOut qui implémente IMidiOut
            midiOut_ = std::make_shared<TeensyUsbMidiOut>();
        } else {
            // Pour l'instant, utiliser TeensyUsbMidiOut même en mode test
            midiOut_ = std::make_shared<TeensyUsbMidiOut>();
        }
        
        // Enregistrer l'implémentation que nous venons de créer
        container_->registerImplementation<IMidiOut, IMidiOut>(midiOut_);
    }
    
    // Enregistrer ce sous-système comme implémentation de IMidiSystem
    container_->registerImplementation<IMidiSystem, MidiSubsystem>(
        std::shared_ptr<MidiSubsystem>(this, [](MidiSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        })
    );
    
    initialized_ = true;
}

void MidiSubsystem::update() {
    // Pour l'instant, pas de logique de mise à jour périodique nécessaire
}

void MidiSubsystem::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (!initialized_ || !midiOut_) {
        std::cout << "MidiSubsystem: Not initialized or midiOut is null" << std::endl;
        return;
    }
    
    std::cout << "MidiSubsystem: Sending NoteOn " << (int)channel << ", " << (int)note << ", " << (int)velocity << std::endl;
    
    // Conversion des types pour correspondre à la signature attendue
    midiOut_->sendNoteOn(MidiChannel(channel), MidiNote(note), velocity);
}

void MidiSubsystem::sendNoteOff(uint8_t channel, uint8_t note) {
    if (!initialized_ || !midiOut_) {
        return;
    }
    
    // sendNoteOff attend un troisième paramètre (velocity)
    midiOut_->sendNoteOff(MidiChannel(channel), MidiNote(note), 0);
}

void MidiSubsystem::sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) {
    if (!initialized_ || !midiOut_) {
        return;
    }
    
    // La méthode s'appelle sendCc et non sendControlChange
    midiOut_->sendCc(MidiChannel(channel), MidiCC(controller), value);
}