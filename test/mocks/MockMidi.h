#pragma once
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/utils/Result.hpp"
#include <vector>

/**
 * @brief Mock du système MIDI pour les tests
 */
class MockMidi : public IMidiSystem {
public:
    void init() override { initialized = true; }
    void update() override { updateCount++; }
    
    Result<bool, std::string> sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {
        noteOnMessages.push_back({channel, note, velocity});
        return Result<bool, std::string>::success(true);
    }
    
    Result<bool, std::string> sendNoteOff(uint8_t channel, uint8_t note) override {
        noteOffMessages.push_back({channel, note});
        return Result<bool, std::string>::success(true);
    }
    
    Result<bool, std::string> sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) override {
        ccMessages.push_back({channel, controller, value});
        return Result<bool, std::string>::success(true);
    }
    
    // Variables pour vérifier l'état
    bool initialized = false;
    int updateCount = 0;
    
    // Structures pour stocker les messages
    struct NoteOnMessage {
        uint8_t channel;
        uint8_t note;
        uint8_t velocity;
    };
    
    struct NoteOffMessage {
        uint8_t channel;
        uint8_t note;
    };
    
    struct CCMessage {
        uint8_t channel;
        uint8_t controller;
        uint8_t value;
    };
    
    std::vector<NoteOnMessage> noteOnMessages;
    std::vector<NoteOffMessage> noteOffMessages;
    std::vector<CCMessage> ccMessages;
};
