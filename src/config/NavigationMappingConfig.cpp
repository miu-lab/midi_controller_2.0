#include "config/NavigationMappingConfig.hpp"

// Liste des contrôles dédiés à la navigation
// Ajoutez ici les IDs des encodeurs et boutons qui ne doivent pas envoyer de MIDI
// mais plutôt être utilisés pour naviguer dans l'interface utilisateur
const std::vector<NavigationControl> navigationControls = {
    // Exemple: l'encodeur ID 1 est dédié à la navigation
    {79},

    // Exemple: le bouton ID 5 est dédié à la navigation
    {51},
    {52},

    // Ajoutez d'autres contrôles de navigation ici...
};

// Nombre de contrôles de navigation (calculé automatiquement)
const std::size_t navigationControlCount = navigationControls.size();
