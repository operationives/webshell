#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>
#include "parametres.h"
#include "mywebview.h"
#include "winaddon.h"
#include "informations.h"

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
    Informations *infos;
    bool stayOpen;
    void DisplayInfos();
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void showContextMenu(const QPoint& pos);
    void changeScreenMode(bool fullscreen);
    void changeToolsMode(bool toolsActivated);
    void quit();
    void changeIcon(QIcon icon);
};

#endif
