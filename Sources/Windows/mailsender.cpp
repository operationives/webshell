#include <QDir>
#include <wchar.h>
#include "mailsender.h"

/**
 * @brief Construit l'objet permettant d'envoyer les mails avec pièces jointes
 */
MailSender::MailSender()
{
	m_hLib = LoadLibrary(QString("MAPI32.DLL").toStdWString().c_str());
}

/**
 * @brief Libère la librairie allouée
 */
MailSender::~MailSender()
{
	FreeLibrary(m_hLib);
}

/**
 * @brief Ajoute une pièce jointe
 * @param file	Chemin du fichier
 * @param name	Nom du fichier
 */
void MailSender::AddFile( QString file, QString name )
{
	attachment a;
	a.path = file;
	if (name.isNull())
	{
		//On recode PathFindFilePath inaccessible
		a.name = file;
		if(a.name.lastIndexOf('/') == a.name.length())
		   a.name.chop(1);
		a.name.remove(0,a.name.lastIndexOf('/'));
	}
	else
        a.name = name;
	m_Files.push_back(a);
}

/**
 * @brief Ouvre l'interface mail par défaut de l'utilisateur avec le mail de la classe
 * @param szSubject	Sujet du mail
 * @return true si tout s'est bien passé, false sinon
 */
bool MailSender::Send(QString szSubject)
{
	if (!m_hLib)
		return false;

	LPMAPISENDMAIL SendMail;
    SendMail = (LPMAPISENDMAIL) GetProcAddress(m_hLib, QString("MAPISendMail").toStdString().c_str());

	if (!SendMail)
		return false;

	std::vector<MapiFileDesc> filedesc;
	for (QVector<attachment>::const_iterator ii = m_Files.begin(); ii!=m_Files.end(); ii++)
	{
		MapiFileDesc fileDesc;
		ZeroMemory(&fileDesc, sizeof(fileDesc));
		fileDesc.nPosition = (ULONG)-1;
		fileDesc.lpszPathName = (LPSTR) QDir::toNativeSeparators(ii->path + "/" + ii->name).toStdString().c_str();
		fileDesc.lpszFileName = NULL;
		filedesc.push_back(fileDesc);
	}

	QString subject;
	if (!szSubject.isNull())
		subject = szSubject;
	else
	{
		for (QVector<attachment>::const_iterator ii = m_Files.begin(); ii!=m_Files.end(); ii++)
		{
			subject += ii->name;
			if (ii+1 != m_Files.end())
				subject += ", ";
		}
	}

	MapiMessage message;
	ZeroMemory(&message, sizeof(message));
	message.lpszSubject = (LPSTR) subject.toStdString().c_str();
	message.nFileCount = filedesc.size();
	message.lpFiles = (lpMapiFileDesc)  &filedesc[0];

    int nError = SendMail(0, (ULONG)HWND_DESKTOP, &message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
        return false;

	return true;
}
