#include "mainwindow.h"
#include "Webview/mynetworkaccessmanager.h"
#include "Webview/wnavigator.h"
#include "Outils/configmanager.h"
#ifdef Q_OS_WIN
#include "Windows/mailsender.h"
#endif

/**
 * @brief Initialisation de la fenêtre principale avec la page de chargement
 * @param iconPath	Chemin vers l'icône des fenêtres de l'application
 * @param parent	Widget parent, placé à NULL dans son utilisation actuelle
 */
MainWindow::MainWindow(const QString &iconPath, QWidget *parent)
	:QMainWindow(parent)
{
	QNetworkProxyFactory::setUseSystemConfiguration(true);

	ConfigManager &config = ConfigManager::Instance();
	connect(&config,SIGNAL(toolsMode(bool)),this,SLOT(changeToolsMode(bool)));
	connect(&config,SIGNAL(minSize(int,int)),this,SLOT(changeMinSize(int,int)));
    connect(&config,SIGNAL(newLanguage(QString)),this,SLOT(changeActionNames(QString)));

	stayOpen = true;
	infos = new Informations();

	//On initialise les actions des différents menus
	clearAllAction = new QAction(this);
	quitAction = new QAction(this);
	inspectAction = new QAction(this);
	fullscreenAction = new QAction(this);
	normalscreenAction = new QAction(this);
	reloadAction = new QAction(this);
	infoAction = new QAction(this);
#ifdef Q_OS_WIN
	sendlogAction = new QAction(this);
#endif

	//Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
	QWebSettings::globalSettings()->enablePersistentStorage(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+config.GetAppName());
	QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, config.GetDeveloperToolsMode());

	connect (quitAction, SIGNAL(triggered()), this, SLOT(quit()));
	QMenu *trayIconMenu = new QMenu(this);
	trayIconMenu->addAction (quitAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu (trayIconMenu);
	trayIcon->show();

	//Ajout du menu dans la barre de titre
	fileMenu = menuBar()->addMenu(tr("&Fichier"));
	fileMenu->addAction(quitAction);
	fileMenu->addAction(clearAllAction);
	menuBar()->setVisible(config.GetMenuBarPresent());
	connect(&config,SIGNAL(menuBarPresence(bool)),menuBar(),SLOT(setVisible(bool)));

	changeActionNames(config.GetLanguage());

	view = new MyWebView(this);

	//On indique qu'on utilise un menu personnalisé
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(view,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

	//Initialisation de l'inspecteur de la page
	inspector = new QWebInspector();
	inspector->setPage(view->page());

	windowIconSpecified = !iconPath.isNull();
	if(windowIconSpecified)
    {
        QIcon windowIcon(iconPath);
		this->setWindowIcon(windowIcon);
		infos->setWindowIcon(windowIcon);
        inspector->setWindowIcon(windowIcon);

        this->trayIcon->setIcon(windowIcon);
	}



    //connect(view,SIGNAL(changeIcon(QIcon)),this,SLOT(changeIcon(QIcon)));
	connect(view,SIGNAL(changeTitle(QString)),this,SLOT(setWindowTitle(QString)));
	connect(view,SIGNAL(close()),this,SLOT(quit()));
	connect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished()));
	connect (clearAllAction, SIGNAL(triggered()), this, SIGNAL(clearAll()));
	view->LoadInternalPage("loader");

    // Size configuration:
	this->setMinimumSize(config.GetMinWidth(),config.GetMinHeight());
    if (config.GetUserWidth() < config.GetMinWidth() || config.GetUserHeight() < config.GetMinHeight())
    {
        this->resize(config.GetDefaultWidth(),config.GetDefaultHeight());
        connect(&config,SIGNAL(defaultSize(int,int)),this,SLOT(changeDefaultSize(int,int)));
    }
    else
    {
        this->resize(config.GetUserWidth(),config.GetUserHeight());
        disconnect(&config,SIGNAL(defaultSize(int,int)),this,SLOT(changeDefaultSize(int,int)));
    }

	this->setWindowTitle("Chargement en cours");
	if(config.GetScreenMode())
		this->showFullScreen();

	setCentralWidget(view);
	setUnifiedTitleAndToolBarOnMac(true);

}

