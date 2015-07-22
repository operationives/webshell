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
    wnavigator = new WNavigator(this);
    wapp = new WebApp(this);
    navigatorplugins = new NavigatorPlugins(this);
    //On permet l'accès aux méthodes dans WNavigator par les appels javascript
    this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
    this->page()->mainFrame()->addToJavaScriptWindowObject("navigatorplugins", navigatorplugins);
    this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);
    this->page()->mainFrame()->addToJavaScriptWindowObject("webshellParameters", new WebshellParameters());

    connect(wapp,SIGNAL(changeIcon(QIcon)),this,SIGNAL(changeIcon(QIcon)));

    connect(this,SIGNAL(urlChanged(QUrl)),this,SLOT(handleRedirect(QUrl)));

    firstPage = true;

    wapp->setProperty("icon",config->GetIcon());
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
    if(firstPage)
    {
        //baseUrl peut être modifié à tout moment par le service
        if(!wapp->IsPageInApplication())
        {
            QStringList baseUrl = wapp->property("baseUrl").toStringList();
            baseUrl.append(url.url());
            wapp->setProperty("baseUrl",QVariant(baseUrl));
        }
        firstPage = false;
    }
    else if(!wapp->IsPageInApplication())
    {
            this->page()->triggerAction(QWebPage::Back);
            QDesktopServices::openUrl(url);
    }
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
    QString code =QString("var webshellEvent = new Event('%1');").arg(evtType);
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
