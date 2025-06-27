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
            DEBUG_LOG(DEBUG_LEVEL_INFO, "%.77s...", line.c_str());
        } else {
            DEBUG_LOG(DEBUG_LEVEL_INFO, "%s", line.c_str());
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
#ifdef DEBUG
    size_t startIndex = _isFull ? _currentIndex : 0;
    size_t count = _isFull ? _maxLines : _currentIndex;
    
    for (size_t i = 0; i < count; i++) {
        size_t index = (startIndex + i) % _maxLines;
        DEBUG_LOG(DEBUG_LEVEL_INFO, "%s", _buffer[index].c_str());
    }
#endif
}

// Effacement du tampon
void SerialBuffer::clearBuffer() {
    for (size_t i = 0; i < _maxLines; i++) {
        _buffer[i] = "";
    }
    _currentIndex = 0;
    _isFull = false;
}