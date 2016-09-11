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
    m_translator                = NULL;
    m_loadingLabel              = NULL;
    m_loaderIcon                = NULL;
    m_loadingTimer              = NULL;
    refreshTimer                = NULL;
    stopRefreshTimer            = NULL;
    trayIcon                    = NULL;
    inspector                   = NULL;
    infos                       = NULL;
    m_progressBar               = NULL;
    m_checkInternetStatusTimer  = NULL;
    m_notification              = NULL;
    m_notificationLabel         = NULL;
    m_showNotificationAnimation = NULL;
    m_hideNotificationAnimation = NULL;

    m_is_everything_saved_before_exiting = false;

    // NETWORK CONFIGURATION:
    m_networkConfigurationManager = new QNetworkConfigurationManager();
    QNetworkConfiguration cfg = m_networkConfigurationManager->defaultConfiguration();
    if (!cfg.isValid())
    {   
        qWarning() << "Invalid network configuration at start.";
        m_session = NULL;
        m_currentNetworkState = QNetworkSession::Invalid;
        m_lastNetworkState = m_currentNetworkState;
        delete m_networkConfigurationManager;
        m_networkConfigurationManager = NULL;

        m_checkInternetStatusTimer = new QTimer(this);
        m_checkInternetStatusTimer->setInterval(CHECK_INTERNET_STATUS_TICK_TIMER);
        connect(m_checkInternetStatusTimer, SIGNAL(timeout()), this, SLOT(checkNetworkConfiguration()));
        m_checkInternetStatusTimer->start(CHECK_INTERNET_STATUS_TICK_TIMER);
    }
    else
    {
        m_session = new QNetworkSession(cfg);
        connect(m_session, SIGNAL(stateChanged(QNetworkSession::State)), this,  SLOT(handleNetworkStateChanged(QNetworkSession::State))/*SLOT(stateChanged(QNetworkSession::State))*/);
        m_currentNetworkState = m_session->state();
        m_lastNetworkState = m_currentNetworkState;
    }

    QNetworkProxyFactory::setUseSystemConfiguration(true);

	ConfigManager &config = ConfigManager::Instance();
	connect(&config,SIGNAL(toolsMode(bool)),this,SLOT(changeToolsMode(bool)));
	connect(&config,SIGNAL(minSize(int,int)),this,SLOT(changeMinSize(int,int)));
    connect(&config,SIGNAL(newLanguage(QString)),this,SLOT(changeActionNames(QString)));
    connect(&config,SIGNAL(newLanguage(QString)),this,SLOT(loadTranslator(QString)));

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
    //clearPointUrlAction = new QAction(this);

#ifdef Q_OS_WIN
	sendlogAction = new QAction(this);
