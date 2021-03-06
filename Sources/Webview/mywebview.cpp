#include <QWebFrame>
#include <QWebElementCollection>
#include "mywebview.h"
#include "mywebpage.h"
#include "wnavigator.h"
#include "navigatorplugins.h"
#include "webapp.h"
#include "mynetworkaccessmanager.h"
#include "Outils/configmanager.h"


/**
 * @brief Constructeur de notre webview et des objets à intégrer dans l'application
 * @param parent	Widget parent, dans le cas présent il s'agit de la MainWindow
 */
MyWebView::MyWebView(QWidget *parent) : QWebView(parent)
{
    m_current_popup = NULL;

    this->setPage(new MyWebPage(this));
    this->page()->setForwardUnsupportedContent(true);
    //On enlève les barres de défilement inutiles dans le cadre du webshell
    this->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	this->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

	navigatorplugins = new NavigatorPlugins(this);
	wapp = new WebApp(this);
	webshellParameters = new WebshellParameters();
	wnavigator = new WNavigator(this,webshellParameters);
	//On permet l'accès aux méthodes dans WNavigator par les appels javascript
	this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
	this->page()->mainFrame()->addToJavaScriptWindowObject("navigatorplugins", navigatorplugins);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webshellParameters", webshellParameters);

	connect(wapp,SIGNAL(changeIcon(QIcon)),this,SIGNAL(changeIcon(QIcon)));
	connect(wnavigator,SIGNAL(close()),this,SIGNAL(close()));
	connect(this,SIGNAL(loadFinished(bool)),this,SLOT(updateTitle()));
	connect(this->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(updateJavaScriptObjects()));
    connect(this->page(),SIGNAL(unsupportedContent(QNetworkReply*)),this,SLOT(downloadContent(QNetworkReply*)));

	//Si il est défini, on affecte l'icône présent dans le fichier xml à la page principale
	ConfigManager &config = ConfigManager::Instance();
	QString icon = config.GetIcon();
	if(!icon.isEmpty())
		wapp->setProperty("icon",icon);

    MyNetworkAccessManager *m_WebCtrl = MyNetworkAccessManager::Instance();
	this->page()->setNetworkAccessManager(m_WebCtrl);
    connect(m_WebCtrl,SIGNAL(finished(QNetworkReply*)),this,SLOT(handleNetworkAccess(QNetworkReply*)));
    connect(m_WebCtrl,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
	connectionLost = false;

	connect(parent,SIGNAL(clearAll()),m_WebCtrl,SLOT(clearAll()));

	//On fait un test de connexion régulièrement sachant que le signal de perte de connexion n'est parfois pas émis
    /*timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateConnectivity()));
    timer->start(5000);*/

	if(!wapp->IsPageInApplication(config.GetLaunchUrl()))
	{
		QStringList baseUrl = QStringList() << config.GetLaunchUrl() << config.GetBaseUrl();
		wapp->setProperty("baseUrl",baseUrl);
	}

    isUpdating = false;
}

/**
 * @brief MyWebView::~MyWebView Destructeur de la page et des objets intégrés au moteur JavaScript
 */
MyWebView::~MyWebView()
{
    //timer->stop();
    //delete timer;
	delete wnavigator;
	delete wapp;
	delete navigatorplugins;
    qDeleteAll(m_downloads.begin(), m_downloads.end());
    m_downloads.clear();
    if (m_current_popup) m_current_popup->close();
}

/**
 * @brief Envoie un signal pour mettre à jour le titre de la MainWindow
 */
void MyWebView::updateTitle()
{
	emit changeTitle(this->title());
}

/**
 * @brief Renseigne le dernier login utilisé et affiche un menu déroulant avec les logins utilisés
 */
void MyWebView::updateLogin()
{
    QMultiMap<QString, QString> map;
    QMap<QString, QString>::iterator it;
    QWebFrame* frame = this->page()->currentFrame();
    map = frame->metaData();
    for (it = map.begin(); it != map.end(); ++it) {
        if (it.value() == "he" || it.value() == "ar")
        {
            qDebug() << "He or Ar language detected via metadata";
            ConfigManager::Instance().SetLanguage(it.value());
        }
    }

    QWebElementCollection login_button = frame->findAllElements("input[name=loginbutton]");
    if (login_button.count() != 0)
    {
        qDebug() << "Login page detected";
        QWebElement user_auth_field = frame->findFirstElement("input[name=email]");
        QWebElement pwd_auth_field = frame->findFirstElement("input[name=password]");
        if (!user_auth_field.isNull())
        {
            ConfigManager &config = ConfigManager::Instance();
            QStringList login_list(config.GetLoginList());
            QString last_login = config.GetLastLogin();

            if (!last_login.isEmpty())
            {
                qDebug() << "A login already exists: show it. Last login: " << last_login;
                user_auth_field.setAttribute("value", last_login);
                pwd_auth_field.setAttribute("value", "");
            }

            if (login_list.size()>1)
            {
                QString onfocus_attribute;

                onfocus_attribute = user_auth_field.attribute("onfocus");
                onfocus_attribute.append("this.previousElementSibling.selectedIndex = 0;");
                user_auth_field.setAttribute("onfocus",onfocus_attribute);

                // Reduce the width of the login field:
                // FIXME: don't change the size: it's done by the service!
                int arrow_width = 35;
                QString width_css_property = user_auth_field.styleProperty("width",QWebElement::ComputedStyle);
                width_css_property.replace(QString("px"),QString(""));
                bool ok;
                int new_width = width_css_property.toInt(&ok,10)-arrow_width;
                QString new_width_css_property = QString::number(new_width).append("px");
                user_auth_field.setStyleProperty("width",QString(new_width_css_property));

                // Add the selector with all stored logins:
                QString html;
                html.append("<select class='ws_loginfield' onchange='if (this.selectedIndex != 0){ this.nextElementSibling.value=this.value; this.nextElementSibling.nextElementSibling.focus(); this.selectedIndex = 0;}'>");
                html.append("<option></option>");
                for (int i = 0; i < login_list.size(); ++i)
                {
                   html.append("<option>");
                   html.append(login_list.at(i));
                   html.append("</option>");
                }
                html.append("</select>");
                user_auth_field.prependOutside(html);
            }
        }
    }
}

/**
 * @brief Remet les objets JavaScript sur la page lorsqu'ils sont supprimés
 */
void MyWebView::updateJavaScriptObjects()
{
	this->page()->mainFrame()->addToJavaScriptWindowObject("wnavigator", wnavigator);
	this->page()->mainFrame()->addToJavaScriptWindowObject("navigatorplugins", navigatorplugins);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webapp", wapp);
	this->page()->mainFrame()->addToJavaScriptWindowObject("webshellParameters", webshellParameters);
}

/**
 * @brief Si la connexion est perdue, on sauvegarde la page courante et on affiche la page de perte de connectivité en local\n
 * Lorsque la connexion est retrouvée, on charge la page sauvegardée et on la remplace par une chaîne vide dans le ConfigManager
 */
void MyWebView::updateConnectivity()
{
    MyNetworkAccessManager *m_WebCtrl = MyNetworkAccessManager::Instance();
	if(m_WebCtrl->networkAccessible() == QNetworkAccessManager::NotAccessible && !connectionLost)
	{
        qDebug() << "Network is not accessible";
        ConfigManager &config = ConfigManager::Instance();
		config.SetSavedAdress(this->url().toString());
        //LoadInternalPage("disconnected");
		connectionLost = true;
	}
	if(m_WebCtrl->networkAccessible() == QNetworkAccessManager::Accessible && connectionLost)
	{
        qDebug() << "Network is accessible. Reload the saved page";
		QEventLoop loop;
		connect(this,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
		LoadInternalPage("loader");
		loop.exec();

		disconnect(this,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
		ConfigManager &config = ConfigManager::Instance();
		QString savedAdress(config.GetSavedAdress());
		if(savedAdress.isEmpty())
			return;
        this->load(savedAdress);
		config.SetSavedAdress("");
		connectionLost = false;
	}
}


/**
 * @brief Lance un événement depuis une cible particulière avec des clefs et valeurs spécifiques
 * @param evtType   Nom de l'événement
 * @param evtTarget Target de l'événement
 * @param keyValues Doublons clés/valeurs, si la liste ne contient pas un nombre pair d'éléments on a une erreur
 * @return
 */
bool MyWebView::DispatchJsEvent(const QString & evtType, const QString & evtTarget, const QStringList &keyValues)
{
	QString code =QString("var webshellEvent = new CustomEvent('%1');").arg(evtType);
	QString key;
	QString value;

	QStringList::const_iterator i;
	for(i = keyValues.begin(); i!= keyValues.end();++i)
	{
		key = *i;
		if(i!= keyValues.end())
		{
			++i;
		}
		else
		{
			qDebug() << "Nombre d'arguments invalide";
			return false;
		}
		value = *i;
		code.append(QString("webshellEvent.%1='%2';").arg(key,value));
	}

	code.append(evtTarget+".dispatchEvent(webshellEvent);");
	this->page()->mainFrame()->evaluateJavaScript(code);
	return true;
}

/**
 * @brief Charge une page interne dans la langue par défaut si c'est possible, en anglais sinon
 * @param page	Type de page à charger ("loader","disconnected")
 */
void MyWebView::LoadInternalPage(QString page)
{
	ConfigManager &config = ConfigManager::Instance();
	if(QFile::exists(QString(QApplication::applicationDirPath()+"/" + page + "-"+config.GetLanguage()+".html")))
        this->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-"+config.GetLanguage()+".html")));
    else
		this->load(QUrl(QString("file:///"+QApplication::applicationDirPath()+"/" + page + "-en"+".html")));
}

void MyWebView::mousePressEvent(QMouseEvent * ev)
{
    QWebView::mousePressEvent(ev);
    emit mousePressed();
}

void MyWebView::downloadContent(QNetworkReply *reply)
{
    this->stop();
    qDebug() << "Download unsupported content: [" << reply->url().url() << "]";
    if(!reply->rawHeader("Content-Type").isEmpty()) {
        qDebug() << "> MIME type: [" << reply->rawHeader("Content-Type") << "]";
    }

    /*QList<QByteArray>   h = reply->rawHeaderList();
    foreach(QByteArray x,h) {
        qDebug() << x << "\t" << reply->rawHeader(x);
    }*/

    DownloadItem *unsupported_item = new DownloadItem(reply, this);
    m_downloads.append(unsupported_item);
    //connect(unsupported_item, SIGNAL(downloadFinished(DownloadItem *)), this, SLOT(downloadFinished(DownloadItem *)));
    connect(unsupported_item, SIGNAL(downloadFinished(DownloadItem *)), this->parent(), SLOT(handleDownloadFinished()));

    QString caption;
    if(ConfigManager::Instance().GetLanguage() == FR)
        caption = "Sauvegarder la capture d'écran";
    else
        caption = "Save the screenshot";

    QString fileName = reply->url().url().split("/").last();
    QString fileNameExtension = fileName.split(".").last();
    QString dir = "";
    QString filter = "*." ;

    if (fileNameExtension.contains(QRegularExpression("(jpg|png|gif|bmp)")))
    {
        dir.append(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).at(0)+"/");
        if(ConfigManager::Instance().GetLanguage() == FR)
        {
            dir.append("capture");
            //dir.append(QDateTime::currentDateTime().toString("_yyyy.MM.dd_hh.mm.ss"));
        }
        else
        {
            dir.append("screenshot");
            //dir.append(QDateTime::currentDateTime().toString("_yyyy.MM.dd_hh.mm.ss"));
        }
        dir.append(".");
        dir.append(fileNameExtension);
    }
    else
    {
        dir.append(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).at(0)+"/");
        dir.append(fileName);
    }

    filter.append(fileNameExtension);

    QString pictureFileName = QFileDialog::getSaveFileName(this, caption, dir , filter);

    if (!pictureFileName.isEmpty() && !pictureFileName.isNull())
        unsupported_item->setUserFileName(pictureFileName);
    else
    {
        unsupported_item->abortDownload();
        //emit unsupported_item->downloadFinished(unsupported_item);
        //m_downloads.removeOne(unsupported_item);
    }

}

QWebView *MyWebView::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);

    if (m_current_popup)
    {
        m_current_popup->close();
        delete m_current_popup;
        m_current_popup = NULL;
    }

    QWebView *web_view = new QWebView();
    QWebPage *new_webPage = new QWebPage(web_view);
    m_current_popup = web_view;
    connect(web_view, SIGNAL(destroyed()), this, SLOT(handleClosePopup()));
    web_view->setAttribute(Qt::WA_DeleteOnClose, true);
    web_view->setPage(new_webPage);
    web_view->resize(QSize(400,400));
    web_view->setWindowTitle(ConfigManager::Instance().GetDisplayName());
    web_view->setWindowFlags(Qt::WindowStaysOnTopHint);
    web_view->show();

    return web_view;
}

