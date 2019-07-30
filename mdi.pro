
QT       += core gui sql network printsupport
RC_ICONS = favicon.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS       = mainwindow.h \
                mdichild.h \
    constants.h \
    lefttreeview.h \
    sqlitedb.h \
    deviceeditdialog.h \
    deviceinfo.h \
    globalvariable.h \
    globalfunction.h \
    devicetype.h \
    devicetypeedit.h \
    motoredit.h \
    beartype.h \
    motortype.h \
    beartypeedit.h \
    motortypeedit.h \
    motorinfo.h \
    devicepipeedit.h \
    channelinfo.h \
    treenodeinfo.h \
    fearturechartwidget.h \
    qcustomplot.h \
    devicelinkmanager.h \
    xsocketclient.h \
    cmieedeviceinfo.h \
    chargeinfo.h \
    freqinfo.h \
    vibrateinfo.h \
    waveinfo.h \
    cmievdeviceinfo.h \
    cmietdeviceinfo.h \
    temperatureinfo.h \
    queryinfo.h \
    savesamplethread.h \
    mdisubwidget.h \
    recordwavewidget.h \
    databasemanager.h \
    mfdslib/Basic.h \
    mfdslib/ElcWaveAnsys.h \
    mfdslib/FFT_ANSYS.h \
    mfdslib/Globals.h \
    mfdslib/HilbertTransform.h \
    mfdslib/IO_Param.h \
    mfdslib/mfdslib_global.h \
    mfdslib/VibWaveAnsys.h \
    conditioninfo.h \
    chartbannerwidget.h \
    diagnosesetting.h \
    motorconditiontable.h \
    mfdslib/Diagnosis.h \
    freqxselectdialog.h \
    surfacechart.h \
    pathtrackingchannelselect.h \
    paramsettingdialog.h \
    fftparamsettingdialog.h \
    fftparams.h \
    faultwidget.h \
    faulttypeengine.h \
    faultinfo.h \
    faultdetailresultdialog.h \
    cmievwaveinfo.h \
    cmievanalysethread.h \
    cmieewaveinfo.h \
    cmieeanalysethread.h \
    serverparametersetting.h \
    qiplineedit.h \
    parktransformationwidget.h \
    tw888deviceinfo.h \
    syncdatainfo.h \
    tw888info.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp \
    lefttreeview.cpp \
    sqlitedb.cpp \
    deviceeditdialog.cpp \
    deviceinfo.cpp \
    globalvariable.cpp \
    globalfunction.cpp \
    devicetype.cpp \
    devicetypeedit.cpp \
    motoredit.cpp \
    beartype.cpp \
    motortype.cpp \
    beartypeedit.cpp \
    motortypeedit.cpp \
    motorinfo.cpp \
    devicepipeedit.cpp \
    channelinfo.cpp \
    treenodeinfo.cpp \
    fearturechartwidget.cpp \
    qcustomplot.cpp \
    devicelinkmanager.cpp \
    xsocketclient.cpp \
    cmieedeviceinfo.cpp \
    chargeinfo.cpp \
    freqinfo.cpp \
    vibrateinfo.cpp \
    waveinfo.cpp \
    cmievdeviceinfo.cpp \
    cmietdeviceinfo.cpp \
    temperatureinfo.cpp \
    queryinfo.cpp \
    savesamplethread.cpp \
    mdisubwidget.cpp \
    recordwavewidget.cpp \
    databasemanager.cpp \
    mfdslib/ElcWaveAnsys.cpp \
    mfdslib/FFT_ANSYS.cpp \
    mfdslib/Globals.cpp \
    mfdslib/HilbertTransform.cpp \
    mfdslib/VibWaveAnsys.cpp \
    conditioninfo.cpp \
    chartbannerwidget.cpp \
    diagnosesetting.cpp \
    motorconditiontable.cpp \
    mfdslib/Diagnosis.cpp \
    freqxselectdialog.cpp \
    surfacechart.cpp \
    pathtrackingchannelselect.cpp \
    paramsettingdialog.cpp \
    fftparamsettingdialog.cpp \
    fftparams.cpp \
    faultwidget.cpp \
    faulttypeengine.cpp \
    faultinfo.cpp \
    faultdetailresultdialog.cpp \
    cmievwaveinfo.cpp \
    cmievanalysethread.cpp \
    cmieewaveinfo.cpp \
    cmieeanalysethread.cpp \
    serverparametersetting.cpp \
    qiplineedit.cpp \
    parktransformationwidget.cpp \
    tw888deviceinfo.cpp \
    syncdatainfo.cpp \
    tw888info.cpp

RESOURCES     = mdi.qrc \
    qss.qrc

TRANSLATIONS += zh.ts \
               en.ts

#LIBS += D:\\QtProject\\build\\build-mdi-Desktop_Qt_5_10_0_MinGW_32bit-Debug\\debug\\mfdslib.dll
#LIBS += -L . -l mfdslib
# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/mdi
INSTALLS += target
