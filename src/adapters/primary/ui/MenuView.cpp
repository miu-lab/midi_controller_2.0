// Ajouter l'inclusion du gestionnaire de vue par défaut
#include "MenuView.hpp"
#include "ViewManager.hpp"
#include "core/utils/AppStrings.hpp"
#include "core/utils/FlashStrings.hpp"

MenuView::MenuView(std::shared_ptr<DisplayPort> display)
    : View(display), selectedIndex_(0), scrollOffset_(0), maxVisibleItems_(4) {}

bool MenuView::init() {
    // Initialiser les éléments de menu simplifiés
    
    // Menu simplifié avec seulement les options essentielles
    addItem("Home", 0);
    addItem("Parameters", 1);
    addItem("Settings", 2);
    
    // Utiliser la chaîne partagée MENU
    char buffer[16];
    FlashStrings::copy(buffer, sizeof(buffer), MENU);
    title_ = buffer;
    return true;
}

void MenuView::update() {
    // Dans une implémentation plus complexe, cette méthode pourrait mettre
    // à jour les états des éléments de menu, par exemple en désactivant
    // certaines options selon l'état du système
}

void MenuView::render() {
    if (!active_) return;
    
    // Dessiner le cadre du menu
    display_->drawRect(0, 0, 128, 64, false);
    
    // Dessiner le titre du menu
    display_->drawText(4, 1, title_.c_str());
    
    // Ligne horizontale sous le titre
    display_->drawLine(0, 10, 128, 10);
    
    // Calculer le nombre d'éléments à afficher
    int numItems = items_.size();
    int startY = 13; // Position Y initiale pour les éléments de menu
    int itemHeight = 12; // Hauteur de chaque élément
    
    // Afficher les éléments visibles
    for (int i = 0; i < maxVisibleItems_ && (i + scrollOffset_) < numItems; i++) {
        int itemIndex = i + scrollOffset_;
        int y = startY + (i * itemHeight);
        
        // Mettre en évidence l'élément sélectionné
        if (itemIndex == selectedIndex_) {
            display_->drawRect(2, y - 1, 124, itemHeight, true); // Rectangle rempli
            // Inverser la couleur du texte pour l'élément sélectionné
        }
        
        // Afficher le label de l'élément (tronqué si nécessaire)
        String displayText = items_[itemIndex].label;
        if (displayText.length() > 20) { // Limiter la longueur du texte
            displayText = displayText.substring(0, 17) + "...";
        }
        
        // Afficher le texte de l'élément
        display_->drawText(4, y, displayText.c_str());
    }
    
    // Indicateurs de défilement si nécessaire
    if (scrollOffset_ > 0) {
        // Flèche vers le haut
        display_->drawLine(124, 13, 127, 16);
        display_->drawLine(124, 13, 121, 16);
    }
    
    if ((scrollOffset_ + maxVisibleItems_) < numItems) {
        // Flèche vers le bas
        display_->drawLine(124, 61, 127, 58);
        display_->drawLine(124, 61, 121, 58);
    }
}

bool MenuView::handleEvent(uint8_t eventType, int32_t data) {
    // Traitement des événements d'entrée
    // Cette implémentation simplifiée ne gère pas les événements directement
    return false;
}

bool MenuView::isActive() const {
    return active_;
}

void MenuView::setActive(bool active) {
    active_ = active;
}

void MenuView::addItem(const String& label, int id, bool enabled) {
    items_.push_back(MenuItem(label, id, enabled));
}

void MenuView::selectNext() {
    if (items_.empty()) return;
    
    selectedIndex_ = (selectedIndex_ + 1) % items_.size();
    updateScrollOffsetIfNeeded();
}

void MenuView::selectPrevious() {
    if (items_.empty()) return;
    
    selectedIndex_ = (selectedIndex_ - 1 + items_.size()) % items_.size();
    updateScrollOffsetIfNeeded();
}

void MenuView::selectItem(int index) {
    if (index >= 0 && index < static_cast<int>(items_.size())) {
        selectedIndex_ = index;
        updateScrollOffsetIfNeeded();
        
        // Notifier le ViewManager si présent
        if (viewManager_) {
            // Convertir l'ID d'item de menu en action correspondante
            switch (items_[selectedIndex_].id) {
                case 0: // Home/Main Screen
                    viewManager_->showHome();
                    break;
                case 1: // Retour aux paramètres
                    viewManager_->showParameterFocus(7, 1, 100, "VOLUME"); // Exemple
                    break;
                case 2: // Configuration (pour l'instant rien)
                    // TODO: implémenter vue de configuration
                    break;
                default:
                    // Retourner à la vue par défaut
                    viewManager_->showHome();
                    break;
            }
        }
    }
}

int MenuView::getCurrentIndex() const {
    return selectedIndex_;
}

int MenuView::getCurrentItemId() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(items_.size())) {
        return items_[selectedIndex_].id;
    }
    return -1;
}

void MenuView::setTitle(const String& title) {
    title_ = title;
}

void MenuView::updateScrollOffsetIfNeeded() {
    // Faire défiler vers le haut si nécessaire
    if (selectedIndex_ < scrollOffset_) {
        scrollOffset_ = selectedIndex_;
    }
    
    // Faire défiler vers le bas si nécessaire
    if (selectedIndex_ >= (scrollOffset_ + maxVisibleItems_)) {
        scrollOffset_ = selectedIndex_ - maxVisibleItems_ + 1;
    }
}
