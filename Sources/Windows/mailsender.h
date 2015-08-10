#ifndef MAILSENDER_H
#define MAILSENDER_H

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

	void AddFile(QString path, QString name = NULL);
	bool Send(QString szSubject = NULL);

private:
	struct attachment { QString path, name; };
	QVector<attachment> m_Files;
	HMODULE m_hLib;
};

#endif // MAILSENDER_H