/**
 * @brief Destructeur de MainWindow et de ses éléments
 */
MainWindow::~MainWindow()
{
	delete view;
	delete trayIcon;
	delete inspector;
	delete infos;
	delete fileMenu;
}

/**
 * @brief Affichage du menu lors d'un clic droit sur la fenêtre principale
 * @param pos	Position du pointeur lors du clic
 */
void MainWindow::showContextMenu(const QPoint &pos)
{

	QMenu myMenu;
	if(QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
	{
		myMenu.addAction(inspectAction);
	}
	if(!this->isFullScreen())
	{
		myMenu.addAction(fullscreenAction);
	}
	else
	{
		myMenu.addAction(normalscreenAction);
	}
	myMenu.addAction(quitAction);
    if (view->IsUpdating() == false)
        myMenu.addAction(reloadAction);
	myMenu.addAction(infoAction);
	myMenu.addAction(clearAllAction);
#ifdef Q_OS_WIN
	myMenu.addAction(sendlogAction);
#endif

	QPoint globalPos = this->mapToGlobal(pos);

	QAction* selectedItem = myMenu.exec(globalPos);
	if(selectedItem == NULL)
		return;

	if (selectedItem->text()==inspectAction->text())
	{
		inspector->show();
	}

	if (selectedItem->text()==fullscreenAction->text())
	{
		changeScreenMode(true);
	}

	if (selectedItem->text()==normalscreenAction->text())
	{
		changeScreenMode(false);
	}
	if (selectedItem->text()==reloadAction->text())
	{
	   view->reload();
	}
	if (selectedItem->text()==infoAction->text())
	{
		this->DisplayInfos();
	}
#ifdef Q_OS_WIN
	if (selectedItem->text()==sendlogAction->text())
	{
		MailSender mail;
		mail.AddFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation), qAppName() + ".log");
		mail.Send(sendlogAction->text());
	}
#endif
}

/**
 * @brief Change l'affichage de la fenêtre\n
 * Si on quitte le mode plein écran, on met la fenêtre au centre de l'écran et on le redimensionne avec les valeurs enregistrées
 * @param fullscreen	Vrai: met en mode plein écran, mode fenêtré sinon
 */
void MainWindow::changeScreenMode(bool fullscreen)
{
	ConfigManager &config = ConfigManager::Instance();
	if(fullscreen)
	{
		this->showFullScreen();
	}
	else
	{
		this->showNormal();
		this->setMinimumSize(config.GetMinWidth(),config.GetMinHeight());
        this->resize(config.GetUserWidth(),config.GetUserHeight());
		this->CenterScreen();
	}
	config.SetScreenMode(fullscreen);
}

/**
 * @brief Change l'accès aux outils développeur sur la page
 * @param toolsActivated	Vrai: active les outils, les désactive sinon
 */
void MainWindow::changeToolsMode(bool toolsActivated)
{
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, toolsActivated);
}

/**
 * @brief Change la taille minimale de la fenêtre si c'est possible
 * @param minWidth	Nouvelle largeur minimale
 * @param minHeight	Nouvelle hauteur minimale
 */
void MainWindow::changeMinSize(int minWidth, int minHeight)
{
	if(!this->isFullScreen())
	{
		this->setMinimumSize(minWidth,minHeight);
	}
}

/**
 * @brief Change la taille par défaut de la fenêtre si c'est possible
 * @param defaultWidth	Nouvelle largeur par défaut
 * @param defaultHeight	Nouvelle hauteur par défaut
 */
void MainWindow::changeDefaultSize(int defaultWidth, int defaultHeight)
{
	if(!this->isFullScreen())
	{
		this->resize(defaultWidth,defaultHeight);
        this->CenterScreen();
	}
}

/**
 * @brief Modifie les textes associés aux différents menus et actions associées
 * @param lang	Langue auquel le texte doit correspondre
 */
