QT += webkitwidgets core network xml
win32 {
    QT += winextras
}

CONFIG += warn_off

QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

HEADERS =	mainwindow.h \
		filedownloader.h \
		wnavigator.h \
		parametres.h \
		mywebview.h \
		configmanager.h \
		downloadprogresslistener.h \
		webapp.h \
		navigatorplugins.h \
		semaphore.h \
		global.h \
		informations.h \
		webshellparameters.h \
		cookiejar.h \
		autosaver.h \
		mynetworkaccessmanager.h
win32 {
    HEADERS +=	mailsender.h \
		winaddon.h
}

SOURCES =	main.cpp \
		mainwindow.cpp \
		filedownloader.cpp \
		wnavigator.cpp \
		parametres.cpp \
		mywebview.cpp \
		configmanager.cpp \
		webapp.cpp \
		navigatorplugins.cpp \
		semaphore.cpp \
		global.cpp \
		informations.cpp \
		webshellparameters.cpp \
		cookiejar.cpp \
		autosaver.cpp \
		mynetworkaccessmanager.cpp
win32 {
    SOURCES +=	mailsender.cpp \
		winaddon.cpp
}
