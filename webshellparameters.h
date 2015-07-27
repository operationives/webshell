#ifndef DEVELOPERTOOLSACTIVATED_H
#define DEVELOPERTOOLSACTIVATED_H

#include <QObject>

class WebshellParameters : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool developerToolsActivated READ DeveloperToolsActivated WRITE SetDeveloperToolsActivated)
	Q_PROPERTY(bool minimization READ Minimization WRITE SetMinimization)
public:
	WebshellParameters();


private:
	bool m_developerToolsActivated;
	bool m_minimization;
	bool DeveloperToolsActivated() const;
	void SetDeveloperToolsActivated(bool developerToolsActivated);
	bool Minimization() const;
	void SetMinimization(bool minimization);

};

#endif // DEVELOPERTOOLSACTIVATED_H

