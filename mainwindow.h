#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>
#include "parametres.h"
#include "mywebview.h"

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
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

    //L'attribut sera à placer dans configManager
    bool minimization;
    bool stayOpen;

private slots:
    void showContextMenu(const QPoint& pos);
    void changeScreenMode(bool fullscreen);
    void changeCloseButtonMode(bool minimization);
    void changeToolsMode(bool toolsActivated);
};

#endif
