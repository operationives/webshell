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

	bool windowIconSpecified;
	bool stayOpen;
	void DisplayInfos();
	void CenterScreen();
	void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent* event);
    void changeEvent( QEvent* event );
    void showEvent(QShowEvent *event);

signals:
	void clearAll();

private slots:
	void showContextMenu(const QPoint& pos);
	void changeScreenMode(bool fullscreen);
	void changeToolsMode(bool toolsActivated);
	void changeMinSize(int minWidth, int minHeight);
	void changeDefaultSize(int defaultWidth, int defaultHeight);
	void changeActionNames(QString lang);
	void quit();
	void changeIcon(const QIcon &icon);
    void loadFinished(bool ok);
    void forceGuiUpdate();
    void stopForceGuiUpdate();
    void startForceGuiUpdate();
    void handleLoadProgress(int progress);
    void handleLoadFinished(bool ok);
    void handleDownloadFinished();
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif
