#include "adapters/secondary/midi/BufferedMidiOut.hpp"

#include "config/debug/DebugMacros.hpp"

BufferedMidiOut::BufferedMidiOut(MidiOutputPort& output, uint16_t bufferSize)
    : output_(output),
      bufferSize_(bufferSize > 0 ? bufferSize : DEFAULT_BUFFER_SIZE),
      nextIndex_(0),
      dirtyCount_(0),
      highPriority_(false),
      usingDynamicBuffer_(bufferSize > STATIC_BUFFER_SIZE) {
    // Utiliser le buffer statique si possible, sinon allouer dynamiquement
    if (usingDynamicBuffer_) {
        dynamicBuffer_ = std::make_unique<MidiMessage[]>(bufferSize_);
        buffer_ = dynamicBuffer_.get();
        DEBUG_MIDI("Allocation dynamique du buffer MIDI : %u octets",
                   bufferSize_ * sizeof(MidiMessage));
    } else {
        buffer_ = staticBuffer_;
        DEBUG_MIDI("Utilisation du buffer statique MIDI : %u octets",
                   STATIC_BUFFER_SIZE * sizeof(MidiMessage));
    }

    // Initialiser la table de hachage
    for (uint16_t i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable_[i] = INVALID_INDEX;
    }

    clear();
}

BufferedMidiOut::~BufferedMidiOut() {
    // Le destructeur de std::unique_ptr se charge de libérer la mémoire si nécessaire
    // buffer_ est également mis à nullptr automatiquement
}

void BufferedMidiOut::sendCc(MidiChannel ch, MidiCC cc, uint8_t value) {
    uint16_t index = findOrCreateMessage(MessageType::CC, ch, cc);

    // Seulement mettre à jour si la valeur a changé
    if (buffer_[index].value != value || buffer_[index].sent) {
        buffer_[index].value = value;

        // Marquer comme non envoyé uniquement si nécessaire
        if (buffer_[index].sent) {
            buffer_[index].sent = false;
            dirtyCount_++;
        }
    }
}

void BufferedMidiOut::sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t index = findOrCreateMessage(MessageType::NOTE_ON, ch, note);

    // Optimisation : vérifier si la valeur a changé
    if (buffer_[index].value != velocity || buffer_[index].sent) {
        buffer_[index].value = velocity;

        // Marquer comme non envoyé si c'est déjà envoyé
        if (buffer_[index].sent) {
            buffer_[index].sent = false;
            dirtyCount_++;
        }
    }
}

void BufferedMidiOut::sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t index = findOrCreateMessage(MessageType::NOTE_OFF, ch, note);

    // Optimisation : vérifier si la valeur a changé
    if (buffer_[index].value != velocity || buffer_[index].sent) {
        buffer_[index].value = velocity;

        // Marquer comme non envoyé si c'est déjà envoyé
        if (buffer_[index].sent) {
            buffer_[index].sent = false;
            dirtyCount_++;
        }
    }
}

void BufferedMidiOut::flush() {
    // Si aucun message à envoyer, sortir immédiatement
    if (dirtyCount_ == 0) return;

    // Optimisation : utiliser un tableau d'indices pour parcourir les messages une seule fois
    uint16_t messagesToSend[bufferSize_];
    uint16_t messageCount = 0;

    // Parcourir le buffer une seule fois pour collecter tous les messages à envoyer
    for (uint16_t i = 0; i < bufferSize_ && messageCount < dirtyCount_; i++) {
        if (!buffer_[i].sent) {
            messagesToSend[messageCount++] = i;
        }
    }

    // Envoyer tous les messages collectés
    for (uint16_t i = 0; i < messageCount; i++) {
        uint16_t idx = messagesToSend[i];

        switch (buffer_[idx].type) {
        case MessageType::CC:
            output_.sendCc(buffer_[idx].channel, buffer_[idx].control, buffer_[idx].value);
            break;
        case MessageType::NOTE_ON:
            output_.sendNoteOn(buffer_[idx].channel, buffer_[idx].control, buffer_[idx].value);
            break;
        case MessageType::NOTE_OFF:
            output_.sendNoteOff(buffer_[idx].channel, buffer_[idx].control, buffer_[idx].value);
            break;
        }

        buffer_[idx].sent = true;

        // Temporisation minimale entre les messages (sauf en mode haute priorité)
        if (!highPriority_ && i < messageCount - 1) {
            delayMicroseconds(100);
        }
    }

    // Tous les messages ont été envoyés
    dirtyCount_ = 0;

    // Optimiser le buffer si plus de la moitié des messages ont été envoyés
    if (nextIndex_ > bufferSize_ / 2) {
        optimizeBuffer();
    }
}