#endif

	//Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
    QWebSettings::globalSettings()->enablePersistentStorage(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+config.GetAppName());
	QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, config.GetDeveloperToolsMode());
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);

	connect (quitAction, SIGNAL(triggered()), this, SLOT(quit()));
	QMenu *trayIconMenu = new QMenu(this);
	trayIconMenu->addAction (quitAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu (trayIconMenu);
    //trayIcon->show();

	//Ajout du menu dans la barre de titre
	fileMenu = menuBar()->addMenu(tr("&Fichier"));
	fileMenu->addAction(quitAction);
	fileMenu->addAction(clearAllAction);
    //fileMenu->addAction(clearPointUrlAction);

	menuBar()->setVisible(config.GetMenuBarPresent());
	connect(&config,SIGNAL(menuBarPresence(bool)),menuBar(),SLOT(setVisible(bool)));

	changeActionNames(config.GetLanguage());

	view = new MyWebView(this);

    //DPI awareness
#ifdef Q_OS_WIN
    int horizontalDpi = QApplication::desktop()->screen()->logicalDpiX();
    float zoom_factor = horizontalDpi / 96.0;
    if(zoom_factor!=1.0)
    {
        view->setZoomFactor(zoom_factor);
        qWarning() << "Apply [ x" << zoom_factor << "] zoom factor for [" <<  horizontalDpi << "dpi ] high resolution adaptation";
    }
#endif

	//On indique qu'on utilise un menu personnalisé
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(view,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

	//Initialisation de l'inspecteur de la page
	inspector = new QWebInspector();
	inspector->setPage(view->page());
    inspector->resize(QSize(QApplication::desktop()->screenGeometry().width(),QApplication::desktop()->screenGeometry().height()/4));

    windowIconSpecified = !iconPath.isNull();
	if(windowIconSpecified)
    {
        QIcon windowIcon(iconPath);
		this->setWindowIcon(windowIcon);
		infos->setWindowIcon(windowIcon);
        inspector->setWindowIcon(windowIcon);

        this->trayIcon->setIcon(windowIcon);
        trayIcon->show();
	}

#ifdef Q_OS_WIN
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7 && QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS10)
    {
        jumplist = new QWinJumpList(this);
        tasks = jumplist->tasks();
        quitItem = new QWinJumpListItem(QWinJumpListItem::Link);
        if (config.GetLanguage() == FR)
        {
            quitItem->setDescription("Quitter l'application");
            quitItem->setTitle("Quitter");
        }
        else
        {
            quitItem->setDescription("Quit the application");
            quitItem->setTitle("Quit");
        }
        quitItem->setArguments(QStringList("--quit"));
        quitItem->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        tasks->addItem(quitItem);
        tasks->setVisible(true);
    }
#endif

    //connect(view,SIGNAL(changeIcon(QIcon)),this,SLOT(changeIcon(QIcon)));
	connect(view,SIGNAL(changeTitle(QString)),this,SLOT(setWindowTitle(QString)));
	connect(view,SIGNAL(close()),this,SLOT(quit()));
    connect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
    //connect (clearPointUrlAction, SIGNAL(triggered()), this, SLOT(clearPointUrl()));
    //connect (clearPointUrlAction, SIGNAL(triggered()), this, SLOT(showClearPointUrlNotification()));
	connect (clearAllAction, SIGNAL(triggered()), this, SIGNAL(clearAll()));
    connect (clearAllAction, SIGNAL(triggered()), this, SLOT(showClearAllNotification()));
    view->LoadInternalPage("loader");



    // Size configuration:
	this->setMinimumSize(config.GetMinWidth(),config.GetMinHeight());
    if (config.GetUserWidth() < config.GetMinWidth() || config.GetUserHeight() < config.GetMinHeight())
    {
        config.SetUserSize(config.GetDefaultWidth(),config.GetDefaultHeight());
        this->resize(config.GetDefaultWidth(),config.GetDefaultHeight());
        connect(&config,SIGNAL(defaultSize(int,int)),this,SLOT(changeDefaultSize(int,int)));
    }
    else
    {
        this->resize(config.GetUserWidth(),config.GetUserHeight());
        disconnect(&config,SIGNAL(defaultSize(int,int)),this,SLOT(changeDefaultSize(int,int)));
    }

    //this->setWindowTitle("Chargement en cours");

    setCentralWidget(view);
	setUnifiedTitleAndToolBarOnMac(true);

    // Handle the UI refresh:
    refreshTimer = new QTimer(this);
    stopRefreshTimer = new QTimer(this);
    stopRefreshTimer->setSingleShot(true);
    stopRefreshTimer->setInterval(FORCED_REFRESH_DURATION);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(forceGuiUpdate()));
    connect(view, SIGNAL(mousePressed()), this, SLOT(startForceGuiUpdate()));

    // PROGRESS BAR
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setTextVisible(true);
    m_progressBar->hide();
    m_progressBar->setTextVisible(false);
    m_progressBar->resize(QSize(this->size().width(),10));
    //QString style = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #2978FF,stop: 0.4999 #FF78FF,stop: 0.5 #2978FF,stop: 1 #FF78FF );}";
    QString style = "QProgressBar::chunk {background-color: #2978FF;}";
    m_progressBar->setStyleSheet(style);

    m_loadingLabel = new QLabel(this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setGeometry(QRect(0, 0, this->size().width(), this->size().height()-m_progressBar->height()));
    //m_loadingLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_loadingLabel->setStyleSheet("background-color: rgba(255, 255, 255, 90);");
    m_loaderIcon = new QMovie(QApplication::applicationDirPath()+"/loader.gif");
    m_loaderIcon->setScaledSize(QSize(100,100));
    if (!m_loaderIcon->isValid())
    {
        qDebug() << "Invalid loader movie";
    }
    else
    {
        m_loadingLabel->setMovie(m_loaderIcon);
    }
    m_loadingLabel->hide();
    m_loadingTimer = new QTimer(this);
    m_loadingTimer->setSingleShot(true);
    m_loadingTimer->setInterval(LOADER_TRIGGER_TIME);
    connect(m_loadingTimer, SIGNAL(timeout()), this, SLOT(displayLoader()));

    createNotification();

#ifdef Q_OS_WIN
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
    {
        m_taskbarButton = new QWinTaskbarButton(this);
        m_taskbarButton->setWindow(windowHandle());

        m_taskbarProgress = m_taskbarButton->progress();
        m_taskbarProgress->setVisible(true);
        m_taskbarProgress->setRange(0, 100);
        m_taskbarProgress->setValue(50);
    }
    else
    {
        m_taskbarButton     = NULL;
        m_taskbarProgress   = NULL;
    }
#endif

    switch(config.GetScreenMode())
    {
        case FULLSCREEN:
            changeScreenMode(true);
        break;
        case MAXIMIZED:
            oldSize = QSize(config.GetUserWidth(),config.GetUserHeight());
            this->showMaximized();
            break;
        case WINDOWED:
        default:
            break;
    }
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
    if (refreshTimer->isActive())
        refreshTimer->stop();
    delete refreshTimer;
    if (stopRefreshTimer->isActive())
        stopRefreshTimer->stop();
    delete stopRefreshTimer;
    if (m_loadingTimer->isActive())
        m_loadingTimer->stop();
    delete m_loadingTimer;

#ifdef Q_OS_WIN
    if (m_taskbarButton) delete m_taskbarButton;
#endif
    delete m_session;
    delete m_loaderIcon;
    delete m_loadingLabel;
    delete m_notificationLabel;
    delete m_showNotificationAnimation;
    delete m_hideNotificationAnimation;
    delete m_notification;
}

