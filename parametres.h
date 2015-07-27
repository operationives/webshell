#ifndef PARAMETRES_H
#define PARAMETRES_H

#include <QDialog>
#include <QtWidgets>

class Tab : public QWidget
{
	Q_OBJECT

public:
	Tab(QString title, QString button1Text, QString button2Text, bool buttonChecked, QWidget *parent = 0);

signals:
	void optionClicked(bool value);

private slots:
	void option1Clicked();
	void option2Clicked();
};

class Parametres : public QDialog
{
	Q_OBJECT

public:
	Parametres(bool minimization, bool toolsactivated, QWidget *parent = 0);

private:
	QTabWidget *tabWidget;
	QDialogButtonBox *buttonBox;

signals:
	void closeButtonMode(bool minimization);
	void toolsMode(bool toolsActivated);
};

#endif
