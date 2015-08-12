#ifndef INFORMATIONS_H
#define INFORMATIONS_H

#include <QDialog>
#include <QtWidgets>
#include <QNetworkAccessManager>

class Informations : public QDialog
{
	Q_OBJECT

public:
	Informations(QWidget *parent = 0);
	~Informations();
	void UpdateValues();

private:
	QString version;
	QString infosAppli;
	QLabel *label;
	QDialogButtonBox *buttonBox;
};

#endif // INFORMATIONS_H

