#include <QWebFrame>
#include "mywebview.h"

MyWebView::MyWebView(QWidget *parent) : QWebView(parent){
    wnavigator = new WNavigator(qobject_cast<QWebView *>(this));
    wapp = new WebApp(qobject_cast<QWebView *>(this));
    wnavigatorplugins = new WNavigatorPlugins(qobject_cast<QWebView *>(this));
    //On permet l'accès aux méthodes dans WNavigator par les appels javascript
    this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
    this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);

    wapp->baseUrl->append(QUrl("http://www.qt.io/"));
    connect(wapp,SIGNAL(ChangeIcon(QIcon)),this,SIGNAL(ChangeIcon(QIcon)));

    connect(this,SIGNAL(urlChanged(QUrl)),this,SLOT(handleRedirect(QUrl)));
}

MyWebView::~MyWebView(){
    delete wnavigator;
    delete wapp;
    delete wnavigatorplugins;
}

/**
 * @brief MyWebView::handleRedirect    Redirige l'url dans la webview ou dans le navigateur en fonction de sa valeur
 * @param url   Url chargée
 */
void MyWebView::handleRedirect(QUrl url){
    if(!wapp->ispageInApplication() && url.url()!=QString("file:///"+QApplication::applicationDirPath()+"/"+"index.html")){
            this->page()->triggerAction(QWebPage::Back);
            QDesktopServices::openUrl(url);
    }
}
