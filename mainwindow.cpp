#include "mainwindow.h"
#include "wnavigator.h"
#include "global.h"

/**
 * @brief MainWindow::MainWindow Initialisation de la fenêtre principale
 * @param url   Url avec laquelle le service est initialisé
 */
MainWindow::MainWindow()
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    //Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    //On définit les actions du menu de trayIcon
    QAction *quitAction = new QAction("Quitter", this);
    connect (quitAction, SIGNAL(triggered()), this, SLOT(quit()));
    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction (quitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu (trayIconMenu);
    trayIcon->show();

    //Ajout du menu dans la barre de titre
    QMenu *fileMenu = menuBar()->addMenu(tr("&Fichier"));
    fileMenu->addAction(quitAction);
    QAction *clearCookies = new QAction("&Effacer les cookies", this);
    fileMenu->addAction(clearCookies);

    view = new MyWebView(this);
    connect(view,SIGNAL(changeIcon(QIcon)),this,SLOT(changeIcon(QIcon)));
    connect(view,SIGNAL(changeTitle(QString)),this,SLOT(setWindowTitle(QString)));
    connect(view,SIGNAL(close()),this,SLOT(quit()));
    connect (clearCookies, SIGNAL(triggered()), view->m_cookieJar, SLOT(clear()));
    view->load(QUrl(config->GetLaunchUrl()));
    //On met en place la taille minimale
    this->setMinimumSize(1000,800);

    //On enlève les barres de défilement inutiles dans le cadre de la webshell
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    //On indique qu'on utilise un menu personnalisé
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    //Initialisation de l'inspecteur de la page
    i = new QWebInspector();
    i->setPage(view->page());

    stayOpen = true;
    if(config->GetScreenMode())
        this->showFullScreen();
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, config->GetDeveloperToolsMode());

    connect(config,SIGNAL(toolsMode(bool)),this,SLOT(changeToolsMode(bool)));

    infos = new Informations();

    //On ajoute les suppléments windows
    WinAddon *waddon = new WinAddon();
    if(waddon->isWidgetType()){}
}

/**
 * @brief MainWindow::~MainWindow Destructeur de MainWindow
 */
MainWindow::~MainWindow()
{
    delete view;
    delete trayIcon;
    delete i;
    delete infos;
}

/**
 * @brief MainWindow::showContextMenu Affichage du menu lors d'un clic droit sur la fenêtre principale
 * @param pos   Position du pointeur lors du clic
 */
void MainWindow::showContextMenu(const QPoint &pos)
{

    QMenu myMenu;
    if(QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
    {
        myMenu.addAction("Inspect");
    }
    if(!this->isFullScreen())
    {
        myMenu.addAction("Plein écran");
    }
    else
    {
        myMenu.addAction("Fenêtré");
    }
    myMenu.addAction("Fermer");
    myMenu.addAction("Reload");
    myMenu.addAction("Informations");

    QPoint globalPos = this->mapToGlobal(pos);

    QAction* selectedItem = myMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    if (selectedItem->text()=="Inspect")
    {
        i->show();
    }

    if (selectedItem->text()=="Plein écran")
    {
        changeScreenMode(true);
    }

    if (selectedItem->text()=="Fenêtré")
    {
        changeScreenMode(false);
    }
    if (selectedItem->text()=="Fermer")
    {
        this->quit();
    }
    if (selectedItem->text()=="Reload")
    {
       view->reload();
    }
    if (selectedItem->text()=="Informations")
    {
        this->DisplayInfos();
    }
}

/**
 * @brief MainWindow::changeScreenMode  Change l'affichage de la fenêtre
 * @param fullscreen    Vrai si mode plein écran, mode fenêtré sinon
 */
void MainWindow::changeScreenMode(bool fullscreen)
{
    if(fullscreen)
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
    config->SetScreenMode(fullscreen);
}

/**
 * @brief MainWindow::changeToolsMode   Change l'accès aux outils développeur sur la page et dans le fichier xml
 * @param toolsActivated    Vrai si les outils sont activés, désactivés sinon
 */
void MainWindow::changeToolsMode(bool toolsActivated)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, toolsActivated);
}

/**
 * @brief MainWindow::keyPressEvent Si la touche tapée est ESC, on quitte le mode plein écran, sinon on traite l'événement normalement
 * @param event Evénement de touche tapée
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if(this->isFullScreen())
            this->changeScreenMode(false);
    }
    else
    {
        QMainWindow::keyPressEvent(event); // call the default implementation
    }
}

/**
 * @brief MainWindow::closeEvent    Indique si la fenêtre doit être femrée ou minimisée
 * @param event Evénement de fermeture
 */
void MainWindow::closeEvent (QCloseEvent *event)
{
    if (config->GetCloseButtonBehaviour() && stayOpen)
    {
        event->ignore();
        this->setWindowState(Qt::WindowMinimized);
    }
    else
    {
        delete this;
        event->accept();
    }
}

/**
 * @brief MainWindow::quit Quitte l'application
 */
void MainWindow::quit ()
{
    stayOpen = false;
    view->DispatchJsEvent("Exit","window");
    this->close();
}

/**
 * @brief MainWindow::changeIcon Change l'icône dans la barre de notification
 * @param icon  Icône à placer
 */
void MainWindow::changeIcon(QIcon icon)
{
    this->trayIcon->setIcon(icon);
}

/**
 * @brief MainWindow::DisplayInfos Met à jour les données de la page d'information et l'affiche
 */
void MainWindow::DisplayInfos()
{
    infos->UpdateValues();
    infos->show();
}
