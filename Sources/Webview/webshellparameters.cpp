#include "webshellparameters.h"
#include "Outils/configmanager.h"

/**
 * @brief Crée l'objet WebshellParameters
 */
WebshellParameters::WebshellParameters()
	:QObject()
{
}


/**
 * @brief Indique si les outils développeur sont activés ou non
 * @return config.GetDeveloperToolsMode()
 */
bool WebshellParameters::DeveloperToolsActivated() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetDeveloperToolsMode();
}

/**
 * @brief Active ou désactive les outils développeur
 * @param developerToolsActivated Si vrai, active les outils, sinon, les désactive
 */
void WebshellParameters::SetDeveloperToolsActivated(bool developerToolsActivated)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetDeveloperToolsMode(developerToolsActivated);
}

/**
 * @brief Indique si le fait de cliquer sur la croix rouge minimise la fenêtre ou la ferme
 * @return config.GetCloseButtonBehaviour()
 */
bool WebshellParameters::Minimization() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetCloseButtonBehaviour();
}

/**
 * @brief Met à jour le comportement d'un clic sur la croix rouge
 * @param minimization  Si vrai, la croix rouge minimise l'application, sinon elle la ferme
 */
void WebshellParameters::SetMinimization(bool minimization)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetCloseButtonBehaviour(minimization);
}

/**
 * @brief Indique si la barre de menu est présente
 * @return config.GetCloseButtonBehaviour()
 */
bool WebshellParameters::MenuBarPresent() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetMenuBarPresent();
}

/**
 * @brief Met à jour l'affichage de la barre de menu
 * @param menuBarPresent  Si vrai, le menu est affiché, sinon non
 */
void WebshellParameters::SetMenuBarPresent(bool menuBarPresent)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetMenuBarPresent(menuBarPresent);
}

/**
 * @brief Retourne la cible des événements
 * @return m_target
 */
QString WebshellParameters::Target() const
{
	return m_target;
}

/**
 * @brief Met à jour la cible des événements
 * @param target	Nouvelle cible des événements
 */
void WebshellParameters::SetTarget(const QString &target)
{
	m_target = target;
}

/**
 * @brief Retourne la langue de l'application
 * @return config.GetLanguage()
 */
QString WebshellParameters::Lang() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetLanguage();
}

/**
 * @brief Met à jour la langue de l'application
 * @param lang	Nouvelle langue pour l'application
 */
void WebshellParameters::SetLang(const QString &lang)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetLanguage(lang);
}
