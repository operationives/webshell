#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>
#include "parametres.h"
#include "mywebview.h"
#include "winaddon.h"

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    MyWebView *view;
    QSystemTrayIcon *trayIcon;
    QWebInspector *i;
    Parametres *params;
    bool stayOpen;
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void showContextMenu(const QPoint& pos);
    void changeScreenMode(bool fullscreen);
    void changeCloseButtonMode(bool minimization);
    void changeToolsMode(bool toolsActivated);
    void quit();
    void paramsWindow();
    void changeIcon(QIcon icon);
};

#endif
