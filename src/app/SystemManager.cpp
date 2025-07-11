#include "SystemManager.hpp"

#include "app/InitializationScript.hpp"

SystemManager::SystemManager()
    : currentState_(State::UNINITIALIZED), lastErrorTime_(0) {}

Result<void> SystemManager::initialize() {
    Serial.begin(SystemConstants::Hardware::SERIAL_BAUD_RATE);
    Serial.println("=== 🎹 MIDI Controller Initialization ===");
    Serial.print("Build: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);

    auto result = performInitialization();

    if (result.isSuccess()) {
        currentState_ = State::RUNNING;
        Serial.println("=== ✅ Initialization Complete ===");
    } else {
        Serial.println("=== ❌ Initialization Failed ===");
        enterRecoveryMode();
    }

    return result;
}

void SystemManager::update() {
    switch (currentState_) {
    case State::RUNNING:
        updateRunningState();
        break;

    case State::RECOVERY_MODE:
        updateRecoveryMode();
        break;

    case State::ERROR_STATE:
        // État d'erreur critique - système arrêté
        delay(1000);
        Serial.println("💀 System in critical error state");
        break;

    case State::UNINITIALIZED:
    case State::INITIALIZING:
        // États transitoires - ne rien faire
        delay(10);
        break;
    }
}

void SystemManager::shutdown() {
    Serial.println("🛑 System shutdown requested");
    cleanup();
    currentState_ = State::ERROR_STATE;
}

Result<void> SystemManager::performInitialization() {
    currentState_ = State::INITIALIZING;

    Serial.println("📦 Creating DependencyContainer...");
    container_ = std::make_shared<DependencyContainer>();
    if (!container_) {
        return Result<void>::error(
            {ErrorCode::InitializationFailed, "Failed to create DependencyContainer"});
    }

    Serial.println("🔧 Initializing container...");
    auto initResult = InitializationScript::initializeContainer(container_, appConfig_);
    if (initResult.isError()) {
        logError("InitializationScript", initResult.error().value());
        return Result<void>::error(initResult.error().value());
    }

    Serial.println("🚀 Creating MidiControllerApp...");
    app_ = std::make_shared<MidiControllerApp>(container_);
    if (!app_) {
        return Result<void>::error(
            {ErrorCode::InitializationFailed, "Failed to create MidiControllerApp"});
    }

    auto appInitResult = app_->init();
    if (appInitResult.isError()) {
        logError("MidiControllerApp::init", appInitResult.error().value());
        return Result<void>::error(appInitResult.error().value());
    }

    Serial.println("✅ System validation...");
    // Validation post-initialisation
    if (!app_) {
        return Result<void>::error({ErrorCode::OperationFailed, "App validation failed"});
    }

    return Result<void>::success();
}

void SystemManager::enterRecoveryMode() {
    currentState_ = State::RECOVERY_MODE;
    lastErrorTime_ = millis();

    Serial.println("🔄 Entering recovery mode...");
    Serial.println("   System will attempt restart in 5 seconds");

    cleanup();
}

Result<void> SystemManager::attemptRecovery() {
    Serial.println("🔄 Attempting system recovery...");
    return performInitialization();
}

void SystemManager::updateRunningState() {
    if (app_) {
        app_->update();
    } else {
        Serial.println("⚠️  App became null during runtime, entering recovery mode");
        enterRecoveryMode();
    }
}

void SystemManager::updateRecoveryMode() {
    // Tentative de récupération après délai
    if (millis() - lastErrorTime_ >= ERROR_RECOVERY_DELAY) {
        auto result = attemptRecovery();
        if (result.isSuccess()) {
            currentState_ = State::RUNNING;
            Serial.println("✅ System recovery successful!");
        } else {
            Serial.println("❌ Recovery failed, retrying in 5 seconds...");
            lastErrorTime_ = millis();  // Reset timer for next attempt
        }
    }
}

void SystemManager::logError(const char* context, const Error& error) {
    Serial.print("❌ ERREUR [");
    Serial.print(context);
    Serial.print("]: Code=");
    Serial.print(static_cast<int>(error.code));
    Serial.print(", Message=\"");
    Serial.print(error.message);
    Serial.println("\"");
}

void SystemManager::cleanup() {
    // Cleanup basic - reset des shared_ptr
    app_.reset();
    container_.reset();
}