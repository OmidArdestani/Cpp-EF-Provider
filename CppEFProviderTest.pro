QT += core sql widgets testlib

CONFIG += c++2a

#TEMPLATE = app

SOURCES +=\
	EfProviderTest.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/CppEFProviderLib/x64/release/ -lCppEFProvider
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/CppEFProviderLib/x64/debug/ -lCppEFProvider

INCLUDEPATH += $$PWD/CppEFProviderLib
DEPENDPATH += $$PWD/CppEFProviderLib
