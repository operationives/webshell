#ifndef WINADDON_H
#define WINADDON_H

#include <QWinJumpList>
#include <QWinJumpListCategory>
#include <QWinJumpListItem>
#include <QCoreApplication>
#include <QDir>

class WinAddon : public QObject
{

    Q_OBJECT

public:
    WinAddon();
    ~WinAddon();

private:
    QWinJumpList *jumplist;
    QWinJumpListCategory *tasks;
    QWinJumpListItem *quitItem;
};

#endif // WINADDON_H

