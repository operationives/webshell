#ifndef CONTROLEUR_H
#define CONTROLEUR_H

#include "mainwindow.h"

class Controleur : public QObject{

    Q_OBJECT

public:
    Controleur(MainWindow *mw);
    void evaluateJavaScript(QString code);
private:
    MainWindow *mw;
};


#endif // CONTROLEUR_H

