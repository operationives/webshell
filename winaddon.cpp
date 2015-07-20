#include "winaddon.h"

/**
 * @brief WinAddon::WinAddon Constructeur de la JumpList de l'application
 */
WinAddon::WinAddon()
{
    jumplist = new QWinJumpList();
    tasks = jumplist->tasks();

    quitItem = new QWinJumpListItem(QWinJumpListItem::Link);
    quitItem->setDescription("Ouvrir à nouveau l'application");
    quitItem->setTitle("Nouveau");
    quitItem->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    tasks->addItem(quitItem);
    tasks->setVisible(true);
}

/**
 * @brief WinAddon::~WinAddon Destructeur de la JumpList de l'application
 */
WinAddon::~WinAddon()
{
    delete jumplist;
    delete quitItem;
    this->~QObject();
}
