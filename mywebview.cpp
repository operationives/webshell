#include <QWebFrame>
#include "mywebview.h"
#include "global.h"

/**
 * @brief MyWebView::MyWebView  Constructeur de notre webview et des objets à intégrer dans l'application
 * @param parent
 */
MyWebView::MyWebView(QWidget *parent) : QWebView(parent)
{
    wnavigator = new WNavigator(qobject_cast<QWebView *>(this));
    wapp = new WebApp(qobject_cast<QWebView *>(this));
    wnavigatorplugins = new WNavigatorPlugins(qobject_cast<QWebView *>(this));
    //On permet l'accès aux méthodes dans WNavigator par les appels javascript
    this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
    this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigatorplugins", wnavigatorplugins);
    this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);

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
    delete wnavigatorplugins;
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
            qDebug() << "On insère l'url";
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
