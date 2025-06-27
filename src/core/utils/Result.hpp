#pragma once

#include <optional>
#include <variant>
#include "Error.hpp" // Inclure notre nouvelle classe Error

/**
 * @file Result.hpp
 * @brief Classe utilitaire pour représenter le résultat d'une opération qui peut réussir ou échouer
 */

/**
 * @brief Classe générique pour représenter le résultat d'une opération pouvant échouer
 * 
 * Remplace les codes de retour et pointeurs null par une structure plus expressive
 * et sécurisée.
 * 
 * @tparam T Type de la valeur en cas de succès
 * @tparam E Type de l'erreur en cas d'échec (notre struct Error par défaut)
 */
template<typename T, typename E = Error>
class Result {
public:
  /**
   * @brief Construit un résultat représentant un succès
   * @param value Valeur du résultat
   * @return Result contenant la valeur
   */
  static Result<T, E> success(const T& value) {
    return Result(value);
  }
  
  /**
   * @brief Construit un résultat représentant une erreur
   * @param error Description de l'erreur
   * @return Result contenant l'erreur
   */
  static Result<T, E> error(const E& error) {
    return Result(error);
  }
  
  /**
   * @brief Vérifie si le résultat est un succès
   * @return true si succès, false si erreur
   */
  bool isSuccess() const {
    return std::holds_alternative<T>(result_);
  }
  
  /**
   * @brief Vérifie si le résultat est une erreur
   * @return true si erreur, false si succès
   */
  bool isError() const {
    return !isSuccess();
  }
  
  /**
   * @brief Récupère la valeur si succès
   * @return std::optional contenant la valeur si succès, vide sinon
   */
  std::optional<T> value() const {
    if (isSuccess()) {
      return std::get<T>(result_);
    }
    return std::nullopt;
  }
  
  /**
   * @brief Récupère l'erreur si échec
   * @return std::optional contenant l'erreur si échec, vide sinon
   */
  std::optional<E> error() const {
    if (!isSuccess()) {
      return std::get<E>(result_);
    }
    return std::nullopt;
  }
  
  /**
   * @brief Exécute l'un des deux callbacks selon le résultat
   * 
   * @tparam SuccessCallback Type du callback en cas de succès
   * @tparam ErrorCallback Type du callback en cas d'erreur
   * @param onSuccess Callback appelé en cas de succès avec la valeur
   * @param onError Callback appelé en cas d'erreur avec l'erreur
   * @return Auto - Le résultat du callback exécuté
   */
  template<typename SuccessCallback, typename ErrorCallback>
  auto match(SuccessCallback&& onSuccess, ErrorCallback&& onError) const {
    if (isSuccess()) {
      return onSuccess(*value());
    } else {
      return onError(*error());
    }
  }
  
private:
  std::variant<T, E> result_;
  
  explicit Result(const T& value) : result_(value) {}
  explicit Result(const E& error) : result_(error) {}
};