void MainWindow::changeActionNames(QString lang)
{
	//ATTENTION: Si il y a des modifications ici, il faut penser à celles dans informations.cpp
	if(lang == FR)
	{
		//Valeurs françaises
		fileMenu->setTitle("Fichier");
		clearAllAction->setText("Vider le cache");
		quitAction->setText("Fermer");
		inspectAction->setText("Inspecter");
		fullscreenAction->setText("Plein écran");
		normalscreenAction->setText("Fenêtré");
		reloadAction->setText("Recharger");
		infoAction->setText("Informations");
#ifdef Q_OS_WIN
		sendlogAction->setText("Envoi de logs");
#endif
	}
	else
	{
		//Valeurs anglaises
		fileMenu->setTitle("File");
		clearAllAction->setText("Clear cache");
		quitAction->setText("Quit");
		inspectAction->setText("Inspect");
		fullscreenAction->setText("Fullscreen");
		normalscreenAction->setText("Show normal");
		reloadAction->setText("Reload");
		infoAction->setText("Informations");
#ifdef Q_OS_WIN
		sendlogAction->setText("Send logs");
#endif
	}
}

/**
 * @brief Si la touche tapée est ESC, on quitte le mode plein écran, sinon on traite l'événement normalement
 * @param event Evénement de touche tapée
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		if(this->isFullScreen())
			this->changeScreenMode(false);
		QMainWindow::keyPressEvent(event); // call the default implementation
	}
	else
	{
		QMainWindow::keyPressEvent(event); // call the default implementation
	}
}

/**
 * @brief Si la fermeture est associée à la minimisation et que l'application ne force pas la fermeture, on minimise la fenêtre. Sinon, on ferme l'application
 * @param event Evénement de fermeture
 */
void MainWindow::closeEvent (QCloseEvent *event)
{
	ConfigManager &config = ConfigManager::Instance();
    if (config.GetCloseButtonBehaviour() && stayOpen)
	{
		event->ignore();
		this->setWindowState(Qt::WindowMinimized);
	}
    else if (view->page()->mainFrame()->evaluateJavaScript("(typeof onbeforeunload == 'function');") == true)
    {
        //view->page()->mainFrame()->evaluateJavaScript("window.onbeforeunload();");
        view->LoadInternalPage("loader");
        MyNetworkAccessManager *m_WebCtrl = MyNetworkAccessManager::Instance();
        CookieJar *cookieJar = m_WebCtrl->getCookieJar();
        connect(cookieJar,SIGNAL(cookieSaved()),this,SLOT(quit()));
        event->ignore();
        QTimer::singleShot(3000, this, SLOT(quit()));
    }
	else
	{
		this->deleteLater();
		event->accept();
	}
}

/**
 * @brief Quitte l'application
 */
void MainWindow::quit()
{
    stayOpen = false;
    view->DispatchJsEvent("Exit","window");
    this->close();
}

/**
 * @brief Change l'icône dans la barre de notification
 * @param icon  Icône à placer
 */
void MainWindow::changeIcon(const QIcon &icon)
{
	this->trayIcon->setIcon(icon);
	if(!windowIconSpecified)
	{
		this->setWindowIcon(icon);
		infos->setWindowIcon(icon);
		inspector->setWindowIcon(icon);
	}
}

/**
 * @brief Fait la transition entre la page de chargement et la page principale
 */
void MainWindow::loadFinished()
{
	ConfigManager &config = ConfigManager::Instance();
	disconnect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished()));
    qDebug() << "Launch url: " << config.GetLaunchUrl();
    qDebug() << "Base url(s): " << config.GetBaseUrl();
	view->load(QUrl(config.GetLaunchUrl()));
}

/**
 * @brief Met à jour les données de la page d'information et l'affiche
 */
void MainWindow::DisplayInfos()
{
	infos->UpdateValues();
	infos->show();
}

/**
 * @brief Met la page principale au centre de l'écran
 */
void MainWindow::CenterScreen()
{
	QPoint center = this->rect().center();
	//On recalibre le centre afin que les bordures soient présentes dans le cadre lorsque la taille est au maximum
	center.setX(center.x() + WINDOW_FRAME_WIDTH/2);
	center.setY(center.y() + WINDOW_FRAME_HEIGHT);
	this->move(QApplication::desktop()->screen()->rect().center() - center);
}

/**
 * @brief Enregistre la taille courante de la fenêtre lors du resize par l'utilisateur
 */
void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   ConfigManager &config = ConfigManager::Instance();

   if(!this->isFullScreen())
    config.SetUserSize(event->size().width(),event->size().height());
}
