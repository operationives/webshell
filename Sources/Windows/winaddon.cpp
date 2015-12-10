#include "winaddon.h"

/**
 * @brief Constructeur de la JumpList de l'application
 */
WinAddon::WinAddon()
{
	jumplist = new QWinJumpList();
	tasks = jumplist->tasks();

	quitItem = new QWinJumpListItem(QWinJumpListItem::Link);
    quitItem->setDescription("Quitter l'application");
    quitItem->setTitle("Quitter");
	quitItem->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	tasks->addItem(quitItem);
	tasks->setVisible(true);
}

/**
 * @brief Destructeur de la JumpList de l'application
 */
WinAddon::~WinAddon()
{
	delete jumplist;
	delete quitItem;
}
