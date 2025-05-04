#pragma once
#include "domain/IMidiOut.hpp"
#include "storage/ProfileManager.hpp"

class InputRouter {
public:
InputRouter(IMidiOut& midiOut, IProfileManager& profileManager);
    void init();  // s'abonne aux événements
private:
    IMidiOut& midiOut_;
    IProfileManager& profileManager_;
};
