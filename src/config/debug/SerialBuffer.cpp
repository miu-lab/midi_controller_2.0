#include "SerialBuffer.hpp"
#include "DebugMacros.hpp"

// Initialisation du pointeur d'instance statique
SerialBuffer* SerialBuffer::_instance = nullptr;

// Initialisation du tampon
void SerialBuffer::init(size_t maxLines) {
    if (_instance == nullptr) {
        _instance = new SerialBuffer(maxLines);
    }
}

// Affichage d'une ligne
void SerialBuffer::println(const String& line) {
#ifdef DEBUG
    if (_instance) {
        _instance->addLine(line);
    } else {
        // Si pas encore initialisé, affiche directement sur le port série
        // et s'assure qu'il y a un saut de ligne
        if (line.length() > 80) {
            // TODO DEBUG MSG TO IMPLEMENT
        } else {
            // TODO DEBUG MSG TO IMPLEMENT
        }
    }
#endif
}

// Affichage de tout le contenu
void SerialBuffer::flush() {
#ifdef DEBUG
    if (_instance) {
        _instance->dumpBuffer();
    }
#endif
}

// Effacement du tampon
void SerialBuffer::clear() {
    if (_instance) {
        _instance->clearBuffer();
    }
}

// Constructeur
SerialBuffer::SerialBuffer(size_t maxLines) 
    : _maxLines(maxLines), _currentIndex(0), _isFull(false) {
    _buffer = new String[maxLines];
}

// Destructeur
SerialBuffer::~SerialBuffer() {
    delete[] _buffer;
}

// Ajout d'une ligne au tampon
void SerialBuffer::addLine(const String& line) {
    _buffer[_currentIndex] = line;
    _currentIndex = (_currentIndex + 1) % _maxLines;
    
    if (!_isFull && _currentIndex == 0) {
        _isFull = true;
    }
}

// Affichage de tout le contenu du tampon
void SerialBuffer::dumpBuffer() {
    // TODO  : Affiche le contenu du tampon sur le port série
}

// Effacement du tampon
void SerialBuffer::clearBuffer() {
    for (size_t i = 0; i < _maxLines; i++) {
        _buffer[i] = "";
    }
    _currentIndex = 0;
    _isFull = false;
}