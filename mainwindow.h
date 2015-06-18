#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>

class MainWindow : public QMainWindow{

    Q_OBJECT

public:
    MainWindow(const QUrl& url);
    ~MainWindow();
    void evaluateJavaScript(QString code);
private:
    QWebView *view;
};

#endif
