#include <wchar.h>
#include <iostream>
#include "mailsender.h"

MailSender::MailSender()
{
    m_hLib = LoadLibrary(QString("MAPI32.DLL").toStdWString().c_str());
}

MailSender::~MailSender()
{
    FreeLibrary(m_hLib);
}

void MailSender::AddFile( LPCTSTR file, LPCTSTR name )
{
    attachment a;
    a.path = QString::fromWCharArray(file);
    if (!name)
    {
        //On recode PathFindFilePath inaccessible
        a.name = QString::fromWCharArray(file);
        if(a.name.lastIndexOf('/') == a.name.length())
           a.name.chop(1);
        a.name.remove(0,a.name.lastIndexOf('/'));
    }
    else
        a.name = QString::fromWCharArray(name);
    m_Files.push_back(a);
}

bool MailSender::Send(HWND hWndParent, LPCTSTR szSubject)
{
    if (!m_hLib)
        return false;

    LPMAPISENDMAIL SendMail;
    SendMail = (LPMAPISENDMAIL) GetProcAddress(m_hLib, QString("MAPISendMail").toStdString().c_str());

    if (!SendMail)
        return false;

    QVector<MapiFileDesc> filedesc;
    for (QVector<attachment>::const_iterator ii = m_Files.begin(); ii!=m_Files.end(); ii++)
    {
        MapiFileDesc fileDesc;
        ZeroMemory(&fileDesc, sizeof(fileDesc));
        fileDesc.nPosition = (ULONG)-1;
        fileDesc.lpszPathName = (LPTSTR) ii->path.toStdWString().c_str();
        fileDesc.lpszFileName = (LPTSTR) ii->name.toStdWString().c_str();
        filedesc.push_back(fileDesc);
    }

    tstring subject;
    if (szSubject)
        subject = szSubject;
    else
    {
        for (std::vector<attachment>::const_iterator ii = m_Files.begin(); ii!=m_Files.end(); ii++)
        {
            subject += ii->name.c_str();
            if (ii+1 != m_Files.end())
                subject += ", ";
        }
    }

    MapiMessage message;
    ZeroMemory(&message, sizeof(message));
    message.lpszSubject = (LPTSTR) subject.c_str();
    message.nFileCount = filedesc.size();
    message.lpFiles = &filedesc[0];

    int nError = SendMail(0, (ULONG)hWndParent, &message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

    if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
        return false;

    return true;
}
