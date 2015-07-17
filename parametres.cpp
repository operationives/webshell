#include "parametres.h"

/**
 * @brief Parametres::Parametres    Fenêtre de paramètres
 * @param fullscreen        Vrai si la page est en plein écran
 * @param minimization      Vrai si le bouton de fermeture est associé à la minimisation
 * @param toolsactivated    Vrai si les outils développeur sont activés
 * @param parent            Widget parent
 */
Parametres::Parametres(bool minimization, bool toolsactivated, QWidget *parent) : QDialog(parent)
{

    //On enlève le bouton "?" inutile
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    this->setWindowIcon(QIcon(QApplication::applicationDirPath()+"/djanah.png"));

    tabWidget = new QTabWidget;
//    Tab *screen = new Tab("Dimensions de l'écran","Plein écran","Fenêtré",fullscreen);
    Tab *closeOption = new Tab("Action du bouton de fermeture","Minimisation","Fermeture",minimization);
    Tab *tools = new Tab("Outils de développement","Activés","Désactivés",toolsactivated);
//    tabWidget->addTab(screen, tr("Ecran"));
    tabWidget->addTab(closeOption, tr("Fermeture"));
    tabWidget->addTab(tools, tr("Outils"));

//    connect(screen,SIGNAL(OptionClicked(bool)),this,SIGNAL(screenMode(bool)));
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
 * @brief Tab::Tab      Instancie l'onglet
 * @param title         Titre de l'onglet
 * @param button1Text   Titre de l'option 1
 * @param button2Text   Titre de l'option 2
 * @param buttonChecked Si vrai, option 1 cochée, sinon, option 2 cochée
 * @param parent        Widget parent
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
 * @brief Tab::option1Clicked   Indique que l'option 1 est choisie
 */
void Tab::option1Clicked()
{
    emit optionClicked(true);
}

/**
 * @brief Tab::option2Clicked   Indique que l'option 2 est choisie
 */
void Tab::option2Clicked()
{
    emit optionClicked(false);
}