/**
 * @brief Creer une fenêtre de notifcation avec animation
 */
void MainWindow::createNotification()
{
    // NOTIFICATION WIDGET CREATION

    m_notification = new QWidget(this);
    m_notification->setGeometry(QRect(-NOTIFICATION_WIDTH, 0, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));

    // NOTIFICATION STYLE
    QString notification_style = "color:#FFFFFF; font-size: 14px; font-weight: bold; font-family: Arial, sans-serif;";

    // NOTIFICATION ICON CREATION

    QLabel *notification_icon = new QLabel(m_notification);
    notification_icon->setGeometry(QRect(0, 0, NOTIFICATION_HEIGHT, NOTIFICATION_HEIGHT));
    notification_icon->setStyleSheet(notification_style + "background-color: rgba(0, 0, 0, 0)");

    QPixmap notification_pixmap(QString(QApplication::applicationDirPath()+"./icon_notification.png"));
    notification_icon->setPixmap(notification_pixmap);
    notification_icon->show();

    // NOTIFICATION LABEL CREATION

    m_notificationLabel = new QLabel(m_notification);
    m_notificationLabel->setAlignment(Qt::AlignCenter);
    m_notificationLabel->setGeometry(QRect(0, 0, NOTIFICATION_WIDTH-notification_icon->width(), NOTIFICATION_HEIGHT));
    m_notificationLabel->setStyleSheet(notification_style + "background-color: rgba(0, 0, 0, 0);");
    m_notificationLabel->show();

    QHBoxLayout *layout = new QHBoxLayout(m_notification);
    layout->addWidget(notification_icon);
    layout->addWidget(m_notificationLabel);

    m_notification->setStyleSheet(notification_style + "background-color: rgba(0, 0, 0, 0.9);");
    m_notification->setLayout(layout);
    m_notification->show();

    // SHOW/HIDE ANIMATIONS

    m_showNotificationAnimation = new QPropertyAnimation(m_notification , "geometry");
    m_showNotificationAnimation->setDuration(NOTIFICATION_ANIMATION_DURATION);
    m_showNotificationAnimation->setStartValue(QRect(-NOTIFICATION_WIDTH, NOTIFICATION_YPOS, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));
    m_showNotificationAnimation->setEndValue(QRect(NOTIFICATION_XPOS, NOTIFICATION_YPOS, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));

    m_hideNotificationAnimation = new QPropertyAnimation(m_notification , "geometry");
    m_hideNotificationAnimation->setDuration(NOTIFICATION_ANIMATION_DURATION);
    m_hideNotificationAnimation->setStartValue(QRect(NOTIFICATION_XPOS, NOTIFICATION_YPOS, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));
    m_hideNotificationAnimation->setEndValue(QRect(-NOTIFICATION_WIDTH, NOTIFICATION_YPOS, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));

    //m_showNotificationAnimation->start();
    //m_notificationLabel->setText("Vous venez d'être déconnecté du réseau");
}

