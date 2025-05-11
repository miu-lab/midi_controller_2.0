#pragma once

#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/utils/Result.hpp"
#include <vector>
#include <tuple>

class MockMidiSubsystem : public IMidiSystem {
public:
    bool initCalled = false;
    bool updateCalled = false;
    
    struct MidiMessage {
        enum class Type { NoteOn, NoteOff, ControlChange };
        Type type;
        uint8_t channel;
        uint8_t data1; // note or controller
        uint8_t data2; // velocity or value
    };
    
    std::vector<MidiMessage> sentMessages;

    // Implémentation de IMidiSystem
    Result<bool, std::string> init() override {
        initCalled = true;
        return Result<bool, std::string>::success(true);
    }

    void update() override {
        updateCalled = true;
    }

    Result<bool, std::string> sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {
        MidiMessage msg{MidiMessage::Type::NoteOn, channel, note, velocity};
        sentMessages.push_back(msg);
        return Result<bool, std::string>::success(true);
    }

    Result<bool, std::string> sendNoteOff(uint8_t channel, uint8_t note) override {
        MidiMessage msg{MidiMessage::Type::NoteOff, channel, note, 0};
        sentMessages.push_back(msg);
        return Result<bool, std::string>::success(true);
    }

    Result<bool, std::string> sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) override {
        MidiMessage msg{MidiMessage::Type::ControlChange, channel, controller, value};
        sentMessages.push_back(msg);
        return Result<bool, std::string>::success(true);
    }
    
    // Méthodes d'aide pour les tests
    void clearMessages() {
        sentMessages.clear();
    }
    
    bool hasMessage(MidiMessage::Type type, uint8_t channel, uint8_t data1, uint8_t data2 = 0) {
        for (const auto& msg : sentMessages) {
            if (msg.type == type && msg.channel == channel && 
                msg.data1 == data1 && (type == MidiMessage::Type::NoteOff || msg.data2 == data2)) {
                return true;
            }
        }
        return false;
    }
};
