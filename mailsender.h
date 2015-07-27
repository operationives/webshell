#ifndef MAILSENDER
#define MAILSENDER

#include <Windows.h>
#include <mapi.h>
#include <comdef.h>
#include <QObject>
#include <QVector>
#include <vector>

class MailSender
{
public:
    MailSender();
    ~MailSender();

    void AddFile(LPCTSTR path, LPCTSTR name = NULL);
    bool Send(HWND hWndParent, LPCTSTR szSubject = NULL);

private:
    struct attachment { QString path, name; };
    QVector<attachment> m_Files;
    HMODULE m_hLib;
};

#endif // MAILSENDER

