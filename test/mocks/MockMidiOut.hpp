#pragma once

#include "core/ports/output/MidiOutputPort.hpp"
#include "core/utils/Result.hpp"
#include <vector>
#include <cstdint>

/**
 * @brief Mock pour MidiOutputPort permettant d'isoler les tests MIDI
 * 
 * Capture les messages MIDI envoyés pour validation dans les tests
 * sans dépendre du hardware réel.
 */
class MockMidiOut : public MidiOutputPort {
public:
    struct CapturedMessage {
        enum Type { NOTE_ON, NOTE_OFF, CONTROL_CHANGE };
        Type type;
        uint8_t channel;
        uint8_t data1;
        uint8_t data2;
        unsigned long timestamp;
    };

    MockMidiOut() : call_count_(0) {}

    // Interface MidiOutputPort
    Result<bool> sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {
        if (should_fail_) {
            return Result<bool>::error({ErrorCode::OperationFailed, "Mock MIDI failure"});
        }
        
        CapturedMessage msg;
        msg.type = CapturedMessage::NOTE_ON;
        msg.channel = channel;
        msg.data1 = note;
        msg.data2 = velocity;
        msg.timestamp = millis();
        
        captured_messages_.push_back(msg);
        call_count_++;
        
        return Result<bool>::success(true);
    }

    Result<bool> sendNoteOff(uint8_t channel, uint8_t note) override {
        if (should_fail_) {
            return Result<bool>::error({ErrorCode::OperationFailed, "Mock MIDI failure"});
        }
        
        CapturedMessage msg;
        msg.type = CapturedMessage::NOTE_OFF;
        msg.channel = channel;
        msg.data1 = note;
        msg.data2 = 0;
        msg.timestamp = millis();
        
        captured_messages_.push_back(msg);
        call_count_++;
        
        return Result<bool>::success(true);
    }

    Result<bool> sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) override {
        if (should_fail_) {
            return Result<bool>::error({ErrorCode::OperationFailed, "Mock MIDI failure"});
        }
        
        CapturedMessage msg;
        msg.type = CapturedMessage::CONTROL_CHANGE;
        msg.channel = channel;
        msg.data1 = controller;
        msg.data2 = value;
        msg.timestamp = millis();
        
        captured_messages_.push_back(msg);
        call_count_++;
        
        return Result<bool>::success(true);
    }

    // Méthodes de test
    void reset() {
        captured_messages_.clear();
        call_count_ = 0;
        should_fail_ = false;
    }

    void setShouldFail(bool fail) {
        should_fail_ = fail;
    }

    size_t getCallCount() const {
        return call_count_;
    }

    const std::vector<CapturedMessage>& getCapturedMessages() const {
        return captured_messages_;
    }

    bool hasMessage(CapturedMessage::Type type, uint8_t channel, uint8_t data1, uint8_t data2) const {
        for (const auto& msg : captured_messages_) {
            if (msg.type == type && msg.channel == channel && 
                msg.data1 == data1 && msg.data2 == data2) {
                return true;
            }
        }
        return false;
    }

    CapturedMessage getLastMessage() const {
        if (captured_messages_.empty()) {
            // Retourner un message vide par défaut
            CapturedMessage empty = {};
            return empty;
        }
        return captured_messages_.back();
    }

private:
    std::vector<CapturedMessage> captured_messages_;
    size_t call_count_;
    bool should_fail_ = false;
};