void MyWebView::handleClosePopup()
{
    m_current_popup = NULL;
}

void MyWebView::handleSslErrors(QNetworkReply* reply,QList<QSslError> errors)
{
    qDebug() << "SSL errors: ";
    foreach(QSslError error, errors) {
        qDebug() << error.errorString();
    }
    // Uncomment the following line to ignore SSL errors:
    //reply->ignoreSslErrors();
}


void MyWebView::handleNetworkAccess(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 407) // Proxy Authentification required
    {
        qDebug() << "Proxy Authentification required";
    }
}


/*void MyWebView::downloadFinished(DownloadItem* item)
{
    if (item == NULL) return;

    if (ConfigManager::Instance().GetDeveloperToolsMode())
        qDebug() << "MyWebView::" << __FUNCTION__ << "Download Finished: remove download item";
    m_downloads.removeOne(item);
}*/


/*
 * DOWNLOAD ITEM:
 * */

DownloadItem::DownloadItem(QNetworkReply *reply, QWidget *parent)
    : QWidget(parent)
    , m_reply(reply)
{
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));
    connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this->parent()->parent(), SLOT(handleDownloadProgress(qint64, qint64)));
    //connect(m_reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
    //connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));

    m_data = NULL;
    m_fileIsSaved = false;
    m_userFileName = "";
}

