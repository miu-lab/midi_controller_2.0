#include "core/use_cases/ProcessEncoders.hpp"
#include "core/domain/events/InputEvent.hpp"
#include "core/domain/EventBus.hpp"

ProcessEncoders::ProcessEncoders(const std::vector<IEncoder *> &encoders)
    : encoders_(encoders), lastPressed_(encoders.size(), false) {}

void ProcessEncoders::update()
{
    for (size_t i = 0; i < encoders_.size(); ++i)
    {
        IEncoder *encoder = encoders_[i];

        // Lire le delta pour mettre à jour la position interne
        encoder->readDelta();

        // Obtenir la position absolue actuelle
        int32_t absPos = encoder->getAbsolutePosition();
        
        // Nous ne publions plus les événements en fonction du delta
        // mais uniquement en fonction de la position absolue
        static std::vector<int32_t> lastAbsPos(encoders_.size(), 0);
        
        // N'envoyer un événement que si la position absolue a changé
        if (absPos != lastAbsPos[i])
        {
            // Déboguer uniquement les changements de position absolue
#if defined(DEBUG) && defined(DEBUG_RAW_CONTROLS) && (DEBUG_RAW_CONTROLS >= 1)
            Serial.print("ENC_ABS_CHANGED ");
            Serial.print(encoder->getId());
            Serial.print(" old:");
            Serial.print(lastAbsPos[i]);
            Serial.print(" new:");
            Serial.println(absPos);
#endif
            // Mettre à jour la dernière position absolue connue
            lastAbsPos[i] = absPos;
            
            // Publier l'événement avec uniquement la position absolue
            EventBus<EncoderTurnedEvent>::publish({
                .id = encoder->getId(),
                .absolutePosition = absPos
            });
        }

        bool pressed = encoder->isPressed();
        if (pressed != lastPressed_[i])
        {
            lastPressed_[i] = pressed;
            EventBus<EncoderButtonEvent>::publish({encoder->getId(), pressed});
        }
    }
}
