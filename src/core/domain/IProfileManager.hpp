#pragma once

#include "core/ports/output/ProfileStoragePort.hpp"

/**
 * @brief Alias pour maintenir la compatibilité
 * @deprecated Utiliser ProfileStoragePort à la place
 */
class IProfileManager : public ProfileStoragePort {
public:
    // Méthodes virtuelles pour maintenir la compatibilité avec l'ancienne interface
    virtual std::vector<MidiControlMapping> getAllMappings() const override {
        return {};
    }

    virtual bool saveProfile() override {
        return false;
    }

    virtual bool loadProfile() override {
        return false;
    }

    virtual void resetToDefaults() override {}

    virtual bool removeBinding(ControlId id) override {
        return false;
    }
};