void BufferedMidiOut::clear() {
    // Vider la table de hachage
    for (uint16_t i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable_[i] = INVALID_INDEX;
    }

    // Réinitialiser le buffer
    for (uint16_t i = 0; i < bufferSize_; i++) {
        buffer_[i].type = MessageType::CC;
        buffer_[i].channel = 0;
        buffer_[i].control = 0;
        buffer_[i].value = 0;
        buffer_[i].sent = true;
        buffer_[i].hashNext = INVALID_INDEX;
    }

    nextIndex_ = 0;
    dirtyCount_ = 0;
}

uint16_t BufferedMidiOut::findOrCreateMessage(MessageType type, MidiChannel ch, uint8_t control) {
    // Calculer le hash pour ce message - utilise la fonction de hachage optimisée
    uint16_t hash = hashMessage(type, ch, control);

    // Chercher si ce message existe déjà dans la chaîne de hachage - accès direct
    uint16_t index = hashTable_[hash];
    uint16_t prevIndex = INVALID_INDEX;

    // Parcourir la chaîne de hachage avec une limite pour éviter les boucles infinies
    // La fonction de hachage améliorée devrait réduire les collisions
    uint16_t searchLimit = 4;  // Limité à 4 grâce à la meilleure fonction de hachage
    uint16_t searchCount = 0;

    while (index != INVALID_INDEX && searchCount < searchLimit) {
        if (buffer_[index].type == type && buffer_[index].channel == ch &&
            buffer_[index].control == control) {
            // Message trouvé - mise en tête de chaîne pour optimiser les accès futurs (seulement si
            // pas déjà en tête)
            if (prevIndex != INVALID_INDEX) {
                // Retirer de sa position actuelle
                buffer_[prevIndex].hashNext = buffer_[index].hashNext;
                // Mettre en tête de chaîne
                buffer_[index].hashNext = hashTable_[hash];
                hashTable_[hash] = index;
            }
            return index;
        }

        // Passer au message suivant dans la chaîne
        prevIndex = index;
        index = buffer_[index].hashNext;
        searchCount++;
    }

    // Message non trouvé - d'abord chercher un message déjà envoyé (marqué comme sent)
    // Optimisation : commencer la recherche depuis le dernier index utilisé pour réduire les
    // recherches
    uint16_t startIndex = nextIndex_;

    for (uint16_t i = 0; i < bufferSize_; i++) {
        uint16_t currentIndex = (startIndex + i) % bufferSize_;
        if (buffer_[currentIndex].sent) {
            // Trouvé un emplacement libre, initialiser les données du message
            buffer_[currentIndex].type = type;
            buffer_[currentIndex].channel = ch;
            buffer_[currentIndex].control = control;
            buffer_[currentIndex].sent = false;  // Marquer comme non envoyé
            dirtyCount_++;                       // Incrémenter le compteur de messages à envoyer

            // Insérer au début de la chaîne de hachage (position optimale)
            buffer_[currentIndex].hashNext = hashTable_[hash];
            hashTable_[hash] = currentIndex;

            // Mettre à jour nextIndex_ pour la prochaine recherche
            nextIndex_ = (currentIndex + 1) % bufferSize_;

            return currentIndex;
        }
    }

    // Tous les emplacements sont occupés, utiliser l'index suivant (stratégie circulaire)
    uint16_t currIndex = nextIndex_;
    nextIndex_ = (nextIndex_ + 1) % bufferSize_;

    // Mettre à jour la table de hachage : enlever l'ancien message s'il existe
    uint16_t oldHash = hashMessage(buffer_[currIndex].type,
                                   buffer_[currIndex].channel,
                                   buffer_[currIndex].control);

    // Cas spécial: si le message est au début de la chaîne
    if (hashTable_[oldHash] == currIndex) {
        hashTable_[oldHash] = buffer_[currIndex].hashNext;
    } else {
        // Chercher le message précédent dans la chaîne
        prevIndex = hashTable_[oldHash];
        searchCount = 0;

        while (prevIndex != INVALID_INDEX && buffer_[prevIndex].hashNext != currIndex &&
               searchCount < searchLimit) {
            prevIndex = buffer_[prevIndex].hashNext;
            searchCount++;
        }

        if (prevIndex != INVALID_INDEX && buffer_[prevIndex].hashNext == currIndex) {
            buffer_[prevIndex].hashNext = buffer_[currIndex].hashNext;
        }
    }

    // Initialiser le nouveau message
    buffer_[currIndex].type = type;
    buffer_[currIndex].channel = ch;
    buffer_[currIndex].control = control;
    buffer_[currIndex].sent = false;  // Marquer comme non envoyé
    dirtyCount_++;                    // Incrémenter le compteur de messages à envoyer

    // Insérer au début de la chaîne de hachage pour le nouveau hash
    buffer_[currIndex].hashNext = hashTable_[hash];
    hashTable_[hash] = currIndex;

    return currIndex;
}

