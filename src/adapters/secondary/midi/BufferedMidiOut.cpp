#include "adapters/secondary/midi/BufferedMidiOut.hpp"

BufferedMidiOut::BufferedMidiOut(IMidiOut& output, uint16_t bufferSize)
    : output_(output),
      bufferSize_(bufferSize > 0 ? bufferSize : DEFAULT_BUFFER_SIZE),
      nextIndex_(0),
      dirtyCount_(0) {
    buffer_ = new MidiMessage[bufferSize_];
    
    // Initialiser la table de hachage
    for (uint16_t i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable_[i] = INVALID_INDEX;
    }
    
    clear();
}

void BufferedMidiOut::sendCc(MidiChannel ch, MidiCC cc, uint8_t value) {
    uint16_t index = findOrCreateMessage(MessageType::CC, ch, cc);
    buffer_[index].value = value;
    
    // Marquer comme non envoyé si la valeur a changé ou si c'est déjà envoyé
    if (buffer_[index].sent) {
        buffer_[index].sent = false;
        dirtyCount_++;
    }
}

void BufferedMidiOut::sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t index = findOrCreateMessage(MessageType::NOTE_ON, ch, note);
    buffer_[index].value = velocity;
    
    // Marquer comme non envoyé si c'est déjà envoyé
    if (buffer_[index].sent) {
        buffer_[index].sent = false;
        dirtyCount_++;
    }
}

void BufferedMidiOut::sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t index = findOrCreateMessage(MessageType::NOTE_OFF, ch, note);
    buffer_[index].value = velocity;
    
    // Marquer comme non envoyé si c'est déjà envoyé
    if (buffer_[index].sent) {
        buffer_[index].sent = false;
        dirtyCount_++;
    }
}

void BufferedMidiOut::flush() {
    // Si aucun message à envoyer, sortir immédiatement
    if (dirtyCount_ == 0) return;
    
    uint16_t sentCount = 0;
    
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (!buffer_[i].sent) {
            switch (buffer_[i].type) {
            case MessageType::CC:
                output_.sendCc(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                break;
            case MessageType::NOTE_ON:
                output_.sendNoteOn(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                break;
            case MessageType::NOTE_OFF:
                output_.sendNoteOff(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                break;
            }
            buffer_[i].sent = true;
            sentCount++;
            
            // Sortir tôt si tous les messages sont envoyés
            if (sentCount >= dirtyCount_) break;
        }
    }
    
    // Mettre à jour le compteur de messages en attente
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
    uint16_t searchLimit = 8; // Limiter la profondeur de recherche pour les performances
    uint16_t searchCount = 0;
    
    while (index != INVALID_INDEX && searchCount < searchLimit) {
        if (buffer_[index].type == type && 
            buffer_[index].channel == ch && 
            buffer_[index].control == control) {
            // Message trouvé - le déplacer en tête de chaîne pour optimiser les accès futurs
            if (prevIndex != INVALID_INDEX) {
                // Retirer de sa position actuelle
                buffer_[prevIndex].hashNext = buffer_[index].hashNext;
                // Mettre en tête de chaîne
                buffer_[index].hashNext = hashTable_[hash];
                hashTable_[hash] = index;
            }
            // Note: nous ne marquons pas le message comme non-envoyé ici, cela est fait dans sendCc, etc.
            return index;
        }
        
        // Passer au message suivant dans la chaîne
        prevIndex = index;
        index = buffer_[index].hashNext;
        searchCount++;
    }
    
    // Message non trouvé - d'abord chercher un message déjà envoyé (marqué comme sent)
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (buffer_[i].sent) {
            // Trouvé un emplacement libre, initialiser les données du message
            buffer_[i].type = type;
            buffer_[i].channel = ch;
            buffer_[i].control = control;
            buffer_[i].sent = false;  // Marquer comme non envoyé
            dirtyCount_++;            // Incrémenter le compteur de messages à envoyer
            
            // Insérer au début de la chaîne de hachage (position optimale)
            buffer_[i].hashNext = hashTable_[hash];
            hashTable_[hash] = i;
            
            return i;
        }
    }
    
    // Tous les emplacements sont occupés, utiliser l'index suivant (stratégie circulaire)
    uint16_t currIndex = nextIndex_;
    nextIndex_ = (nextIndex_ + 1) % bufferSize_;
    
    // Mettre à jour la table de hachage : enlever l'ancien message s'il existe
    uint16_t oldHash = hashMessage(buffer_[currIndex].type, buffer_[currIndex].channel, buffer_[currIndex].control);
    
    // Cas spécial: si le message est au début de la chaîne
    if (hashTable_[oldHash] == currIndex) {
        hashTable_[oldHash] = buffer_[currIndex].hashNext;
    } else {
        // Sinon, trouver le message précédent dans la chaîne avec une limite de profondeur
        prevIndex = hashTable_[oldHash];
        searchCount = 0;
        
        while (prevIndex != INVALID_INDEX && buffer_[prevIndex].hashNext != currIndex && searchCount < searchLimit) {
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
    
    uint16_t activeCount = 0;
    uint16_t activeIndices[bufferSize_]; // Stockage temporaire des messages actifs
    
    // Première passe : identifier les messages non envoyés
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (!buffer_[i].sent) {
            activeIndices[activeCount++] = i;
        }
    }
    
    // Deuxième passe : compacter le buffer en déplaçant les messages actifs au début
    for (uint16_t i = 0; i < activeCount; i++) {
        if (activeIndices[i] != i) { // Si le message n'est pas déjà à sa place optimale
            // Copier le message actif à sa nouvelle position
            buffer_[i] = buffer_[activeIndices[i]];
            // Marquer l'ancien emplacement comme envoyé/inutilisé
            buffer_[activeIndices[i]].sent = true;
        }
        
        // Calculer le hash et mettre à jour la table de hachage
        uint16_t hash = hashMessage(buffer_[i].type, buffer_[i].channel, buffer_[i].control);
        buffer_[i].hashNext = hashTable_[hash];
        hashTable_[hash] = i;
    }
    
    // Marquer le reste du buffer comme disponible
    for (uint16_t i = activeCount; i < bufferSize_; i++) {
        buffer_[i].sent = true;
        buffer_[i].hashNext = INVALID_INDEX;
    }
    
    // Réinitialiser l'index pour la réutilisation des emplacements libres
    nextIndex_ = activeCount; // Commencer juste après les messages actifs
}

void BufferedMidiOut::update() {
    // N'envoyer les messages que s'il y en a à envoyer
    if (dirtyCount_ > 0) {
        // Optimisation : limiter le nombre de messages envoyés par cycle pour réduire la latence
        // et permettre une meilleure distribution du temps de traitement
        const uint16_t MAX_MESSAGES_PER_UPDATE = 8;
        uint16_t sentCount = 0;
        
        for (uint16_t i = 0; i < bufferSize_ && sentCount < MAX_MESSAGES_PER_UPDATE; i++) {
            if (!buffer_[i].sent) {
                switch (buffer_[i].type) {
                    case MessageType::CC:
                        output_.sendCc(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                        break;
                    case MessageType::NOTE_ON:
                        output_.sendNoteOn(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                        break;
                    case MessageType::NOTE_OFF:
                        output_.sendNoteOff(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                        break;
                }
                buffer_[i].sent = true;
                sentCount++;
                dirtyCount_--;
            }
        }
        
        // Optimiser le buffer si plus de la moitié des messages ont été envoyés
        // et qu'il ne reste plus de messages en attente
        if (dirtyCount_ == 0 && nextIndex_ > bufferSize_ / 2) {
            optimizeBuffer();
        }
    }
}