/**
 * @brief Affichage du menu lors d'un clic droit sur la fenêtre principale
 * @param pos	Position du pointeur lors du clic
 */
void MainWindow::showContextMenu(const QPoint &pos)
{

	QMenu myMenu;

	if(!this->isFullScreen())
	{
		myMenu.addAction(fullscreenAction);
	}
	else
	{
		myMenu.addAction(normalscreenAction);
	}
    myMenu.addAction(infoAction);

    myMenu.addSeparator();
    if(QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
    {
        myMenu.addAction(inspectAction);
    }
    if ( (view->IsUpdating() == false) && (m_currentNetworkState == QNetworkSession::Connected) )
        myMenu.addAction(reloadAction);
    //myMenu.addAction(clearPointUrlAction);

	myMenu.addAction(clearAllAction);
#ifdef Q_OS_WIN
	myMenu.addAction(sendlogAction);
#endif

    myMenu.addSeparator();
    myMenu.addAction(quitAction);

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
        if (m_currentNetworkState != QNetworkSession::Connected) return;

        ConfigManager &config = ConfigManager::Instance();
        QUrl reloading_url;
        if ( m_currentNetworkState == QNetworkSession::Connected && view->url().url().startsWith("file:") )
        {
            reloading_url = QUrl(config.GetLaunchUrl());
            qDebug() << "Reconnection => load the base url: " << reloading_url.url();
        }
        else
        {
            reloading_url = QUrl(config.GetSavedAdress());
            qDebug() << "Reload the current page: " << reloading_url.url();
        }
        view->load(reloading_url);
        connect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));
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

void MainWindow::checkNetworkConfiguration()
{
    if(!m_session)
    {
        if (!m_networkConfigurationManager)
            m_networkConfigurationManager = new QNetworkConfigurationManager();
        if (m_networkConfigurationManager->isOnline())
        {
            m_checkInternetStatusTimer->stop();
            m_networkConfigurationManager->updateConfigurations();
            connect(m_networkConfigurationManager, SIGNAL(updateCompleted()), this, SLOT(handleNetworkConfigurationUpdated()));
        }
    }
}

void MainWindow::handleNetworkConfigurationUpdated()
{
    qDebug() << "Network configuration updated. Ready to reconnect the app.";
    disconnect(m_networkConfigurationManager, SIGNAL(updateCompleted()), this, SLOT(handleNetworkConfigurationUpdated()));
    QNetworkConfiguration cfg = m_networkConfigurationManager->defaultConfiguration();
    if (!cfg.isValid())
    {
        qWarning() << "Network Configuration invalide";
    }
    else
    {
        if (!m_session)
        {
            m_session = new QNetworkSession(cfg);
            m_session->open();
            if (m_session->waitForOpened(1000))
            {
                qDebug() << "Network Session open.";
                connect(m_session, SIGNAL(stateChanged(QNetworkSession::State)), this,  SLOT(handleNetworkStateChanged(QNetworkSession::State))/*SLOT(stateChanged(QNetworkSession::State))*/);
            }
        }
    }
    m_currentNetworkState = m_session->state();
    m_lastNetworkState = m_currentNetworkState;

    if ( view->url().url().startsWith("file:") )
    {
        QUrl reloading_url = QUrl(ConfigManager::Instance().GetSavedAdress());
        view->load(reloading_url);
        connect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));
    }
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
        if (&config != NULL)
        {
            config.SetScreenMode(FULLSCREEN);
            m_windowSizeBeforeFullscreen = QSize(config.GetUserWidth(),config.GetUserHeight());
            this->showFullScreen();
            config.SetUserSize(m_windowSizeBeforeFullscreen.width(), m_windowSizeBeforeFullscreen.height());
        }
	}
	else
	{
        if (&config != NULL)
        {
            config.SetScreenMode(WINDOWED);
            config.SetUserSize(m_windowSizeBeforeFullscreen.width(), m_windowSizeBeforeFullscreen.height());
        }
        this->showNormal();
        this->setMinimumSize(config.GetMinWidth(),config.GetMinHeight());
        // We get back to the size before the fullscreen mode.
        // We use the variable m_windowSizeBeforeFullscreen to avoid erroneous values from the resize event.
        oldSize = m_windowSizeBeforeFullscreen;
        this->resize(m_windowSizeBeforeFullscreen.width(), m_windowSizeBeforeFullscreen.height());
        this->CenterScreen();
	}

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
    if(!this->isFullScreen() && this->windowState() != Qt::WindowMaximized)
	{
		this->resize(defaultWidth,defaultHeight);
        this->CenterScreen();
	}

}

