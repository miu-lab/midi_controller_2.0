#pragma once
#include "core/ports/output/MidiOutputPort.hpp"

/**
 * @brief Mock de MidiOutputPort pour les tests
 */
class MockMidiOut : public MidiOutputPort {
public:
    MockMidiOut() : noteOnCount_(0), noteOffCount_(0), ccCount_(0) {}

    void sendNoteOn(MidiChannel channel, MidiNote note, uint8_t velocity) override {
        lastChannel_ = channel;
        lastNote_ = note;
        lastVelocity_ = velocity;
        noteOnCount_++;
    }

    void sendNoteOff(MidiChannel channel, MidiNote note, uint8_t velocity) override {
        lastChannel_ = channel;
        lastNote_ = note;
        lastReleaseVelocity_ = velocity;
        noteOffCount_++;
    }

    void sendCc(MidiChannel channel, MidiCC controller, uint8_t value) override {
        lastChannel_ = channel;
        lastController_ = controller;
        lastValue_ = value;
        ccCount_++;
    }

    // Implémentation des autres méthodes requises
    void sendProgramChange(MidiChannel ch, uint8_t program) override {
        // Non utilisé dans les tests actuels
    }

    void sendPitchBend(MidiChannel ch, uint16_t value) override {
        // Non utilisé dans les tests actuels
    }

    void sendChannelPressure(MidiChannel ch, uint8_t pressure) override {
        // Non utilisé dans les tests actuels
    }

    void sendSysEx(const uint8_t* data, uint16_t length) override {
        // Non utilisé dans les tests actuels
    }

    // Accesseurs pour les tests
    int getNoteOnCount() const {
        return noteOnCount_;
    }

    int getNoteOffCount() const {
        return noteOffCount_;
    }

    // Méthode spécifique pour les tests (fournit le compteur de CC sous un nom compatible avec les
    // tests existants)
    int getControlChangeCount() const {
        return ccCount_;
    }

    MidiChannel getLastChannel() const {
        return lastChannel_;
    }

    MidiNote getLastNote() const {
        return lastNote_;
    }

    uint8_t getLastVelocity() const {
        return lastVelocity_;
    }

    MidiCC getLastController() const {
        return lastController_;
    }

    uint8_t getLastValue() const {
        return lastValue_;
    }

private:
    int noteOnCount_;
    int noteOffCount_;
    int ccCount_;

    MidiChannel lastChannel_ = MidiChannel(0);
    MidiNote lastNote_ = MidiNote(0);
    uint8_t lastVelocity_ = 0;
    uint8_t lastReleaseVelocity_ = 0;
    MidiCC lastController_ = MidiCC(0);
    uint8_t lastValue_ = 0;
};