#include "mainwindow.h"
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
	connect(&config,SIGNAL(defaultSize(int,int)),this,SLOT(changeDefaultSize(int,int)));
	connect(&config,SIGNAL(newLanguage(QString)),this,SLOT(changeActionNames(QString)));

	stayOpen = true;
	infos = new Informations();

	//On initialise les actions des différents menus
	clearCookiesAction = new QAction(this);
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
	QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
	QWebSettings::globalSettings()->enablePersistentStorage(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/cache");
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
	fileMenu->addAction(clearCookiesAction);
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
	}

	connect(view,SIGNAL(changeIcon(QIcon)),this,SLOT(changeIcon(QIcon)));
	connect(view,SIGNAL(changeTitle(QString)),this,SLOT(setWindowTitle(QString)));
	connect(view,SIGNAL(close()),this,SLOT(quit()));
	connect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished()));
	connect (clearCookiesAction, SIGNAL(triggered()), this, SIGNAL(clearCookies()));
	view->LoadInternalPage("loader");

	this->setMinimumSize(config.GetMinWidth(),config.GetMinHeight());
	this->resize(config.GetDefaultWidth(),config.GetDefaultHeight());
	this->CenterScreen();
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
	myMenu.addAction(reloadAction);
	myMenu.addAction(infoAction);
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
	if (selectedItem->text()==quitAction->text())
	{
		//L'action est déjà connectée à la méthode quit, on ne fait rien
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
		this->resize(config.GetDefaultWidth(),config.GetDefaultHeight());
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
		this->CenterScreen();
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
	if(lang == "fr")
	{
		//Valeurs françaises
		fileMenu->setTitle("Fichier");
		clearCookiesAction->setText("Nettoyer les cookies");
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
		clearCookiesAction->setText("Clear cookies");
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
	else
	{
		this->deleteLater();
		event->accept();
	}
}

/**
 * @brief Quitte l'application
 */
void MainWindow::quit ()
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
