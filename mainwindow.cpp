#include "mainwindow.h"
#include "updateoperations.h"
#include "global.h"

/**
 * @brief MainWindow::MainWindow Initialisation de la fenêtre principale
 * @param url   Url avec laquelle le service est initialisé
 */
MainWindow::MainWindow(const QUrl& url){
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    //Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    view = new QWebView(this);
    //On permet l'accès aux méthodes dans UdpdateOperations par les appels javascript
    view->page()->mainFrame()->addToJavaScriptWindowObject("updateOperations", new UdpdateOperations);
    view->load(url);
    //Il faudra voir quelle taille de fenêtre minimale est autorisée
//    view->setMinimumSize(int width,int height);

    //On enlève les barres de défilement inutiles dans le cadre de la webshell
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    //On crée le controleur accessible depuis toutes les classes permettant d'accéder à la méthode evaluatejavascript
    ctrl = new Controleur(this);
}

/**
 * @brief MainWindow::~MainWindow Destructeur de MainWindow
 */
MainWindow::~MainWindow(){
    delete view;
}

/**
 * @brief MainWindow::evaluateJavaScript Méthode d'appel JavaScript
 * @param code  Code à évaluer avec evaluateJavaScript
 */
void MainWindow::evaluateJavaScript(QString code){
    view->page()->mainFrame()->evaluateJavaScript(code);
}
