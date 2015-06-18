#include "mainwindow.h"
#include "updateoperations.h"
#include "global.h"

MainWindow::MainWindow(const QUrl& url){
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    //Les settings initiaux permettent d'autoriser les npapi plugins, javascript, et la console javascript (clic droit->inspect)
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    view = new QWebView(this);
    //On permet l'accès aux méthodes dans UdpdateOperations par les appels javascript
    view->page()->mainFrame()->addToJavaScriptWindowObject("updateOperations", new UdpdateOperations);
    view->load(url);
    //Il faudra voir quelle taille de fenêtre minimale est autorisée
//    view->setMinimumSize(int width,int height);

    //On enlève les barres de défilement inutiles dans le cadre de la webshell
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    ctrl = new Controleur(this);

    //Test liste plugins utilisables
//    int length = view->page()->mainFrame()->evaluateJavaScript("navigator.plugins.length").toInt();
//    for(int i = 0; i < length; i++) {
//        QString version = view->page()->mainFrame()->evaluateJavaScript(QString("navigator.plugins[%1].description").arg(i)).toString();

//        QRegExp rx("\\d*");
//        QStringList list;
//        int pos = 0;

//        while ((pos = rx.indexIn(version, pos)) != -1) {
//            list << rx.cap(1);
//            pos += rx.matchedLength();
//        }
//        // list: ["12", "14", "99", "231", "7"]
//      qDebug() << view->page()->mainFrame()->evaluateJavaScript(QString("navigator.plugins[%1].name").arg(i)).toString()
//        << " | " << view->page()->mainFrame()->evaluateJavaScript(QString("navigator.plugins[%1].filename").arg(i)).toString()
//        << " | " << view->page()->mainFrame()->evaluateJavaScript(QString("window.navigator.plugins[%1].description").arg(i)).toString()
//        << " | " << list[1];
//    }

    //Affichage du UserAgent
//    qDebug() << this->view->page()->mainFrame()->evaluateJavaScript("navigator.userAgent").toString();

}

MainWindow::~MainWindow(){
    delete view;
}

void MainWindow::evaluateJavaScript(QString code){
    view->page()->mainFrame()->evaluateJavaScript(code);
}
