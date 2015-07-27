QT += webkitwidgets core network xml winextras

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

HEADERS =	mainwindow.h \
		filedownloader.h \
		wnavigator.h \
		parametres.h \
		mywebview.h \
		configmanager.h \
		downloadprogresslistener.h \
		winaddon.h \
		webapp.h \
		navigatorplugins.h \
		semaphore.h \
		global.h \
		informations.h \
		webshellparameters.h \
		cookiejar.h \
		autosaver.h \
		mailsender.h
SOURCES =	main.cpp \
		mainwindow.cpp \
		filedownloader.cpp \
		wnavigator.cpp \
		parametres.cpp \
		mywebview.cpp \
		configmanager.cpp \
		winaddon.cpp \
		webapp.cpp \
		navigatorplugins.cpp \
		semaphore.cpp \
		global.cpp \
		informations.cpp \
		webshellparameters.cpp \
		cookiejar.cpp \
		autosaver.cpp \
		mailsender.cpp
