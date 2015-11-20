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

    for (int i = 0 ; i <  local_scheme_list.length() ; i++)
    {
        if (QString::compare(url_scheme,local_scheme_list.at(i)) == 0)
        {
            is_authorized_scheme = true;
            break;
        }
    }

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
        qDebug() << "> Base url: " << config.GetBaseUrl();
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