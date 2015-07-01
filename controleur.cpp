#include "controleur.h"

/**
 * @brief Controleur::Controleur Constructeur de l'objet permettant l'accès à la primitive evaluateJavaScript de la fenêtre principale
 * @param mw Pointeur vers la fenêtre principale
 */
Controleur::Controleur(MainWindow *mw){
    this->mw = mw;
}

/**
 * @brief Controleur::evaluateJavaScript Méthode d'appel JavaScript
 * @param code Code à évaluer avec evaluateJavaScript
 */
void Controleur::evaluateJavaScript(QString code){
    this->mw->evaluateJavaScript(code);
}
