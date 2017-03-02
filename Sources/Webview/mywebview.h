#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWidgets>
#include <QWebView>
#include "webshellparameters.h"
#include <QNetworkReply>
#include <QWinTaskbarProgress>
#include <QWinTaskbarButton>

class WNavigator;
class NavigatorPlugins;
class WebApp;

class DownloadItem : public QWidget
{
    Q_OBJECT

signals:
    void downloadFinished(DownloadItem *item);
    //void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

public:
    DownloadItem(QNetworkReply *reply = 0, QWidget *parent = 0);

    void setUserFileName(QString p_file_name);
    void abortDownload();
    //bool downloading() const;
    //bool downloadedSuccessfully() const;

    //qint64 bytesTotal() const;
    //qint64 bytesReceived() const;
    //double remainingTime() const;
    //double currentSpeed() const;

    //QUrl m_url;



private slots:
    //void downloadReadyRead();
    void error(QNetworkReply::NetworkError code);
    //void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finished();

private:
    QFile m_output;
    QNetworkReply *m_reply;
    QString m_userFileName;
    QByteArray m_data;
    bool m_fileIsSaved;

};

class MyWebView : public QWebView
{

	Q_OBJECT

public:
	MyWebView(QWidget *parent = 0);
	~MyWebView();
	bool DispatchJsEvent(const QString & evtType, const QString & evtTarget, const QStringList &keyValues = QStringList());
	void LoadInternalPage(QString page);
    void SetUpdatingStatus(bool updatingStatus){isUpdating = updatingStatus;}
    bool IsUpdating() {return isUpdating;}
    WebApp* getWebApp() {return wapp;}
    QWebView *createWindow(QWebPage::WebWindowType type);

signals:
	void changeIcon(const QIcon &icon);
	void changeTitle(const QString &appName);
	void close();
    void mousePressed();

private:
    QTimer *timer;
	WNavigator *wnavigator;
	NavigatorPlugins *navigatorplugins;
	WebApp *wapp;
	WebshellParameters *webshellParameters;
	bool connectionLost;
    bool isUpdating;
    void mousePressEvent(QMouseEvent * ev);
    QList<DownloadItem*> m_downloads;
    QProgressBar * m_progressBar;
    QWinTaskbarProgress * m_winTaskBarProgress;
    QWebView *m_current_popup;

private slots:
	void updateTitle();
    void updateLogin();
	void updateJavaScriptObjects();
	void updateConnectivity();
    void downloadContent(QNetworkReply *reply);
    void handleClosePopup();
    void handleNetworkAccess(QNetworkReply* reply);
    //void downloadFinished(DownloadItem *item);
    /*void handleLoadProgress(int progress);
    void handleLoadFinished(bool ok);*/

};

#endif
