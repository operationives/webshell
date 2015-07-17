#include "configmanager.h"
#include <iostream>
#include <QApplication>

/**
 * @brief ConfigManager::ConfigManager  Place les paramètres spécifiés dans le fichier xml du dossier courant dans les attributs de la classe
 */
ConfigManager::ConfigManager(){
    QDomDocument dom("webshell_xml");
    QFile file(QApplication::applicationDirPath()+"/webshell.xml");
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!dom.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    QDomElement docElem = dom.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()){
        QDomElement e = n.toElement();
        if(e.attribute("name") == "fullscreen")
            fullscreen = (e.attribute("value") == "true" ? true : false);
        else if(e.attribute("name") == "minimization")
            minimization = (e.attribute("value") == "true" ? true : false);
        else if(e.attribute("name") == "developerToolsActivated")
            developerToolsActivated = (e.attribute("value") == "true" ? true : false);

        n = n.nextSibling();
    }
}

ConfigManager::~ConfigManager(){

}

/**
 * @brief Xml_Dom::ChangeParameter Affecte la valeur value au paramètre parameter dans les attributs et le fichier xml
 * @param parameter Type de paramètre à changer. Valuers possibles: "fullscreen","minimization","developerToolsActivated"
 * @param value     Valeur à affecter au paramètre
 */
void ConfigManager::ChangeParameter(QString parameter, bool value){

    if(parameter == "fullscreen")
        fullscreen = value;
    else if(parameter == "minimization")
        minimization = value;
    else if(parameter == "developerToolsActivated")
        developerToolsActivated = value;
    else{
        qDebug() << "Type de paramètre incompatible.";
        return;
    }

    QDomDocument dom("webshell_xml");
    QFile doc_xml(QApplication::applicationDirPath()+"/webshell.xml");
    if(!doc_xml.open(QIODevice::ReadOnly))
        return;
    if(!dom.setContent(&doc_xml)){
        doc_xml.close();
        return;
    }
    QDomElement docElem = dom.documentElement();
    QDomNode n = docElem.firstChild();

    while(!n.isNull()){
        QDomElement write_elem = dom.createElement("parameter");
        QDomElement e = n.toElement();
        n = n.nextSibling();
        if(e.attribute("name") == "fullscreen"){
            write_elem.setAttribute("name", "fullscreen");
            if(fullscreen) write_elem.setAttribute("value", "true");
            else write_elem.setAttribute("value", "false");
        }
        else if(e.attribute("name") == "minimization"){
            write_elem.setAttribute("name", "minimization");
            if(minimization) write_elem.setAttribute("value", "true");
            else write_elem.setAttribute("value", "false");
        }
        else if(e.attribute("name") == "developerToolsActivated"){
            write_elem.setAttribute("name", "developerToolsActivated");
            if(developerToolsActivated) write_elem.setAttribute("value", "true");
            else write_elem.setAttribute("value", "false");
        }
        docElem.replaceChild(write_elem,e);

    }

    doc_xml.close();

    QString write_doc = dom.toString();

    QFile fichier(QApplication::applicationDirPath()+"/webshell.xml");
    if(!fichier.open(QIODevice::WriteOnly))
    {
        fichier.close();
        qDebug() << "Impossible d'écrire dans le document XML";
        return;
    }
    QTextStream stream(&fichier);
    stream << write_doc; // On utilise l'opérateur << pour écrire write_doc dans le document XML.
}

/**
 * @brief ConfigManager::Getscreenmode Indique si l'utilisateur doit être en plein écran ou non
 * @return fullscreen
 */
bool ConfigManager::GetScreenMode(){
    return fullscreen;
}

/**
 * @brief ConfigManager::GetCloseButtonBehaviour Indique le comportement du bouton de fermeture
 * @return minimization
 */
bool ConfigManager::GetCloseButtonBehaviour(){
    return minimization;
}

/**
 * @brief ConfigManager::GetDeveloperToolsMode Indique si l'utilisateur a accès aux outils développeur
 * @return developerToolsActivated
 */
bool ConfigManager::GetDeveloperToolsMode(){
    return developerToolsActivated;
}

/**
 * @brief ConfigManager::SetScreenMode Met à jour le paramètre "fullscreen"
 * @param fullscreen    Nouvelle valeur de this.fullscreen
 */
void ConfigManager::SetScreenMode(bool fullscreen){
    ChangeParameter("fullscreen",fullscreen);
}

/**
 * @brief ConfigManager::SetCloseButtonBehaviour Met à jour le paramètre "minimization"
 * @param minimization  Nouvelle valeur de this.minimization
 */
void ConfigManager::SetCloseButtonBehaviour(bool minimization){
    ChangeParameter("minimization",minimization);
}

/**
 * @brief ConfigManager::SetDeveloperToolsMode Met à jour le paramètre "developerToolsActivated"
 * @param developerToolsActivated Nouvelle valeur de this.developerToolsActivated
 */
void ConfigManager::SetDeveloperToolsMode(bool developerToolsActivated){
    ChangeParameter("developerToolsActivated",developerToolsActivated);
}
