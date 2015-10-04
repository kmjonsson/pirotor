QT += widgets
QT += core
QT += network


HEADERS       = rotor.h \
		rotorbuttons.h \
		mmq.h
SOURCES       = rotor.cpp \
		rotorbuttons.cpp \
		mmq.cpp \
                main.cpp

QMAKE_PROJECT_NAME = rotor

# install
target.path = target
INSTALLS += target

UNAME = $$system(uname -m)

DESTDIR=$$UNAME
OBJECTS_DIR=$$UNAME
MOC_DIR=$$UNAME
