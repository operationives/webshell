#include "controleur.h"

Controleur::Controleur(MainWindow *mw){
    this->mw = mw;
}

void Controleur::evaluateJavaScript(QString code){
    this->mw->evaluateJavaScript(code);
}
