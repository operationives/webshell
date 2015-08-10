QT += webkitwidgets core network xml
win32 {
    QT += winextras
}

CONFIG += warn_off

QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

HEADERS =	global.h \
		Outils/autosaver.h \
		Outils/configmanager.h \
		Outils/filedownloader.h \
		Outils/semaphore.h \
		Widgets/mainwindow.h \
		Widgets/informations.h \
		Widgets/parametres.h \
		Webview/cookiejar.h \
		Webview/downloadprogresslistener.h \
		Webview/mynetworkaccessmanager.h \
		Webview/mywebview.h \
		Webview/navigatorplugins.h \
		Webview/webapp.h \
		Webview/webshellparameters.h \
		Webview/wnavigator.h
win32 {
    HEADERS +=	Windows/mailsender.h \
		Windows/winaddon.h
}

SOURCES =	main.cpp \
		global.cpp \
		Outils/autosaver.cpp \
		Outils/configmanager.cpp \
		Outils/filedownloader.cpp \
		Outils/semaphore.cpp \
		Widgets/mainwindow.cpp \
		Widgets/informations.cpp \
		Widgets/parametres.cpp \
		Webview/cookiejar.cpp \
		Webview/mynetworkaccessmanager.cpp \
		Webview/mywebview.cpp \
		Webview/navigatorplugins.cpp \
		Webview/webapp.cpp \
		Webview/webshellparameters.cpp \
		Webview/wnavigator.cpp
win32 {
    SOURCES +=	Windows/mailsender.cpp \
		Windows/winaddon.cpp
}
