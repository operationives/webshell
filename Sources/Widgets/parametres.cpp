#include "parametres.h"
#include "global.h"

/**
 * @brief Fenêtre de paramètres
 */
Parametres::Parametres(QWidget *parent)
	: QDialog(parent)
{

	//On enlève le bouton "?" inutile
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	this->setWindowIcon(QIcon(QApplication::applicationDirPath()+"/djanah.png"));

	tabWidget = new QTabWidget;
	Tab *closeOption = new Tab("Action du bouton de fermeture","Minimisation","Fermeture",config->GetCloseButtonBehaviour());
	Tab *tools = new Tab("Outils de développement","Activés","Désactivés",config->GetDeveloperToolsMode());
	tabWidget->addTab(closeOption, tr("Fermeture"));
	tabWidget->addTab(tools, tr("Outils"));

	connect(closeOption,SIGNAL(optionClicked(bool)),this,SIGNAL(closeButtonMode(bool)));
	connect(tools,SIGNAL(optionClicked(bool)),this,SIGNAL(toolsMode(bool)));

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Paramètres"));
}

/**
 * @brief Instancie l'onglet
 * @param title			Titre de l'onglet
 * @param button1Text	Titre de l'option 1
 * @param button2Text	Titre de l'option 2
 * @param buttonChecked	Si vrai, option 1 cochée, sinon, option 2 cochée
 * @param parent		Widget parent
 */
Tab::Tab(QString title, QString button1Text, QString button2Text, bool buttonChecked, QWidget *parent)
	: QWidget(parent)
{
	QGroupBox *group = new QGroupBox(title);

	QRadioButton *button1 = new QRadioButton(button1Text);
	QRadioButton *button2 = new QRadioButton(button2Text);

	if(buttonChecked)
			button1->setChecked(true);
	else
			button2->setChecked(true);

	//On connecte les signaux de clic aux slots envoyant les signaux optionxActivated
	connect(button1,SIGNAL(pressed()),this,SLOT(option1Clicked()));
	connect(button2,SIGNAL(pressed()),this,SLOT(option2Clicked()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(button1);
	layout->addWidget(button2);
	group->setLayout(layout);


	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(group);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

/**
 * @brief Indique que l'option 1 est choisie
 */
void Tab::option1Clicked()
{
	emit optionClicked(true);
}

/**
 * @brief Indique que l'option 2 est choisie
 */
void Tab::option2Clicked()
{
	emit optionClicked(false);
}
