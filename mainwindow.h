#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebView>
#include <QtWebKitWidgets>
#include <QtNetwork>
#include "parametres.h"
#include "mywebview.h"
#include "informations.h"

class MainWindow : public QMainWindow
{

	Q_OBJECT

public:
	MainWindow(const QString &iconPath = NULL, QWidget *parent = 0);
	~MainWindow();

private:
	MyWebView *view;
	QSystemTrayIcon *trayIcon;
	QWebInspector *inspector;
	Informations *infos;
	bool windowIconSpecified;
	bool stayOpen;
	void DisplayInfos();
	void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);

private slots:
	void showContextMenu(const QPoint& pos);
	void changeScreenMode(bool fullscreen);
	void changeToolsMode(bool toolsActivated);
	void changeMinSize(int minWidth, int minHeight);
	void changeDefaultSize(int defaultWidth, int defaultHeight);
	void quit();
	void changeIcon(QIcon icon);
};

#endif
