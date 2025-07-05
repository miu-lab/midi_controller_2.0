#pragma once

#include <atomic>
#include <cstddef>
#include <type_traits>

/**
 * @brief Ring buffer thread-safe pour systèmes embarqués
 * 
 * Implémentation lock-free avec allocation statique pour éviter
 * la fragmentation mémoire dans les chemins temps réel.
 * 
 * @tparam T Type d'élément stocké
 * @tparam N Taille du buffer (doit être puissance de 2 pour optimisation)
 */
template<typename T, size_t N>
class RingBuffer {
public:
    static_assert(N > 0 && (N & (N - 1)) == 0, "Buffer size must be a power of 2");
    static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
    
    /**
     * @brief Constructeur par défaut
     */
    RingBuffer() : read_pos_(0), write_pos_(0) {}
    
    /**
     * @brief Destructeur par défaut
     */
    ~RingBuffer() = default;
    
    /**
     * @brief Écrit un élément dans le buffer
     * 
     * @param item Élément à écrire
     * @return true si l'écriture a réussi, false si le buffer est plein
     */
    bool write(const T& item) {
        const size_t current_write = write_pos_.load(std::memory_order_relaxed);
        const size_t next_write = (current_write + 1) & (N - 1);
        
        // Vérifier si le buffer est plein
        if (next_write == read_pos_.load(std::memory_order_acquire)) {
            return false; // Buffer plein
        }
        
        // Écrire l'élément
        buffer_[current_write] = item;
        
        // Publier la nouvelle position d'écriture
        write_pos_.store(next_write, std::memory_order_release);
        
        return true;
    }
    
    /**
     * @brief Lit un élément du buffer
     * 
     * @param item Référence où stocker l'élément lu
     * @return true si la lecture a réussi, false si le buffer est vide
     */
    bool read(T& item) {
        const size_t current_read = read_pos_.load(std::memory_order_relaxed);
        
        // Vérifier si le buffer est vide
        if (current_read == write_pos_.load(std::memory_order_acquire)) {
            return false; // Buffer vide
        }
        
        // Lire l'élément
        item = buffer_[current_read];
        
        // Publier la nouvelle position de lecture
        read_pos_.store((current_read + 1) & (N - 1), std::memory_order_release);
        
        return true;
    }
    
    /**
     * @brief Regarde le prochain élément sans le consommer
     * 
     * @param item Référence où stocker l'élément
     * @return true si un élément est disponible, false sinon
     */
    bool peek(T& item) const {
        const size_t current_read = read_pos_.load(std::memory_order_relaxed);
        
        // Vérifier si le buffer est vide
        if (current_read == write_pos_.load(std::memory_order_acquire)) {
            return false; // Buffer vide
        }
        
        // Lire l'élément sans avancer la position
        item = buffer_[current_read];
        return true;
    }
    
    /**
     * @brief Vérifie si le buffer est vide
     * 
     * @return true si vide, false sinon
     */
    bool is_empty() const {
        return read_pos_.load(std::memory_order_relaxed) == 
               write_pos_.load(std::memory_order_relaxed);
    }
    
    /**
     * @brief Vérifie si le buffer est plein
     * 
     * @return true si plein, false sinon
     */
    bool is_full() const {
        const size_t current_write = write_pos_.load(std::memory_order_relaxed);
        const size_t next_write = (current_write + 1) & (N - 1);
        return next_write == read_pos_.load(std::memory_order_relaxed);
    }
    
    /**
     * @brief Obtient le nombre approximatif d'éléments dans le buffer
     * 
     * Note: Cette valeur peut être imprécise dans un contexte multi-thread
     * mais donne une estimation utile pour le monitoring.
     * 
     * @return Nombre d'éléments approximatif
     */
    size_t size() const {
        const size_t write_pos = write_pos_.load(std::memory_order_relaxed);
        const size_t read_pos = read_pos_.load(std::memory_order_relaxed);
        return (write_pos - read_pos) & (N - 1);
    }
    
    /**
     * @brief Obtient la capacité maximale du buffer
     * 
     * @return Capacité maximale (N-1 car une position est réservée)
     */
    constexpr size_t capacity() const {
        return N - 1;
    }
    
    /**
     * @brief Vide le buffer en avançant la position de lecture
     * 
     * Note: Cette opération n'est pas thread-safe avec les écritures
     */
    void clear() {
        read_pos_.store(write_pos_.load(std::memory_order_relaxed), 
                       std::memory_order_relaxed);
    }
    
    /**
     * @brief Obtient le taux d'occupation du buffer
     * 
     * @return Ratio entre 0.0 (vide) et 1.0 (plein)
     */
    float usage_ratio() const {
        return static_cast<float>(size()) / static_cast<float>(capacity());
    }

private:
    // Empêcher la copie et l'assignation
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&&) = delete;
    RingBuffer& operator=(RingBuffer&&) = delete;
    
    // Buffer de données avec alignement optimal
    alignas(64) T buffer_[N];  // Aligné sur ligne de cache pour performance
    
    // Positions atomiques pour thread-safety
    std::atomic<size_t> read_pos_;
    std::atomic<size_t> write_pos_;
};

/**
 * @brief Spécialisations pour messages MIDI avec tailles optimisées
 */
namespace MidiBuffers {
    
    /**
     * @brief Structure pour un message MIDI complet
     */
    struct MidiMessage {
        uint8_t status;      // Status byte (type + channel)
        uint8_t data1;       // Premier data byte
        uint8_t data2;       // Deuxième data byte
        uint32_t timestamp;  // Timestamp en microsecondes
        
        MidiMessage() : status(0), data1(0), data2(0), timestamp(0) {}
        
        MidiMessage(uint8_t s, uint8_t d1, uint8_t d2, uint32_t ts = 0)
            : status(s), data1(d1), data2(d2), timestamp(ts) {}
    };
    
    /**
     * @brief Buffer haute performance pour messages MIDI entrants
     * 
     * Taille 256 = 2^8 pour optimisation des opérations modulo
     */
    using IncomingMidiBuffer = RingBuffer<MidiMessage, 256>;
    
    /**
     * @brief Buffer pour messages MIDI sortants
     * 
     * Taille plus petite car généralement moins de trafic sortant
     */
    using OutgoingMidiBuffer = RingBuffer<MidiMessage, 128>;
    
    /**
     * @brief Buffer pour événements MIDI haute priorité
     * 
     * Utilisé pour les événements qui doivent être traités immédiatement
     */
    using HighPriorityMidiBuffer = RingBuffer<MidiMessage, 64>;
    
} // namespace MidiBuffers