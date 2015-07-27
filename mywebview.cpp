#include <QWebFrame>
#include "mywebview.h"
#include "wnavigator.h"
#include "navigatorplugins.h"
#include "webapp.h"
#include "webshellparameters.h"
#include "global.h"

/**
 * @brief MyWebView::MyWebView  Constructeur de notre webview et des objets à intégrer dans l'application
 * @param parent
 */
MyWebView::MyWebView(QWidget *parent) : QWebView(parent)
{
	navigatorplugins = new NavigatorPlugins(this);
	wapp = new WebApp(this);
	WebshellParameters *webshellParameters = new WebshellParameters();
	wnavigator = new WNavigator(this,webshellParameters);
	//On permet l'accès aux méthodes dans WNavigator par les appels javascript
	this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
	this->page()->mainFrame()->addToJavaScriptWindowObject("navigatorplugins", navigatorplugins);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webshellParameters", webshellParameters);

	connect(wapp,SIGNAL(changeIcon(QIcon)),this,SIGNAL(changeIcon(QIcon)));
	connect(wnavigator,SIGNAL(close()),this,SIGNAL(close()));
	connect(this,SIGNAL(loadFinished(bool)),this,SLOT(updateTitle()));

	this->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this,SIGNAL(linkClicked(QUrl)),this,SLOT(handleRedirect(QUrl)));

	//On affecte l'icône présent dans le fichier xml à la page principale
	wapp->setProperty("icon",config->GetIcon());

	m_WebCtrl = new QNetworkAccessManager();
	m_cookieJar = new CookieJar();
	m_WebCtrl->setCookieJar(m_cookieJar);
	this->page()->setNetworkAccessManager(m_WebCtrl);
}

/**
 * @brief MyWebView::~MyWebView Destructeur de la page et des objets de l'application intégrés
 */
MyWebView::~MyWebView()
{
	delete wnavigator;
	delete wapp;
	delete navigatorplugins;
}

/**
 * @brief MyWebView::handleRedirect Place la première page chargée dans baseUrl, puis renvoie vers le navigateur les url externes à baseUrl
 * @param url   Url chargée
 */
void MyWebView::handleRedirect(QUrl url)
{
	if(!wapp->IsPageInApplication(url.url()))
			QDesktopServices::openUrl(url);
	else
		this->load(url);
}

void MyWebView::updateTitle()
{
	emit changeTitle(this->title());
}

/**
 * @brief MyWebView::DispatchJsEvent Lance un événement depuis une cible particulière avec des clefs et valeurs spécifiques
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
