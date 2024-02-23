QT += core sql widgets testlib

CONFIG += c++2a

SOURCES +=\
	EfProviderTest.cpp

HEADERS += \
    CppEFProviderLib/AbstractEFProvider.hpp \
    CppEFProviderLib/DatabaseModel.h \
    CppEFProviderLib/EFDrivers/AbstractCommonDatabaseCommand.hpp \
    CppEFProviderLib/EFDrivers/EFDriverPostgreSql.hpp \
    CppEFProviderLib/EFDrivers/EFDriverSQLight.hpp \
    MyModels.h

INCLUDEPATH += CppEFProviderLib