/**
 * @brief Charge le dictionnaire de traduction lié à la lagnue selectionnée
 * @param lang	Langue cible pour la traduction
 */
void MainWindow::loadTranslator(QString lang)
{
    if (m_translator)
    {
        QApplication::instance()->removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }

    if (lang == "en") return; //No need of translator

    m_translator = new QTranslator();

    if ( m_translator->load(QLatin1String("qt_") + lang, QApplication::instance()->applicationDirPath() + QLatin1String("/translations")) )
    {
        qDebug() << "Load a new translator for [" << lang << "] language";
        QApplication::instance()->installTranslator(m_translator);
    }
    else
    {
        qDebug() << "Failed to install a translator for the language [" << lang << "] in path [" << QApplication::instance()->applicationDirPath() + QLatin1String("/translations/qt_")  + lang + ".qm" << "]";
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
        //clearPointUrlAction->setText("Effacer l'url du service");
		clearAllAction->setText("Vider le cache");
        quitAction->setText("Quitter");
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
        //clearPointUrlAction->setText("Delete service url");
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
    if (event->key() == Qt::Key_Asterisk && event->modifiers() & Qt::ControlModifier && event->modifiers() & Qt::AltModifier)
    {
        clearPointUrl();
        showClearPointUrlNotification();
    }

    if (event->key() == Qt::Key_Escape)
	{
		if(this->isFullScreen())
			this->changeScreenMode(false);
		QMainWindow::keyPressEvent(event); // call the default implementation
	}
    else if (event->key() == Qt::Key_F5)
    {
        ConfigManager &config = ConfigManager::Instance();
        QString savedAdress(config.GetSavedAdress());
        qDebug() << "Reload the current page: " << savedAdress;
        view->load(QUrl(savedAdress));
        connect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));
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
    else if (!m_is_everything_saved_before_exiting)
    {
        view->LoadInternalPage("loader");
        disconnect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));
        MyNetworkAccessManager *m_WebCtrl = MyNetworkAccessManager::Instance();
        CookieJar *cookieJar = m_WebCtrl->getCookieJar();
        cookieJar->saveNow();
        m_is_everything_saved_before_exiting = true;
        connect(cookieJar,SIGNAL(cookieSaved()),this,SLOT(quit()));
        event->ignore();
        QTimer::singleShot(EXITING_TIMEOUT, this, SLOT(quit()));
    }
	else
	{
        qDebug() << "End of session";
        qDebug() << "------------------------------------------";
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
    trayIcon->show();
}

/**
 * @brief Fait la transition entre la page de chargement et la page principale
 */
void MainWindow::loadFinished(bool ok)
{
	ConfigManager &config = ConfigManager::Instance();

    ConfigManager::Instance().SetSavedAdress(config.GetLaunchUrl());
    if (!ok)
    {
        view->LoadInternalPage("disconnected");
        qCritical() << __FUNCTION__ << " : Le service n'est pas accessible";
        return;
    }
    disconnect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
    qDebug() << "Launch url: " << config.GetLaunchUrl();
    qDebug() << "Base url(s): " << config.GetBaseUrl();

    connect(view,SIGNAL(loadFinished(bool)),this,SLOT(handleLoadFinished(bool)));
    connect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));

    m_is_started = false;
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

   // Clear the progress bar:
   if (m_progressBar->value() == 100) m_progressBar->hide();

   if (this->windowState() == Qt::WindowNoState && !this->isFullScreen() && event->spontaneous() == true)
   {
        config.SetUserSize(event->size().width(),event->size().height());
        qDebug() << "Set User size "  << event->size().width() << " " << event->size().height();
   }

   oldSize = event->oldSize(); //Used to retrieve the old size after maximising event

   if (m_loadingLabel)
   {
       m_loadingLabel->setGeometry(QRect(0, 0, this->size().width(), this->size().height()-m_progressBar->height()));
   }

   if (m_progressBar)
   {
        m_progressBar->resize(QSize(this->size().width(),10));
        m_progressBar->move(0,this->size().height()-m_progressBar->size().height());
   }
}

