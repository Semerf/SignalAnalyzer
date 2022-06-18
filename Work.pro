QT       += core gui \
            widgets network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -L$$(IPP_LIB_X64) -lippsmt -lippcoremt -lippvmmt -lipps

INCLUDEPATH += $$(IPP_DIR)




SOURCES += \
    anotherwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    spectrumwindow.cpp

HEADERS +=  anotherwindow.h \
    mainwindow.h \
    qcustomplot.h \
    spectrumwindow.h

FORMS += \
    anotherwindow.ui \
    mainwindow.ui \
    spectrumwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    1.txt \
    2.txt \
    3.txt \
    4.txt

