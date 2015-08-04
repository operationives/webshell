#include "mainwindow.h"
#include "wnavigator.h"
#include "global.h"
#ifdef Q_OS_WIN
#include "mailsender.h"
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

	connect(config,SIGNAL(toolsMode(bool)),this,SLOT(changeToolsMode(bool)));
	connect(config,SIGNAL(minSize(int,int)),this,SLOT(changeMinSize(int,int)));
	connect(config,SIGNAL(defaultSize(int,int)),this,SLOT(changeDefaultSize(int,int)));

	stayOpen = true;
	launch = false;

	infos = new Informations();

	//Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
	QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
	QWebSettings::globalSettings()->enablePersistentStorage(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/cache");
	QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, config->GetDeveloperToolsMode());

	//On définit les actions du menu de trayIcon
	QAction *quitAction = new QAction("Quitter", this);
	connect (quitAction, SIGNAL(triggered()), this, SLOT(quit()));
	QMenu *trayIconMenu = new QMenu(this);
	trayIconMenu->addAction (quitAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu (trayIconMenu);
	trayIcon->show();

	//Ajout du menu dans la barre de titre
	fileMenu = menuBar()->addMenu(tr("&Fichier"));
	fileMenu->addAction(quitAction);
	QAction *clearCookies = new QAction("&Effacer les cookies", this);
	fileMenu->addAction(clearCookies);
	menuBar()->setVisible(config->GetMenuBarPresent());
	connect(config,SIGNAL(menuBarPresence(bool)),menuBar(),SLOT(setVisible(bool)));

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
	connect (clearCookies, SIGNAL(triggered()), this, SIGNAL(clearCookies()));
	view->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/loader.html")));

	this->setMinimumSize(config->GetMinWidth(),config->GetMinHeight());
	this->resize(config->GetDefaultWidth(),config->GetDefaultHeight());
	this->setWindowTitle("Chargement en cours");
	if(config->GetScreenMode())
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
#ifdef Q_OS_WIN
	myMenu.addAction("Envoi de logs");
#endif

	QPoint globalPos = this->mapToGlobal(pos);
	//Correction de position
	globalPos.setY(globalPos.ry()+20);

	QAction* selectedItem = myMenu.exec(globalPos);
	if(selectedItem == NULL)
		return;

	if (selectedItem->text()=="Inspect")
	{
		inspector->show();
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
#ifdef Q_OS_WIN
	if (selectedItem->text()=="Envoi de logs")
	{
		MailSender mail;
		mail.AddFile(QApplication::applicationDirPath(),qAppName()+".log");
		mail.Send("Envoi de logs");
	}
#endif
}

/**
 * @brief Change l'affichage de la fenêtre
 * @param fullscreen	Vrai: met en mode plein écran, mode fenêtré sinon
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
 * @brief Change l'accès aux outils développeur sur la page
 * @param toolsActivated	Vrai: active les outils, les désactive sinon
 */
void MainWindow::changeToolsMode(bool toolsActivated)
{
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, toolsActivated);
}

/**
 * @brief Change la taille minimale de la fenêtre
 * @param minWidth	Nouvelle largeur minimale
 * @param minHeight	Nouvelle hauteur minimale
 */
void MainWindow::changeMinSize(int minWidth, int minHeight)
{
	this->setMinimumSize(minWidth,minHeight);
}

/**
 * @brief Change la taille par défaut de la fenêtre
 * @param defaultWidth	Nouvelle largeur par défaut
 * @param defaultHeight	Nouvelle hauteur par défaut
 */
void MainWindow::changeDefaultSize(int defaultWidth, int defaultHeight)
{
	this->resize(defaultWidth,defaultHeight);
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
	if(!launch)
	{
		disconnect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished()));
		view->load(QUrl(config->GetLaunchUrl()));
		launch = true;
	}
}

/**
 * @brief Met à jour les données de la page d'information et l'affiche
 */
void MainWindow::DisplayInfos()
{
	infos->UpdateValues();
	infos->show();
}
