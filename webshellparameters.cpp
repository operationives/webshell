#include "webshellparameters.h"
#include "global.h"

/**
 * @brief WebshellParameters::DeveloperToolsActivated Initialise les attributs avec ceux de configmanager
 */
WebshellParameters::WebshellParameters()
	:QObject()
{
	m_developerToolsActivated = config->GetDeveloperToolsMode();
	m_minimization = config->GetCloseButtonBehaviour();
}


/**
 * @brief WebshellParameters::DeveloperToolsActivated Indique si les outils développeur sont activés ou non
 * @return m_developerToolsActivated
 */
bool WebshellParameters::DeveloperToolsActivated() const
{
	return m_developerToolsActivated;
}

/**
 * @brief WebshellParameters::SetDeveloperToolsActivated Active ou désactive les outils développeur
 * @param developerToolsActivated Si vrai, active les outils, sinon, les désactive
 */
void WebshellParameters::SetDeveloperToolsActivated(bool developerToolsActivated)
{
	m_developerToolsActivated = developerToolsActivated;
	config->SetDeveloperToolsMode(developerToolsActivated);
}

/**
 * @brief WebshellParameters::Minimization Indique si le fait de cliquer sur la croix rouge minimise la fenêtre ou la ferme
 * @return m_minimization
 */
bool WebshellParameters::Minimization() const
{
	return m_minimization;
}

/**
 * @brief WebshellParameters::SetMinimization Met à jour le comportement d'un clic sur la croix rouge
 * @param minimization  Si vrai, la croix rouge minimise l'application, sinon elle la ferme
 */
void WebshellParameters::SetMinimization(bool minimization)
{
	m_minimization = minimization;
	config->SetCloseButtonBehaviour(minimization);
}
