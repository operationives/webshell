#include <QWebFrame>
#include "mywebview.h"
#include "wnavigator.h"
#include "navigatorplugins.h"
#include "webapp.h"
#include "Outils/configmanager.h"

/**
 * @brief Constructeur de notre webview et des objets à intégrer dans l'application
 * @param parent	Widget parent, dans le cas présent il s'agit de la MainWindow
 */
MyWebView::MyWebView(QWidget *parent) : QWebView(parent)
{
	//On enlève les barres de défilement inutiles dans le cadre du webshell
	this->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	this->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

	navigatorplugins = new NavigatorPlugins(this);
	wapp = new WebApp(this);
	webshellParameters = new WebshellParameters();
	wnavigator = new WNavigator(this,webshellParameters);
	//On permet l'accès aux méthodes dans WNavigator par les appels javascript
	this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
	this->page()->mainFrame()->addToJavaScriptWindowObject("navigatorplugins", navigatorplugins);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webshellParameters", webshellParameters);

	connect(wapp,SIGNAL(changeIcon(QIcon)),this,SIGNAL(changeIcon(QIcon)));
	connect(wnavigator,SIGNAL(close()),this,SIGNAL(close()));
	connect(this,SIGNAL(loadFinished(bool)),this,SLOT(updateTitle()));
	connect(this->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(updateJavaScriptObjects()));

	this->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this,SIGNAL(linkClicked(QUrl)),this,SLOT(handleRedirect(QUrl)));

	//Si il est défini, on affecte l'icône présent dans le fichier xml à la page principale
	ConfigManager &config = ConfigManager::Instance();
	QString icon = config.GetIcon();
	if(!icon.isEmpty())
		wapp->setProperty("icon",icon);

	m_WebCtrl = new MyNetworkAccessManager();
	this->page()->setNetworkAccessManager(m_WebCtrl);

	connectionLost = false;

	connect(parent,SIGNAL(clearCookies()),m_WebCtrl,SLOT(clearCookies()));

	//On fait un test de connexion régulièrement sachant que le signal de perte de connexion n'est parfois pas émis
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateConnectivity()));
	timer->start(5000);

	if(!wapp->IsPageInApplication(config.GetLaunchUrl()))
	{
		QStringList baseUrl = QStringList() << config.GetLaunchUrl() << config.GetBaseUrl();
		wapp->setProperty("baseUrl",baseUrl);
	}
}

/**
 * @brief MyWebView::~MyWebView Destructeur de la page et des objets intégrés au moteur JavaScript
 */
MyWebView::~MyWebView()
{
	timer->stop();
	delete timer;
	delete wnavigator;
	delete wapp;
	delete navigatorplugins;
}

/**
 * @brief Place la première page chargée dans baseUrl, puis renvoie vers le navigateur les url externes à baseUrl
 * @param url   Url chargée
 */
void MyWebView::handleRedirect(const QUrl &url)
{
	if(!wapp->IsPageInApplication(url.url()))
			QDesktopServices::openUrl(url);
	else
		this->load(url);
}

/**
 * @brief Envoie un signal pour mettre à jour le titre de la MainWindow
 */
void MyWebView::updateTitle()
{
	emit changeTitle(this->title());
}

/**
 * @brief Remet les objets JavaScript sur la page lorsqu'ils sont supprimés
 */
void MyWebView::updateJavaScriptObjects()
{
	this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
	this->page()->mainFrame()->addToJavaScriptWindowObject("navigatorplugins", navigatorplugins);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webshellParameters", webshellParameters);
}

/**
 * @brief Si la connexion est perdue, on sauvegarde la page courante et on affiche la page de perte de connectivité en local\n
 * Lorsque la connexion est retrouvée, on charge la page sauvegardée et on la remplace par une chaîne vide dans le ConfigManager
 */
void MyWebView::updateConnectivity()
{
	if(m_WebCtrl->networkAccessible() == QNetworkAccessManager::NotAccessible && !connectionLost)
	{
		ConfigManager &config = ConfigManager::Instance();
		config.SetSavedAdress(this->url().toString());
		LoadInternalPage("disconnected");
		connectionLost = true;
	}
	if(m_WebCtrl->networkAccessible() == QNetworkAccessManager::Accessible && connectionLost)
	{
		QEventLoop loop;
		connect(this,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
		LoadInternalPage("loader");
		loop.exec();

		disconnect(this,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
		ConfigManager &config = ConfigManager::Instance();
		QString savedAdress(config.GetSavedAdress());
		if(savedAdress.isEmpty())
			return;
		this->load(savedAdress);
		config.SetSavedAdress("");
		connectionLost = false;
	}
}


/**
 * @brief Lance un événement depuis une cible particulière avec des clefs et valeurs spécifiques
 * @param evtType   Nom de l'événement
 * @param evtTarget Target de l'événement
 * @param keyValues Doublons clés/valeurs, si la liste ne contient pas un nombre pair d'éléments on a une erreur
 * @return
 */
bool MyWebView::DispatchJsEvent(const QString & evtType, const QString & evtTarget, const QStringList &keyValues)
{
	QString code =QString("var webshellEvent = new CustomEvent('%1');").arg(evtType);
	QString key;
	QString value;

	QStringList::const_iterator i;
	for(i = keyValues.begin(); i!= keyValues.end();++i)
	{
		key = *i;
		if(i!= keyValues.end())
		{
			++i;
		}
		else
		{
			qDebug() << "Nombre d'arguments invalide";
			return false;
		}
		value = *i;
		code.append(QString("webshellEvent.%1='%2';").arg(key,value));
	}

	code.append(evtTarget+".dispatchEvent(webshellEvent);");
	this->page()->mainFrame()->evaluateJavaScript(code);
	return true;
}

/**
 * @brief Charge une page interne dans la langue par défaut si c'est possible, en anglais sinon
 * @param page	Type de page à charger ("loader","disconnected")
 */
void MyWebView::LoadInternalPage(QString page)
{
	ConfigManager &config = ConfigManager::Instance();
	if(QFile::exists(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-"+config.GetLanguage()+".html")))
		this->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-"+config.GetLanguage()+".html")));
	else
		this->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-en"+".html")));
}
