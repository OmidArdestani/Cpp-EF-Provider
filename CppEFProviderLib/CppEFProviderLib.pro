QT -= qt


CONFIG += c++20

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    AbstractEFProvider.h \
    DatabaseModel.h \
    EFProviderDrives.h \
    PropertyVariant.h

SOURCES += \
    AbstractEFProvider.cpp \
    EFProviderDrives.cpp
