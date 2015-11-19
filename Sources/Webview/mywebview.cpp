#include <QWebFrame>
#include <QWebElementCollection>
#include "mywebview.h"
#include "mywebpage.h"
#include "wnavigator.h"
#include "navigatorplugins.h"
#include "webapp.h"
#include "mynetworkaccessmanager.h"
#include "Outils/configmanager.h"

/**
 * @brief Constructeur de notre webview et des objets à intégrer dans l'application
 * @param parent	Widget parent, dans le cas présent il s'agit de la MainWindow
 */
MyWebView::MyWebView(QWidget *parent) : QWebView(parent)
{
    this->setPage(new MyWebPage(this));
    //On enlève les barres de défilement inutiles dans le cadre du webshell
    this->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
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
    connect(this,SIGNAL(loadFinished(bool)),this,SLOT(updateLogin()));
	connect(this->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(updateJavaScriptObjects()));

	//Si il est défini, on affecte l'icône présent dans le fichier xml à la page principale
	ConfigManager &config = ConfigManager::Instance();
	QString icon = config.GetIcon();
	if(!icon.isEmpty())
		wapp->setProperty("icon",icon);

	MyNetworkAccessManager *m_WebCtrl = MyNetworkAccessManager::Instance();
	this->page()->setNetworkAccessManager(m_WebCtrl);

	connectionLost = false;

	connect(parent,SIGNAL(clearAll()),m_WebCtrl,SLOT(clearAll()));

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
 * @brief Envoie un signal pour mettre à jour le titre de la MainWindow
 */
void MyWebView::updateTitle()
{
	emit changeTitle(this->title());
}

/**
 * @brief Renseigne le dernier login utilisé et affiche un menu déroulant avec les logins utilisés
 */
void MyWebView::updateLogin()
{
    QWebFrame* frame = this->page()->currentFrame();
    QWebElementCollection login_button = frame->findAllElements("input[name=loginbutton]");
    if (login_button.count() != 0)
    {
        qDebug() << "Login page detected";
        QWebElement user_auth_field = frame->findFirstElement("input[name=email]");
        QWebElement pwd_auth_field = frame->findFirstElement("input[name=password]");
        if (!user_auth_field.isNull())
        {
            ConfigManager &config = ConfigManager::Instance();
            QStringList login_list(config.GetLoginList());
            QString last_login = config.GetLastLogin();

            if (!last_login.isEmpty())
            {
                qDebug() << "A login already exists: show it. Last login: " << last_login;
                user_auth_field.setAttribute("value", last_login);
                pwd_auth_field.setAttribute("value", "");
            }

            if (login_list.size()>1)
            {
                QString onfocus_attribute;

                onfocus_attribute = user_auth_field.attribute("onfocus");
                onfocus_attribute.append("this.previousElementSibling.selectedIndex = 0;");
                user_auth_field.setAttribute("onfocus",onfocus_attribute);

                // Reduce the width of the login field:
                int arrow_width = 35;
                QString width_css_property = user_auth_field.styleProperty("width",QWebElement::ComputedStyle);
                width_css_property.replace(QString("px"),QString(""));
                bool ok;
                int new_width = width_css_property.toInt(&ok,10)-arrow_width;
                QString new_width_css_property = QString::number(new_width).append("px");
                user_auth_field.setStyleProperty("width",QString(new_width_css_property));

                // Add the selector with all stored logins:
                QString html;
                html.append("<select class='ws_loginfield' onchange='if (this.selectedIndex != 0){ this.nextElementSibling.value=this.value; this.nextElementSibling.nextElementSibling.focus(); this.selectedIndex = 0;}'>");
                html.append("<option></option>");
                for (int i = 0; i < login_list.size(); ++i)
                {
                   html.append("<option>");
                   html.append(login_list.at(i));
                   html.append("</option>");
                }
                html.append("</select>");
                user_auth_field.prependOutside(html);
            }
        }
    }
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
	MyNetworkAccessManager *m_WebCtrl = MyNetworkAccessManager::Instance();
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
	if(QFile::exists(QString(QApplication::applicationDirPath()+"/" + page + "-"+config.GetLanguage()+".html")))
		this->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-"+config.GetLanguage()+".html")));
	else
		this->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-en"+".html")));
}
