#pragma once

#include <cstdint>

// Enum pour des codes d'erreur spécifiques et légers
enum class ErrorCode : uint8_t {
    OK = 0,
    Unknown,
    InvalidArgument,
    DependencyMissing,
    InitializationFailed,
    HardwareError,
    ConfigError,
    NotImplemented,
    OperationFailed
};

// Structure pour encapsuler une erreur
struct Error {
    ErrorCode code;
    const char* message; // Pointeur vers une chaîne statique (littéral)

    // Constructeur pour les cas de succès (implicite)
    constexpr Error() : code(ErrorCode::OK), message("Success") {}

    // Constructeur pour les erreurs réelles
    constexpr Error(ErrorCode c, const char* msg) : code(c), message(msg) {}

    // Opérateur pour vérifier si une erreur s'est produite
    explicit operator bool() const {
        return code != ErrorCode::OK;
    }
};

// Création de quelques erreurs communes statiques pour éviter la duplication
namespace Errors {
    constexpr Error NoError = {ErrorCode::OK, "Success"};
    constexpr Error Unknown = {ErrorCode::Unknown, "An unknown error occurred"};
    constexpr Error DepMissing = {ErrorCode::DependencyMissing, "A required dependency was missing"};
    constexpr Error InitFailed = {ErrorCode::InitializationFailed, "Initialization failed"};
    constexpr Error HwError = {ErrorCode::HardwareError, "A hardware error occurred"};
}
