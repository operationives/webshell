#include "informations.h"
#include "global.h"

Informations::Informations(QWidget *parent)
    : QDialog(parent)
{
    //On enlève le bouton "?" inutile
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    this->setWindowIcon(QIcon(QApplication::applicationDirPath()+"/djanah.png"));

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

void Informations::UpdateValues()
{
    version = config->GetVersion();
    infosAppli = config->GetInfos();
    //if(manager->networkAccessible() == QNetworkAccessManager::NetworkAccessibility::Accessible)
        label->setText(QString("Version du webshell:\n" + version + "\nInformations complémentaires:\n" + infosAppli));
    //else
    //    label->setText(QString("Version du webshell:\n" + version + "\nInformations complémentaires:\nL'application est déconnectée"));
    //if(manager->networkAccessible() == QNetworkAccessManager::NetworkAccessibility::Accessible)
    //    lineEdit->setText(QString("Version du webshell:\n%1\nInformations complémentaires:\n%2").arg(version,infosAppli));
    //else
    //    lineEdit->setText(QString("Version du webshell:\n%1\nInformations complémentaires:\nL'application n'est pas connectée").arg(version));
}