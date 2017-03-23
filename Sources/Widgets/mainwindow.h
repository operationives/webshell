#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>
#include "parametres.h"
#include "Webview/mywebview.h"
#include "informations.h"
#ifdef Q_OS_WIN
    #include <QtWinExtras>
#endif

// Class creted to reimplement CloseEvent of the Qdialog
class UrlDialog : public QDialog
{
    Q_OBJECT

signals:
    void dialogClosed();

public:
    UrlDialog(QWidget * parent=0, Qt::WindowFlags f = 0) : QDialog(parent,f) {}

    void closeEvent(QCloseEvent *event)
    {
        emit dialogClosed();
    };

private slots:

private:

};

class MainWindow : public QMainWindow
{

	Q_OBJECT

public:
	MainWindow(const QString &iconPath = NULL, QWidget *parent = 0);
	~MainWindow();

private:
	MyWebView *view;
    QSize oldSize;
    QTimer *refreshTimer;
    QTimer *stopRefreshTimer;
	QMenu *fileMenu;
	QAction *clearAllAction;
    QAction *clearPointUrlAction;
	QAction *quitAction;
	QAction *inspectAction;
	QAction *fullscreenAction;
	QAction *normalscreenAction;
	QAction *reloadAction;
	QAction *infoAction;
#ifdef Q_OS_WIN
	QAction *sendlogAction;
#endif

	QSystemTrayIcon *trayIcon;
	QWebInspector *inspector;
	Informations *infos;
    QProgressBar * m_progressBar;
#ifdef Q_OS_WIN
    QWinJumpList *jumplist;
    QWinJumpListCategory *tasks;
    QWinJumpListItem *quitItem;
    QWinTaskbarButton *m_taskbarButton;
    QWinTaskbarProgress * m_taskbarProgress;
#endif
    QNetworkConfigurationManager *m_networkConfigurationManager;
    QNetworkSession *m_session;
    QNetworkSession::State m_currentNetworkState;
    QNetworkSession::State m_lastNetworkState;
    QTranslator *m_translator;
    QSize m_windowSizeBeforeFullscreen;
    QLabel *m_loadingLabel;
    QLabel *m_notificationLabel;
    QWidget *m_notification;
    QMovie *m_loaderIcon;
    QTimer *m_loadingTimer;
    QTimer *m_checkInternetStatusTimer;
    QPropertyAnimation *m_showNotificationAnimation;
    QPropertyAnimation *m_hideNotificationAnimation;

	bool windowIconSpecified;
	bool stayOpen;
    bool m_is_started;
    bool m_is_everything_saved_before_exiting;

	void DisplayInfos();
	void CenterScreen();
	void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent* event);
    void changeEvent( QEvent* event );
    void showEvent(QShowEvent *event);
    void createNotification();

signals:
	void clearAll();

private slots:
	void showContextMenu(const QPoint& pos);
	void changeScreenMode(bool fullscreen);
	void changeToolsMode(bool toolsActivated);
	void changeMinSize(int minWidth, int minHeight);
	void changeDefaultSize(int defaultWidth, int defaultHeight);
	void changeActionNames(QString lang);
    void loadTranslator(QString lang);
	void quit();
	void changeIcon(const QIcon &icon);
    void loadFinished(bool ok);
    void forceGuiUpdate();
    void displayLoader();
    void stopForceGuiUpdate();
    void startForceGuiUpdate();
    void handleLoadProgress(int progress);
    void handleLoadFinished(bool ok);
    void handleDownloadFinished();
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void handleNetworkStateChanged(QNetworkSession::State);
    void hideNotification();
    void handleNetworkConfigurationUpdated();
    void checkNetworkConfiguration();
    void showClearAllNotification();
    void showClearPointUrlNotification();
    void showReportLogNotification();
    void clearPointUrl();
};

#endif