void DownloadItem::finished()
{
    if(m_reply)
    {
        switch(m_reply->error())
        {
            case QNetworkReply::NoError:
                qDebug() << "Download Finished!";
                if (m_userFileName.isEmpty())
                {
                    m_data = m_reply->readAll();
                }
                else
                {
                    QFile   file(m_userFileName);
                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray data = m_reply->readAll();
                        qDebug() << "Written " <<  file.write(data) << " BYTES";
                        file.close();
                    }
                    m_fileIsSaved = true;
                    emit downloadFinished(this);
                    disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this->parent()->parent(), SLOT(handleDownloadProgress(qint64, qint64)));
                    m_reply->deleteLater();
                }
            break;
        }

    }
}

void DownloadItem::error(QNetworkReply::NetworkError)
{
    qWarning() << "DownloadItem::" << __FUNCTION__ << m_reply->errorString();
    emit downloadFinished(this);
}

void DownloadItem::setUserFileName(QString p_file_name)
{
    m_userFileName = p_file_name;
    if (!m_data.isNull() && !m_fileIsSaved)
    {
        QFile   file(m_userFileName);
        if(file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Written " <<  file.write(m_data) << " BYTES";
            file.close();
        }
        m_data.clear();
        emit downloadFinished(this);
        disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this->parent()->parent(), SLOT(handleDownloadProgress(qint64, qint64)));
        m_reply->deleteLater();
    }
}

void DownloadItem::abortDownload()
{
    emit downloadFinished(this);
    disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this->parent()->parent(), SLOT(handleDownloadProgress(qint64, qint64)));
}

