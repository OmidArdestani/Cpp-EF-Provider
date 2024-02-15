QT -= qt


CONFIG += c++20

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    AbstractEFProvider.hpp \
    DatabaseModel.h \
    EFDrivers/AbstractCommonDatabaseCommand.hpp \
    EFDrivers/EFDriverPostgreSql.hpp \
    EFDrivers/EFDriverSQLight.hpp \
    EFProviderDrives.h \
    PropertyVariant.h

