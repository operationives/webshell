#include "winaddon.h"

WinAddon::WinAddon(){
    jumplist = new QWinJumpList();
    tasks = jumplist->tasks();

    quitItem = new QWinJumpListItem(QWinJumpListItem::Link);
    quitItem->setDescription("Quitter l'application");
    quitItem->setTitle("Quitter");
    quitItem->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    tasks->addItem(quitItem);
    tasks->setVisible(true);
}

WinAddon::~WinAddon(){
    delete jumplist;
    delete quitItem;
}
