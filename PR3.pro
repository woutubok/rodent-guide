QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customtreewidget.cpp \
    main.cpp \
    rodent.cpp \
    rodentchangeform.cpp \
    widget.cpp

HEADERS += \
    customtreewidget.h \
    rodent.h \
    rodentchangeform.h \
    widget.h

FORMS += \
    rodentchangeform.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../build-PR3-Desktop_Qt_6_6_1_MinGW_64_bit-Debug/rodents.xml \
    ../build-PR3-Desktop_Qt_6_6_1_MinGW_64_bit-Debug/savedRodents.xml \
    ../build-PR3-Desktop_Qt_6_6_2_MinGW_64_bit-Debug/savedRodents.xml \
    ../build-PR3-Desktop_Qt_6_6_2_MinGW_64_bit-Debug/settings.xml

RESOURCES += \
    res.qrc
