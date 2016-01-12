#include "mywebpage.h"
#include "mywebview.h"
#include "webapp.h"
#include "Outils/configmanager.h"
#include <QNetworkRequest>
#include <QWebSecurityOrigin>

bool MyWebPage::acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, NavigationType type)
{
    MyWebView *view;
    QStringList local_scheme_list;
    bool is_authorized_scheme   = false;
    bool is_authorized_url      = false;
    QString url_scheme = "";
    QString url = request.url().url();

    url_scheme = url.left(url.indexOf(":"));
    local_scheme_list = QWebSecurityOrigin::localSchemes();

    if (QString::compare(url_scheme,"about") == 0)
        return false;

    for (int i = 0 ; i <  local_scheme_list.length() ; i++)
    {
        if (QString::compare(url_scheme,local_scheme_list.at(i)) == 0)
        {
            is_authorized_scheme = true;
            break;
        }
    }

    /*if (type == QWebPage::NavigationTypeFormSubmitted)
    {
        //TODO: handle GET request cancellation after POST request
    }*/

    view = (MyWebView* )this->parent();
    is_authorized_url = view->getWebApp()->IsPageInApplication(request.url());

    if( is_authorized_url || is_authorized_scheme)
    {
        return QWebPage::acceptNavigationRequest(frame,request,type);
    }
    else
    {
        ConfigManager &config = ConfigManager::Instance();
        qDebug() << "Navigation request rejected. Open it in a browser. url: " << request.url();
        qDebug() << "> Base url(s): " << config.GetBaseUrl();
        QString str_sheme_list = "";
        for (int i = 0 ; i <  local_scheme_list.length() ; i++)
        {
            str_sheme_list.append(local_scheme_list.at(i));
            if (i != local_scheme_list.length()-1) str_sheme_list.append(" | ");
        }
        qDebug() << "> Authorized local schemes: " << str_sheme_list;
        qDebug() << "> If necessary authorize additional local schemes for the App";
        QDesktopServices::openUrl(request.url());
        return false;
    }
}

/**
 * @brief Redéfinition de la popup de confirmation javascript
 * @param frame	Fenêtre depuis laquelle le programme javascript est en cours
 * @param msg	Message à afficher
 */
bool MyWebPage::javaScriptConfirm(QWebFrame * frame, const QString & msg)
{
    QMessageBox messageBox;
    QPushButton *okButton = messageBox.addButton(QMessageBox::Ok);
    QPushButton *cancelButton = messageBox.addButton(QMessageBox::Cancel);
    messageBox.setIcon(QMessageBox::Question);
    if(ConfigManager::Instance().GetLanguage() == FR)
        messageBox.setWindowTitle(QObject::tr("Demande de confirmation"));
    else
        messageBox.setWindowTitle(QObject::tr("Confirmation"));
    messageBox.setText("\n" + msg + "\n");
    messageBox.setDefaultButton(okButton);
    messageBox.exec();

    if (messageBox.clickedButton() == okButton)
    {
        return true;
    }
    else if (messageBox.clickedButton() == cancelButton)
    {
        return false;
    }
}

/**
 * @brief Redéfinition de la popup d'alert javascript
 * @param frame	Fenêtre depuis laquelle le programme javascript est en cours
 * @param msg	Message à afficher
 */
void MyWebPage::javaScriptAlert(QWebFrame * frame, const QString & msg)
{
    QMessageBox messageBox;
    QPushButton *okButton = messageBox.addButton(QMessageBox::Ok);
    messageBox.setIcon(QMessageBox::Information);
    if(ConfigManager::Instance().GetLanguage() == FR)
        messageBox.setWindowTitle(QObject::tr("Information"));
    else
        messageBox.setWindowTitle(QObject::tr("Information"));
    messageBox.setText("\n" + msg + "\n");
    messageBox.setDefaultButton(okButton);
    messageBox.exec();
}