void MainWindow::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QWindowStateChangeEvent* event = static_cast< QWindowStateChangeEvent* >( e );
        ConfigManager &config = ConfigManager::Instance();
        if (&config == NULL) return;
        /*if( event->oldState() & Qt::WindowMinimized )
        {
            qDebug() << "Window restored (to normal or maximized state)!";
        }
        else*/
        if( this->windowState() == Qt::WindowNoState )
        {
            config.SetScreenMode(WINDOWED);
        }
        else if( this->windowState() == Qt::WindowFullScreen )
        {
            config.SetScreenMode(FULLSCREEN);
        }
        else if( event->oldState() == Qt::WindowNoState && this->windowState() == Qt::WindowMaximized )
        {
            // Window restored to maximised state
            // The window size is not stored. The previous one is stored
            config.SetScreenMode(MAXIMIZED);
            if (oldSize.width() && oldSize.height())
                config.SetUserSize(oldSize.width(),oldSize.height());
        }
    }

    startForceGuiUpdate();
}

void MainWindow::startForceGuiUpdate()
{
    if (refreshTimer && stopRefreshTimer)
    {
        refreshTimer->start(FORCED_REFRESH_TICK_TIMER);       // Refresh UI every 200ms...
        stopRefreshTimer->start(FORCED_REFRESH_DURATION);    // ...during 3s to avoid visual artifacts!
    }
}

void MainWindow::forceGuiUpdate()
{
    // Clear the progress bar:
    if (m_progressBar)
        if (m_progressBar->value() == 100) m_progressBar->hide();
    view->update();
}

void MainWindow::stopForceGuiUpdate()
{
    refreshTimer->stop();
}

void MainWindow::showEvent(QShowEvent *e)
{
#ifdef Q_OS_WIN32
    m_taskbarButton->setWindow(windowHandle());
#endif

    e->accept();
}

void MainWindow::handleLoadProgress(int progress)
{
    if (m_progressBar)
    {
        m_progressBar->setValue(progress);
        m_progressBar->show();
        m_progressBar->resize(QSize(this->size().width(),10));
        m_progressBar->move(0,this->size().height()-m_progressBar->size().height());
    }

    if (view->url().url().contains("/client/") && m_loadingTimer)
    {
        m_loadingTimer->start(LOADER_TRIGGER_TIME);
    }

#ifdef Q_OS_WIN
    if (m_taskbarProgress)
    {
        m_taskbarProgress->setVisible(true);
        m_taskbarProgress->setValue(progress);
    }
#endif
}

void MainWindow::displayLoader()
{
    if (m_loadingLabel)
    {
        m_loaderIcon->start();
        m_loadingLabel->show();
    }
}

void MainWindow::handleLoadFinished(bool ok)
{
    if (m_progressBar) m_progressBar->hide();
    if (m_loadingLabel)
    {
        m_loadingTimer->stop();
        m_loaderIcon->stop();
        m_loadingLabel->hide();
    }

#ifdef Q_OS_WIN
    if (m_taskbarProgress)
        m_taskbarProgress->setVisible(false);
#endif

    if (!ok)
    {
        qDebug() << "The launch url can't be reached: " << view->url();
        // The launch url is not in cache.
        // So we show the error page if the launch url (login or index page) can't be reached.
        // m_is_started takes the value 'true' if the launch url is reached
        if ( m_is_started == false )/*||
             view->url().url().contains("index.php") ||
             view->url().url().contains("login.php") )*/
        {
            disconnect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));
            view->LoadInternalPage("disconnected");
            qWarning() << "MainWindow:" << __FUNCTION__ << " : The base url can't be reached";
            return;
        }
    }
    else
    {
        qDebug() << "Save current page: " << view->url().toString();
        ConfigManager::Instance().SetSavedAdress(view->url().toString());
        m_is_started = true;
    }
}

void MainWindow::handleDownloadFinished()
{
    if (m_progressBar) m_progressBar->hide();

#ifdef Q_OS_WIN
    if (m_taskbarProgress)
        m_taskbarProgress->setVisible(false);
#endif
}

void MainWindow::handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int progress = 0;
    if (bytesReceived && bytesTotal)
        progress = (float)bytesReceived/bytesTotal*100;

    m_progressBar->setValue(progress);
    m_progressBar->show();
    m_progressBar->resize(QSize(this->size().width(),10));
    m_progressBar->move(0,this->size().height()-m_progressBar->size().height());