void BufferedMidiOut::optimizeBuffer() {
    // Vider la table de hachage
    for (uint16_t i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable_[i] = INVALID_INDEX;
    }

    // Optimisation : utiliser une approche en place pour compacter le buffer
    uint16_t destIndex = 0;

    // Première passe : déplacer tous les messages actifs vers le début
    for (uint16_t srcIndex = 0; srcIndex < bufferSize_; srcIndex++) {
        if (!buffer_[srcIndex].sent) {
            if (srcIndex != destIndex) {
                // Déplacer le message vers sa nouvelle position
                buffer_[destIndex] = buffer_[srcIndex];
                // Marquer l'ancien emplacement comme disponible
                buffer_[srcIndex].sent = true;
            }

            // Mettre à jour la table de hachage
            uint16_t hash = hashMessage(buffer_[destIndex].type,
                                        buffer_[destIndex].channel,
                                        buffer_[destIndex].control);
            buffer_[destIndex].hashNext = hashTable_[hash];
            hashTable_[hash] = destIndex;

            // Incément l'index de destination
            destIndex++;
        }
    }

    // Marquer le reste du buffer comme disponible
    for (uint16_t i = destIndex; i < bufferSize_; i++) {
        buffer_[i].sent = true;
        buffer_[i].hashNext = INVALID_INDEX;
    }

    // Réinitialiser l'index pour la réutilisation des emplacements libres
    nextIndex_ = destIndex;  // Commencer juste après les messages actifs
}

uint16_t BufferedMidiOut::update(uint16_t maxMessages) {
    // N'envoyer les messages que s'il y en a à envoyer
    if (dirtyCount_ == 0) return 0;

    uint16_t sentCount = 0;
    uint16_t messagesToProcess =
        (maxMessages > 0) ? std::min(maxMessages, dirtyCount_) : dirtyCount_;

    // Tableau temporaire pour stocker les indices des messages à envoyer
    uint16_t messagesToSend[messagesToProcess];
    uint16_t messageCount = 0;

    // Ne parcourir le buffer qu'une seule fois pour collecter les messages à envoyer
    for (uint16_t i = 0; i < bufferSize_ && messageCount < messagesToProcess; i++) {
        if (!buffer_[i].sent) {
            messagesToSend[messageCount++] = i;
        }
    }

    // Envoyer les messages collectés
    for (uint16_t i = 0; i < messageCount; i++) {
        uint16_t idx = messagesToSend[i];

        switch (buffer_[idx].type) {
        case MessageType::CC:
            output_.sendCc(buffer_[idx].channel, buffer_[idx].control, buffer_[idx].value);
            break;
        case MessageType::NOTE_ON:
            output_.sendNoteOn(buffer_[idx].channel, buffer_[idx].control, buffer_[idx].value);
            break;
        case MessageType::NOTE_OFF:
            output_.sendNoteOff(buffer_[idx].channel, buffer_[idx].control, buffer_[idx].value);
            break;
        }

        buffer_[idx].sent = true;
        sentCount++;
        dirtyCount_--;

        // Temporisation minimale entre les messages pour éviter de saturer le port MIDI
        // (sauf si mode haute priorité)
        if (!highPriority_ && i < messageCount - 1) {
            delayMicroseconds(200);
        }
    }

    // Optimisation: si nous avons envoyé tous les messages et qu'il y a beaucoup de messages
    // envoyés, compacter le buffer
    if (dirtyCount_ == 0 && nextIndex_ > bufferSize_ / 2) {
        optimizeBuffer();
    }

    return sentCount;
}
