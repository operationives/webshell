#include "mainwindow.h"
#include "wnavigator.h"
#include "global.h"

/**
 * @brief MainWindow::MainWindow Initialisation de la fenêtre principale
 * @param url   Url avec laquelle le service est initialisé
 */
MainWindow::MainWindow(const QUrl& url){
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    //Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    view = new MyWebView(this);
    connect(view,SIGNAL(ChangeIcon(QIcon)),this,SLOT(ChangeIcon(QIcon)));
    view->load(url);
    //Il faudra voir quelle taille de fenêtre minimale est autorisée
//    view->setMinimumSize(int width,int height);

    //On enlève les barres de défilement inutiles dans le cadre de la webshell
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    //On indique qu'on utilise un menu personnalisé
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    //On définit les actions du menu de trayIcon
    QAction *quitAction = new QAction("Quitter", this);
    connect (quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction (quitAction);
    QIcon icon(QApplication::applicationDirPath()+"/djanah.png");
    trayIcon = new QSystemTrayIcon(icon,this);
    trayIcon->setContextMenu (trayIconMenu);
    trayIcon->show();

    //Initialisation de l'inspecteur de la page
    i = new QWebInspector();
    i->setPage(view->page());

    stayOpen = true;
    config = new ConfigManager();
    params = new Parametres(config->getCloseButtonBehaviour(),config->getDeveloperToolsMode());
    if(config->getScreenMode())
        this->showFullScreen();
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, config->getDeveloperToolsMode());

//    connect(params,SIGNAL(screenMode(bool)),this,SLOT(changeScreenMode(bool)));
    connect(params,SIGNAL(closeButtonMode(bool)),this,SLOT(changeCloseButtonMode(bool)));
    connect(params,SIGNAL(toolsMode(bool)),this,SLOT(changeToolsMode(bool)));

    //On ajoute les suppléments windows
    WinAddon *waddon = new WinAddon();
    if(waddon->isWidgetType()){}

    //On crée le controleur accessible depuis toutes les classes permettant d'accéder à la méthode evaluatejavascript
    ctrl = new Controleur(this);
}

/**
 * @brief MainWindow::~MainWindow Destructeur de MainWindow
 */
MainWindow::~MainWindow(){
    delete view;
    delete trayIcon;
    delete i;
    delete params;
}

/**
 * @brief MainWindow::evaluateJavaScript Méthode d'appel JavaScript
 * @param code  Code à évaluer avec evaluateJavaScript
 */
void MainWindow::evaluateJavaScript(QString code){
    view->page()->mainFrame()->evaluateJavaScript(code);
}

/**
 * @brief MainWindow::showContextMenu Affichage du menu lors d'un clic droit sur la fenêtre principale
 * @param pos   Position du pointeur lors du clic
 */
void MainWindow::showContextMenu(const QPoint &pos){

    QMenu myMenu;
    myMenu.addAction("Paramètres");
    if(QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled)){
        myMenu.addAction("Inspect");
    }
    if(!this->isFullScreen()){
        myMenu.addAction("Plein écran");
    }
    else{
        myMenu.addAction("Fenêtré");
    }
    myMenu.addAction("Fermer");
    myMenu.addAction("Test");

    QPoint globalPos = this->mapToGlobal(pos);

    QAction* selectedItem = myMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    if (selectedItem->text().compare("Paramètres",Qt::CaseSensitive) == 0){
        params->show();
    }

    if (selectedItem->text().compare("Inspect",Qt::CaseSensitive) == 0){
        i->show();
    }

    if (selectedItem->text().compare("Plein écran",Qt::CaseSensitive) == 0){
        changeScreenMode(true);
    }

    if (selectedItem->text().compare("Fenêtré",Qt::CaseSensitive) == 0){
        changeScreenMode(false);
    }
    if (selectedItem->text().compare("Fermer",Qt::CaseSensitive) == 0){
        this->quit();
    }
    if (selectedItem->text().compare("Test",Qt::CaseSensitive) == 0){
       //view->load(QUrl("https://www.google.fr"));
    }
}

/**
 * @brief MainWindow::changeScreenMode  Change l'affichage de la fenêtre
 * @param fullscreen    Vrai si mode plein écran, mode fenêtré sinon
 */
void MainWindow::changeScreenMode(bool fullscreen){
    if(fullscreen){
        this->showFullScreen();
    }
    else{
        this->showNormal();
    }
    config->setScreenMode(fullscreen);
}

/**
 * @brief MainWindow::changeCloseButtonMode Change l'effet du bouton de fermeture
 * @param minimization  Vrai si le bouton est associé à la minimisation, association à la fermeture sinon
 */
void MainWindow::changeCloseButtonMode(bool minimization){
        config->setCloseButtonBehaviour(minimization);
}

/**
 * @brief MainWindow::changeToolsMode   Change l'accès aux outils développeur
 * @param toolsActivated    Vrai si les outils sont activés, désactivés sinon
 */
void MainWindow::changeToolsMode(bool toolsActivated){
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, toolsActivated);
    config->setDeveloperToolsMode(toolsActivated);
}

/**
 * @brief MainWindow::keyPressEvent Si la touche tapée est ESC, on quitte le mode plein écran, sinon on traite l'événement normalement
 * @param event Evénement de touche tapée
 */
void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        if(this->isFullScreen())
            this->changeScreenMode(false);
    }
    else{
        QMainWindow::keyPressEvent(event); // call the default implementation
    }
}

/**
 * @brief MainWindow::closeEvent    Indique si la fenêtre doit être femrée ou minimisée
 * @param event Evénement de fermeture
 */
void MainWindow::closeEvent (QCloseEvent *event){
    if (config->getCloseButtonBehaviour() && stayOpen) {
        event->ignore();
        this->setWindowState(Qt::WindowMinimized);
    } else {
        delete this;
        event->accept();
    }
}

/**
 * @brief MainWindow::quit Quitte l'application
 */
void MainWindow::quit (){
    stayOpen = false;
    this->close();
}

void MainWindow::ChangeIcon(QIcon icon)
{
    this->trayIcon->setIcon(icon);
}