#ifdef Q_OS_WIN
    if (m_taskbarProgress)
    {
        m_taskbarProgress->setVisible(true);
        m_taskbarProgress->setValue(progress);
    }
#endif
}

void MainWindow::hideNotification()
{
    if (m_hideNotificationAnimation)
        m_hideNotificationAnimation->start();
}

void MainWindow::showClearPointUrlNotification()
{
    QString notification_text;
    QString notification_title;

    if (ConfigManager::Instance().GetDisplayName() == "")
        notification_title = ConfigManager::Instance().GetAppName();
    else
        notification_title = ConfigManager::Instance().GetDisplayName();

    if(ConfigManager::Instance().GetLanguage() == FR)
    {
        notification_text   = "La configuration de l'url du service a été corretement supprimée";
    }
    else
    {
        notification_text   = "Url service successfully cleared";
    }

    trayIcon->showMessage(notification_title,notification_text);
}

void MainWindow::showClearAllNotification()
{
    QString notification_text;
    QString notification_title;

    if (ConfigManager::Instance().GetDisplayName() == "")
        notification_title = ConfigManager::Instance().GetAppName();
    else
        notification_title = ConfigManager::Instance().GetDisplayName();

    if(ConfigManager::Instance().GetLanguage() == FR)
    {
        notification_text   = "Les données ont été supprimées du cache";
    }
    else
    {
        notification_text   = "Web cache successfully cleared";
    }

    /*m_notificationLabel->setText(notification_text);
    m_showNotificationAnimation->start();
    QTimer::singleShot(NOTIFICATION_DURATION, this, SLOT(hideNotification()));*/

    trayIcon->showMessage(notification_title,notification_text);
}

void MainWindow::handleNetworkStateChanged(QNetworkSession::State state)
{
    QString notification_text;
    QString notification_title;

    if (ConfigManager::Instance().GetDisplayName() == "")
        notification_title = ConfigManager::Instance().GetAppName();
    else
        notification_title = ConfigManager::Instance().GetDisplayName();

    m_currentNetworkState = state;
    qDebug() << "Network state changed: from [" << m_lastNetworkState << "] to [" << m_currentNetworkState << "]";

    if ( m_currentNetworkState == QNetworkSession::Connected )
    {
        qDebug() << "Network is accessible";

        if(ConfigManager::Instance().GetLanguage() == FR)
        {
            notification_text   = "Vous utilisez le service en ligne";
        }
        else
        {
            notification_text   = "The application is inline";
        }
    }
    else
    {
        qDebug() << "Network is not accessible";

        if(ConfigManager::Instance().GetLanguage() == FR)
        {
            notification_text   = "Vous utilisez l'application hors ligne";
        }
        else
        {
            notification_text   = "The application is offline";
        }

        //if (QSysInfo::windowsVersion() <= QSysInfo::WV_WINDOWS7)
        if (USE_IN_APP_NOTIFICATIONS)
        {
            if (m_notificationLabel && m_hideNotificationAnimation)
            {
                m_notificationLabel->setText(notification_text);
                m_showNotificationAnimation->start();
                QTimer::singleShot(NOTIFICATION_DURATION, this, SLOT(hideNotification()));
            }
        }

        if ( view->url().url().contains("index.php") ||
             view->url().url().contains("login.php") ||
             (m_progressBar->value() > 0 && m_progressBar->value() < 100 && m_progressBar->isVisible()) )
        {
            disconnect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));
            view->LoadInternalPage("disconnected");
            qWarning() << "MainWindow:" << __FUNCTION__ << " : The base url can't be reached";
        }
    }

    trayIcon->showMessage(notification_title,notification_text);

    // Restart the service when a network reconection occured after display the local disconnected page:
    if (    m_currentNetworkState == QNetworkSession::Connected &&
            m_currentNetworkState != m_lastNetworkState &&
            view->url().url().startsWith("file:") )
    {
        ConfigManager &config = ConfigManager::Instance();
        qDebug() << "Reconnection => load the service url: " << config.GetLaunchUrl();
        view->load(QUrl(config.GetLaunchUrl()));
        connect(view,SIGNAL(loadProgress(int)),this,SLOT(handleLoadProgress(int)));

    }
    m_lastNetworkState = m_currentNetworkState;
}

void MainWindow::clearPointUrl()
{
    QSettings settings;
    settings.remove("config/point");
}
