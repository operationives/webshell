#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>
#include "parametres.h"
#include "mywebview.h"
#include "configmanager.h"
#include "winaddon.h"

class MainWindow : public QMainWindow{

    Q_OBJECT

public:
    MainWindow(const QUrl& url);
    ~MainWindow();
    void evaluateJavaScript(QString code);

private:
    MyWebView *view;
    QSystemTrayIcon *trayIcon;
    QWebInspector *i;
    Parametres *params;
    ConfigManager *config;
    bool stayOpen;
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void showContextMenu(const QPoint& pos);
    void changeScreenMode(bool fullscreen);
    void changeCloseButtonMode(bool minimization);
    void changeToolsMode(bool toolsActivated);
    void quit();
    void ChangeIcon(QIcon icon);
};

#endif
