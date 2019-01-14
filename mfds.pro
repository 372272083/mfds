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
    FaultFeatureLib/AccFreqaAnsys.cpp \
    FaultFeatureLib/DataMining.cpp \
    FaultFeatureLib/DataPreProcess.cpp \
    FaultFeatureLib/DFT_ANSYS.cpp \
    FaultFeatureLib/Diagnosis.cpp \
    FaultFeatureLib/FFT_ANSYS.cpp \
    FaultFeatureLib/FreqDmnAnsys.cpp \
    FaultFeatureLib/globals.cpp \
    FaultFeatureLib/HilbertTransform.cpp \
    FaultFeatureLib/mfdspanel.cpp \
    FaultFeatureLib/MotorFaultDiagnosis.cpp \
    FaultFeatureLib/RotaCoordinatAnsys.cpp \
    FaultFeatureLib/RotateFreqAnsys.cpp \
    FaultFeatureLib/stdafx.cpp \
    FaultFeatureLib/TimeDmnAnsys.cpp \
    waveform.cpp

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
    FaultFeatureLib/AccFreqAnsys.h \
    FaultFeatureLib/Basic.hpp \
    FaultFeatureLib/DataMining.h \
    FaultFeatureLib/DataPreProcess.h \
    FaultFeatureLib/DFT_ANSYS.h \
    FaultFeatureLib/Diagnosis.h \
    FaultFeatureLib/FaultFeatureLib.h \
    FaultFeatureLib/FFT_ANSYS.h \
    FaultFeatureLib/FreqDmnAnsys.h \
    FaultFeatureLib/globals.h \
    FaultFeatureLib/HilbertTransform.h \
    FaultFeatureLib/IO_Param.h \
    FaultFeatureLib/mfdspanel.h \
    FaultFeatureLib/MotorFaultDiagnosis.h \
    FaultFeatureLib/RotaCoordinatAnsys.h \
    FaultFeatureLib/RotateFreqAnsys.h \
    FaultFeatureLib/stdafx.h \
    FaultFeatureLib/targetver.h \
    FaultFeatureLib/TimeDmnAnsys.h \
    FaultFeatureLib/UserCtrlParm.h \
    waveform.h

RESOURCES += \
    res.qrc

TRANSLATIONS += zh.ts \
               en.ts

FORMS +=
