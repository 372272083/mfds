/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QDatetime>

//#include "savesamplethread.h"

class MdiChild;
class MDISubWidget;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
QT_END_NAMESPACE

class QSplitter;
class LeftTreeView;
class SqliteDB;
class DeviceLinkManager;
class SaveSampleThread;
//class SaveAnalyseThread;
class QTabBar;
class QTcpSocket;
class QLabel;

//class RecordWaveWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
#ifndef QT_NO_CLIPBOARD
    void cut();
    void copy();
    void paste();
#endif
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void switchLayoutDirection();

    void windowLayout();
    void slotSplitterMoved(int pos, int index);
    void devicetypemanager();

    void motortypemanager();
    void bearmanager();

    void showRightControllerSlot(int,int,QString,int,QString,QString,QString,int,int);
    void treeSelectedChangeSlot(int);
    void linkStateChangedSlot(int treeId,bool state);

    void connectDevice();

    void sDataCheckedSlot(int);
    void sOnlineCheckedSlot(int);

    void recoreWaveSlot();
    void recorwWaveWithInfoSlot(int interval,QString name);
    void recoreWaveTimeoutDevice();

    void databaseSlot();

    void enterOfflineWithIntervalSlot(QString,QString,QString);

    void tabBarClickedSlot(int index);

    void diagnoseSettingSlot();
    void chartnodeSettingSlot();

    void fftparamSettingSlot();

    void timeDiagnose();

    void serverparameterSettingSlot();
    void serverStateChangerSlot(bool);

    void timeSendInfo();
    void slotConnected();
    void slotDisconnected();
    void dataReceived();

private:

    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void createtables();

    MDISubWidget *activeMdiChild() const;
    QMdiSubWindow *findMdiChild(const QString &fileName) const;
    QMdiSubWindow *findMdiChild(const int uniqueId) const;

    void initMotorConditionTables();
    void initStyle();

    float getOfflineMotorRotate(QString,QString);

private:
    QString offlinewave_ts;
    QMdiArea *mdiArea;

    QTabBar* utab;

    QMenu *windowMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;

    QAction *recordwaveAct;
    //QAction *recordwavestopAct;

    QAction *databaseAct;

    QAction *editDevice;
    QAction *EditMotor;

    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *windowMenuSeparatorAct;

    QSplitter *pSpliterMain;

    SqliteDB *db;
    LeftTreeView* leftView;
    DeviceLinkManager *devicelink;

    SaveSampleThread* saveThread;
    //SaveAnalyseThread* saveAnalyseThread;

    //RecordWaveWidget *recordWave;
    QTcpSocket* tcpSocket;
    bool isconnected;
    bool isWaiting;
    QLabel* serverState;
    QString serverIP;
    int port;

    QLabel* diagnoseState;
};

#endif
