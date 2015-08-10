#include "informations.h"
#include "global.h"

/**
 * @brief Construit la fenêtre utilisée pour afficher le numéro de version du webshell et les informations liées à l'application
 * @param parent
 */
Informations::Informations(QWidget *parent)
	: QDialog(parent)
{
	//On enlève le bouton "?" inutile
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	label = new QLabel();

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(label);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);

	manager = new QNetworkAccessManager();

	setWindowTitle(tr("Informations"));
}

Informations::~Informations()
{
	delete label;
	delete buttonBox;
	delete manager;
}

/**
 * @brief Met à jour les valeurs de la fenêtre\n
 * Si l'utilisateur est connecté, les informations de l'application sont celles spécifiées par notre ConfigManager\n
 * Sinon on indique que l'application est déconnectée
 */
void Informations::UpdateValues()
{
	version = qApp->applicationVersion();
	infosAppli = config->GetInfos();
	QString webshellText;
	QString appText;
	QString disconnectedText;
	if(config->GetLanguage() == "fr")
	{
		webshellText = "Version du webshell:\n";
		appText = "\nInformations complémentaires:\n";
		disconnectedText = "L'application est déconnectée";
	}
	else
	{
		webshellText = "Webshell version:\n";
		appText = "\nAdditional informations:\n";
		disconnectedText = "The application is disconnected";
	}
	if(manager->networkAccessible() == QNetworkAccessManager::Accessible)
		label->setText(QString(webshellText + version + appText + infosAppli));
	else
		label->setText(QString(webshellText + version + appText + disconnectedText));
}
