#pragma once

#include <Arduino.h>

/**
 * Classe pour gérer un tampon circulaire pour le port série
 * Permet d'éviter la saturation du moniteur série en ne gardant
 * qu'un nombre limité de lignes en mémoire
 */
class SerialBuffer {
public:
    // Initialise le tampon avec une taille maximale
    static void init(size_t maxLines = 100);
    
    // Ajoute une ligne au tampon (remplace la plus ancienne si plein)
    static void println(const String& line);
    
    // Affiche tout le contenu du tampon sur le port série
    static void flush();
    
    // Efface le tampon
    static void clear();

private:
    // Constructeur privé (singleton)
    SerialBuffer(size_t maxLines);
    
    // Destructeur
    ~SerialBuffer();
    
    // Ajoute une ligne au tampon
    void addLine(const String& line);
    
    // Affiche tout le contenu du tampon
    void dumpBuffer();
    
    // Efface le tampon
    void clearBuffer();
    
    static SerialBuffer* _instance;  // Instance singleton
    String* _buffer;                 // Tampon de lignes
    size_t _maxLines;                // Taille maximale du tampon
    size_t _currentIndex;            // Index courant dans le tampon
    bool _isFull;                    // Indique si le tampon est plein
};

