#-------------------------------------------------
#
# Project created by QtCreator 2018-10-29T11:05:25
#
#-------------------------------------------------

QT       += core gui sql network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mfds
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sqlitedb.cpp \
    motormanage.cpp \
    motoredit.cpp \
    motortype.cpp \
    beartype.cpp \
    beartypeedit.cpp \
    motortypeedit.cpp \
    devicemanager.cpp \
    deviceedit.cpp \
    devicetype.cpp \
    devicetypeedit.cpp \
    devicepipe.cpp \
    devicelinkinfo.cpp \
    cmieedevicelinkinfo.cpp \
    cmievdevicelinkinfo.cpp \
    devicelinkmanager.cpp \
    xsocketclient.cpp \
    motorinfowidget.cpp \
    devicepipeselectwidget.cpp \
    motorinfo.cpp \
    chartwidget.cpp \
    chartinfocontroller.cpp \
    qcustomplot.cpp \
    devicepipeinfo.cpp \
    headeritemwidget.cpp \
    devicepipeedit.cpp \
    cmietdevicelinkinfo.cpp \
    chargeinfo.cpp \
    queryinfo.cpp \
    vibrateinfo.cpp \
    freqinfo.cpp \
    waveinfo.cpp \
    temperatureinfo.cpp \
    savesamplethread.cpp \
    paramsetting.cpp \
    qiplineedit.cpp \
    modbustcpserver.cpp \
    modbustcpclient.cpp \
    comsetting.cpp \
    udisk.cpp \
    waveform.cpp \
    cmieeanalysethread.cpp \
    cmieewaveinfo.cpp \
    cmievanalysethread.cpp \
    cmievwaveinfo.cpp \
    mfdslib/Diagnosis.cpp \
    mfdslib/ElcWaveAnsys.cpp \
    mfdslib/FFT_ANSYS.cpp \
    mfdslib/Globals.cpp \
    mfdslib/HilbertTransform.cpp \
    mfdslib/VibWaveAnsys.cpp

HEADERS  += mainwindow.h \
    sqlitedb.h \
    motormanage.h \
    constants.h \
    motoredit.h \
    motortype.h \
    beartype.h \
    beartypeedit.h \
    motortypeedit.h \
    devicemanager.h \
    deviceedit.h \
    devicetype.h \
    devicetypeedit.h \
    devicepipe.h \
    devicelinkinfo.h \
    cmieedevicelinkinfo.h \
    cmievdevicelinkinfo.h \
    devicelinkmanager.h \
    xsocketclient.h \
    motorinfowidget.h \
    devicepipeselectwidget.h \
    motorinfo.h \
    chartwidget.h \
    chartinfocontroller.h \
    qcustomplot.h \
    devicepipeinfo.h \
    headeritemwidget.h \
    devicepipeedit.h \
    cmietdevicelinkinfo.h \
    chargeinfo.h \
    queryinfo.h \
    vibrateinfo.h \
    freqinfo.h \
    waveinfo.h \
    temperatureinfo.h \
    savesamplethread.h \
    paramsetting.h \
    qiplineedit.h \
    modbustcpserver.h \
    modbustcpclient.h \
    comsetting.h \
    udisk.h \
    waveform.h \
    cmieeanalysethread.h \
    cmieewaveinfo.h \
    cmievanalysethread.h \
    cmievwaveinfo.h \
    mfdslib/Basic.h \
    mfdslib/Diagnosis.h \
    mfdslib/ElcWaveAnsys.h \
    mfdslib/FFT_ANSYS.h \
    mfdslib/Globals.h \
    mfdslib/HilbertTransform.h \
    mfdslib/IO_Param.h \
    mfdslib/mfdslib_global.h \
    mfdslib/VibWaveAnsys.h

RESOURCES += \
    res.qrc

TRANSLATIONS += zh.ts \
               en.ts

FORMS +=
