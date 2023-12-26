QT += core sql widgets testlib

CONFIG += c++2a

SOURCES +=\
	EfProviderTest.cpp

HEADERS += \
    CppEFProviderLib/AbstractEFProvider.hpp

INCLUDEPATH += CppEFProviderLib

