#include "mywebview.h"

MyWebView::MyWebView(QWidget *parent) : QWebView(parent){
    this->baseUrl = "http://www.qt.io/";

    connect(this,SIGNAL(urlChanged(QUrl)),this,SLOT(handleRedirect(QUrl)));
}


/**
 * @brief MyWebView::handleRedirect    Redirige l'url dans la webview ou dans le navigateur en fonction de sa valeur
 * @param url   Url chargée
 */
void MyWebView::handleRedirect(QUrl url){
    if(!url.url().startsWith(this->baseUrl) && url.url().compare("file:///"+QApplication::applicationDirPath()+"/"+"index.html")!=0){
            this->page()->triggerAction(QWebPage::Back);
            QDesktopServices::openUrl(url);
    }
}
