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

#include <QtWidgets>
#include <QDatetime>
#include <QTabBar>

#include "mainwindow.h"
#include "mdichild.h"

#include <QSplitter>
#include <QTimer>
#include <QDebug>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QMap>

#include "lefttreeview.h"
#include "sqlitedb.h"
#include "devicetype.h"
#include "motortype.h"
#include "beartype.h"

#include "fearturechartwidget.h"

#include "devicelinkmanager.h"
#include "globalvariable.h"

#include "mdisubwidget.h"
#include "recordwavewidget.h"

#include "databasemanager.h"

#include "savesamplethread.h"
//#include "saveanalysethread.h"
#include "diagnosesetting.h"

#include "motorconditiontable.h"
#include "surfacechart.h"
#include "paramsettingdialog.h"
#include "fftparamsettingdialog.h"
#include "faultwidget.h"
#include "parktransformationwidget.h"

#include "faultinfo.h"
#include "motorinfo.h"
#include "serverparametersetting.h"

#include "mfdslib/Diagnosis.h"

MainWindow::MainWindow()
    : mdiArea(new QMdiArea)
{
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    utab = new QTabBar();
    //utab->setAutoHide(true);
    utab->setStyleSheet("QTabBar::tab:selected {background: lightgray;font-weight:bold;color:black;}");
    connect(utab, SIGNAL(tabBarClicked(int)), this, SLOT(tabBarClickedSlot(int)));

    QSplitter *mainLayout = new QSplitter(Qt::Vertical);
    mainLayout->setContentsMargins(0,2,2,0);

    pSpliterMain = new QSplitter(Qt::Horizontal);
    pSpliterMain->setContentsMargins(0,2,2,0);
    this->setWindowIcon(QIcon(":/images/icon"));

    QString cur_path = QCoreApplication::applicationDirPath();
    //QString dbfile = "D:/Program Files/mfds/data_mf.db";
    QString dbfile = cur_path + "/data_mf.db";
    qDebug() <<"current path: "<< dbfile << endl;
    db = new SqliteDB(dbfile);
    if(db->open())
    {
        createtables();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Read config file failed,application will exit!"));
        this->close();  //读取配置文件失败
        return;
    }

    QSqlTableModel *settingmodel = db->modelNoHeader("setting");
    int tmpRow = settingmodel->rowCount();

    bool tok;
    int attr_value;
    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = settingmodel->record(i);
        int id = record.value("id").toInt(&tok);
        QString attr_value_s = record.value("attr_value").toString();

        switch (id) {
        case 9:
            GlobalVariable::server_ip = attr_value_s;
            break;
        case 14:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                GlobalVariable::server_enable = true;
            }
            else
            {
                GlobalVariable::server_enable = false;
            }
            break;
        case 17:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                GlobalVariable::is_sync_done = true;
            }
            else
            {
                GlobalVariable::is_sync_done = false;
            }
            break;
        default:
            break;
        }
    }
    delete settingmodel;

    leftView = new LeftTreeView(db);
    leftView->setContentsMargins(0,0,0,0);
    leftView->setupDeviceTree();
    leftView->setupMotorTree();
    connect(leftView, SIGNAL(showRightController(int,int,QString,int,QString,QString,QString,int,int)), this, SLOT(showRightControllerSlot(int,int,QString,int,QString,QString,QString,int,int)));
    //treeSelectedChange
    connect(leftView, SIGNAL(treeSelectedChange(int)), this, SLOT(treeSelectedChangeSlot(int)));

    devicelink = new DeviceLinkManager(/*db,*/this);
    connect(devicelink,SIGNAL(linkStateChanged(int,bool)), this, SLOT(linkStateChangedSlot(int,bool)));

    pSpliterMain->addWidget(leftView);

    leftView->setMinimumWidth(250);
    leftView->setMaximumWidth(400);
    pSpliterMain->addWidget(mdiArea);
    //pSpliterMain->setHandleWidth(4);

    connect(pSpliterMain, SIGNAL(splitterMoved(int,int)), this, SLOT(slotSplitterMoved(int,int)));

    mainLayout->addWidget(pSpliterMain);

    QWidget* uwidget = new QWidget(this);
    uwidget->setContentsMargins(0,0,0,0);
    QHBoxLayout* ulayout = new QHBoxLayout(this);
    ulayout->setContentsMargins(0,0,0,0);
    ulayout->setSpacing(0);
    utab->setContentsMargins(0,0,0,0);
    ulayout->addWidget(utab,0,Qt::AlignLeft);
    //ulayout->addStretch();
    uwidget->setLayout(ulayout);
    mainLayout->addWidget(uwidget);

    setCentralWidget(mainLayout);
    //this->setLayout(mainLayout);
    connect(mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateMenus);

    createActions();
    createStatusBar();
    updateMenus();

    readSettings();

    if(GlobalVariable::version < 2)
    {
        switch (GlobalVariable::s_t) {
        case 0:
            setWindowTitle(tr("MDFS"));
            break;
        case 1:
            setWindowTitle(tr("MDFS - Vibrate monitoring and analyse system"));
            break;
        case 10:
            setWindowTitle(tr("Vibration Analysis Tool"));
            break;
        case 2:
            setWindowTitle(tr("MDFS - Electric monitoring and analyse system"));
            break;
        case 3:
            setWindowTitle(tr("MDFS - Temperature monitoring and analyse system"));
            break;
        default:
            break;
        }
    }
    else
    {
        switch (GlobalVariable::s_t) {
        case 0:
            setWindowTitle(tr("MDFS"));
            break;
        case 1:
            setWindowTitle(tr("MDFS - Vibrate monitoring and diagnosis system"));
            break;
        case 10:
            setWindowTitle(tr("Vibration Analysis Tool"));
            break;
        case 2:
            setWindowTitle(tr("MDFS - Electric monitoring and diagnosis system"));
            break;
        case 3:
            setWindowTitle(tr("MDFS - Temperature monitoring and diagnosis system"));
            break;
        default:
            break;
        }
    }
    setUnifiedTitleAndToolBarOnMac(true);

    saveThread = new SaveSampleThread(db);
    saveThread->start();

    //saveAnalyseThread = new SaveAnalyseThread(db);
    //saveAnalyseThread->start();

    QTimer::singleShot(0, this, SLOT(windowLayout()));

    tcpSocket = nullptr;
    QTimer::singleShot(1*1000, this, SLOT(connectDevice()));

    EditMotor->setEnabled(false);
    editDevice->setEnabled(false);
    //recordWave = nullptr;
    this->showMaximized();

    serverState = new QLabel();
    serverState->setText(tr("S"));
    this->statusBar()->addPermanentWidget(serverState);

    if(GlobalVariable::version >= 2)
    {
        diagnoseState = new QLabel();
        diagnoseState->setText(tr("Diagnose Mode"));
        this->statusBar()->addPermanentWidget(diagnoseState);
    }

    //initStyle();
    initMotorConditionTables();

    if(!GlobalVariable::server_enable)
    {
        leftView->updateActionState(0);
    }
}

void MainWindow::windowLayout()
{
    int leftWidth = 200;
    QList<int> lsSize;
    lsSize<<leftWidth;
    lsSize<<pSpliterMain->width()-leftWidth;
    pSpliterMain->setSizes(lsSize);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
    /*
    if(GlobalVariable::s_t == 10)
    {
        devicelink->close();
    }
    */
}

void MainWindow::initStyle()
{
    //加载样式表
    QFile file(":/qss/psblack.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void MainWindow::initMotorConditionTables()
{
    QVector<QString> mcs = db->getMotorConditionTable();

    bool tok;
    QVector<QString>::ConstIterator it;
    for(it=mcs.constBegin();it != mcs.constEnd();it++)
    {
        QString mc = *it;
        QStringList mc_items = mc.split(',');
        if(mc_items.length() == 5)
        {
            QString id_s = mc_items[0];
            QString u_s = mc_items[1];
            QString i_s = mc_items[2];
            QString rotate_s = mc_items[3];
            QString mcode_s = mc_items[4];

            float u_f,i_f,rotate_f;
            u_f = u_s.toFloat(&tok);
            if(!tok)
            {
                u_f = 0;
            }
            i_f = i_s.toFloat(&tok);
            if(!tok)
            {
                i_f = 0;
            }

            rotate_f = rotate_s.toFloat(&tok);
            if(!tok)
            {
                rotate_f = 0;
            }
            int id_i;
            id_i=id_s.toInt(&tok);
            if(!tok)
            {
                continue;
            }

            MotorConditionTable mct;
            mct.id = id_i;
            mct.u = u_f;
            mct.i = i_f;
            mct.rotate = rotate_f;
            if(GlobalVariable::motorConditionTables.contains(mcode_s))
            {
                GlobalVariable::motorConditionTables[mcode_s].push_back(mct);
            }
            else
            {
                QVector<MotorConditionTable> mcts;
                mcts.push_back(mct);
                GlobalVariable::motorConditionTables[mcode_s] = mcts;
            }
        }
    }
}

void MainWindow::newFile()
{
    /*
    MdiChild *child = createMdiChild();
    child->newFile();
    child->showMaximized();
    */
}

void MainWindow::open()
{
    QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Import file will cover current data file,Are you sure import the file?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {
        bool flag = false;
        const QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
        {
            QString cur_path = QCoreApplication::applicationDirPath();
            QString dbfile = cur_path + "/data_mf.db";
            QString dbfile_backup = cur_path + "/data_mf_backup.db";

            QFile f_file(dbfile_backup);

            if(f_file.exists())
            {
                QFile::remove(dbfile_backup);
            }
            if(QFile::copy(dbfile,dbfile_backup))
            {
                db->close();
                QFile f_f_file(dbfile);
                if(f_f_file.remove())
                {
                    if(QFile::copy(fileName,dbfile))
                    {
                        flag = true;
                    }
                }
                db->reConnect();
            }
        }

        if(flag)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Import file finished!"));
            leftView->setupDeviceTree();
            leftView->setupMotorTree();
        }
        else
        {
            QMessageBox::information(this, tr("Infomation"), tr("Import file failed!"));
        }
    }
}

void MainWindow::save()
{
    /*
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
        */
}

void MainWindow::saveAs()
{
    /*
    MdiChild *child = activeMdiChild();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        MainWindow::prependToRecentFiles(child->currentFile());
    }
    */
    QString filename = "./data.db";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), filename, tr("Txt file(*.db);;All file(*.*)"));//getOpenFileName(this, tr("select file"),"./",tr("Db file(*.db);;All file(*.*)"));
    if(fileName.isEmpty())
    {
        QMessageBox mesg;
        mesg.warning(this,"Warning","Open file failed！");
        return;
    }
    else
    {
        QString cur_path = QCoreApplication::applicationDirPath();
        QString dbfile = cur_path + "/data_mf.db";

        QFile f_file(fileName);

        if(f_file.exists())
        {
            f_file.remove();
        }
        if(QFile::copy(dbfile,fileName))
        {
            QMessageBox::information(this, tr("Infomation"), tr("Export file finished!"));
        }
        else
        {
            QMessageBox::information(this, tr("Infomation"), tr("Export file failed!"));
        }
    }
}

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut()
{
    /*
    if (activeMdiChild())
        activeMdiChild()->cut();
    */
}

void MainWindow::copy()
{
    /*
    if (activeMdiChild())
        activeMdiChild()->copy();
    */
}

void MainWindow::paste()
{
    /*
    if (activeMdiChild())
        activeMdiChild()->paste();
    */
}
#endif

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>MFDS</b> applicatioin is using for sample data and analyse data for check the motor state"
               "include electric device,vibrate device and temprature device.\n"
               "Version 0.9.5"));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    //saveAct->setEnabled(hasMdiChild);
    //saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
    //pasteAct->setEnabled(hasMdiChild);
#endif
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    //nextAct->setEnabled(hasMdiChild);
    //previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);

#ifndef QT_NO_CLIPBOARD
    /*
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    */
#endif

    int ulen = utab->count();
    for(int i=ulen-1;i>=0;i--)
    {
        utab->removeTab(i);
    }

    ulen = mdiArea->subWindowList().size();
    if(ulen>1)
    {
        utab->setFixedHeight(20);
        //utab->setFixedWidth((ulen * 150));
        utab->updateGeometry();
        QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
        for (int i = 0; i < windows.size(); ++i) {
            QMdiSubWindow *mdiSubWindow = windows.at(i);
            MDISubWidget *child = qobject_cast<MDISubWidget *>(mdiSubWindow->widget());

            utab->addTab(child->title());
            if(child == activeMdiChild())
            {
                utab->setCurrentIndex(i);
            }
        }
    }
    else
    {
        utab->setFixedHeight(1);
        utab->updateGeometry();
    }
}

void MainWindow::tabBarClickedSlot(int index)
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);

        if(i==index)
        {
            mdiArea->setActiveSubWindow(mdiSubWindow);
            break;
        }
    }
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    /*
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    */
    //windowMenu->addAction(windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MDISubWidget *child = qobject_cast<MDISubWidget *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->title());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->title());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;

    mdiArea->addSubWindow(child);
    //child->showMaximized();

#ifndef QT_NO_CLIPBOARD
    //connect(child, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    //connect(child, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif

    return child;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QToolBar *fileToolBar = addToolBar(tr("File"));

    /*
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);
    */

    const QIcon openIcon = QIcon(":/images/import.png");
    openAct = new QAction(openIcon, tr("&Import"), this);
    //openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Import a file"));
    openAct->setEnabled(false);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    /*
    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileToolBar->addAction(saveAct);
    */

    const QIcon saveAsIcon = QIcon(":/images/export.png");
    saveAsAct = new QAction(saveAsIcon, tr("&Export"), this);
    //saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Export data to a file"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);
    fileToolBar->addAction(saveAsAct);
    fileMenu->addSeparator();

    recordwaveAct = new QAction(tr("Record Waves"), this);
    recordwaveAct->setStatusTip(tr("Record Waves"));
    connect(recordwaveAct, &QAction::triggered, this, &MainWindow::recoreWaveSlot);
    fileMenu->addAction(recordwaveAct);
    fileMenu->addSeparator();

    databaseAct = new QAction(tr("Database"), this);
    databaseAct->setStatusTip(tr("Database"));
    databaseAct->setEnabled(false);
    connect(databaseAct, &QAction::triggered, this, &MainWindow::databaseSlot);
    fileMenu->addAction(databaseAct);
    fileMenu->addSeparator();

    /*
    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());
    */
    fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

    fileMenu->addSeparator();

//! [0]
    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
//! [0]

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    /*
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    editMenu->addSeparator();
    */

    //QMenu *dm = editMenu->addMenu(tr("Device Manage"));
    const QIcon ndIcon = QIcon(":/images/newd.png");
    QAction *newDeviceModel = new QAction(tr("Device Model"));
    connect(newDeviceModel,&QAction::triggered,this,&MainWindow::devicetypemanager);
    editMenu->addAction(newDeviceModel);
    QAction *newDevice = new QAction(ndIcon,tr("New Device"));
    connect(newDevice,&QAction::triggered,leftView,&LeftTreeView::newDeviceSlot);
    editMenu->addAction(newDevice);
    editToolBar->addAction(newDevice);
    editDevice = new QAction(tr("Edit Device"),this);
    //connect(editDevice,&QAction::triggered,leftView,&LeftTreeView::newDeviceSlot);
    editMenu->addAction(editDevice);

    editMenu->addSeparator();
    const QIcon nmIcon = QIcon(":/images/newm.png");
    //QMenu *mm = editMenu->addMenu(tr("Motor Manage"));
    QAction *newMotorModel = new QAction(tr("Motor Model"),this);
    connect(newMotorModel,&QAction::triggered,this,&MainWindow::motortypemanager);
    editMenu->addAction(newMotorModel);
    QAction *newBearModel = new QAction(tr("Bear Model"),this);
    connect(newBearModel,&QAction::triggered,this,&MainWindow::bearmanager);
    editMenu->addAction(newBearModel);
    QAction *newMotor = new QAction(nmIcon,tr("New Monitor Device"),this);
    connect(newMotor,&QAction::triggered,leftView,&LeftTreeView::newMotorSlot);
    editMenu->addAction(newMotor);
    EditMotor = new QAction(tr("Edit Monitor Device"),this);
    editMenu->addAction(EditMotor);
    editToolBar->addAction(newMotor);

    //menuBar()->addMenu(tr("&View"));
    menuBar()->addMenu(tr("&Alarm"));

    if(GlobalVariable::s_t != 3)
    {
        QMenu *analyseMenu = menuBar()->addMenu(tr("A&nalyse"));
        QAction* fftparam = new QAction(tr("FFT Parameter setting"), this);
        fftparam->setStatusTip(tr("FFT parameter setting"));
        analyseMenu->addAction(fftparam);
        connect(fftparam, &QAction::triggered, this, &MainWindow::fftparamSettingSlot);
    }

    QMenu *optionMenu = menuBar()->addMenu(tr("&Option"));

    if(GlobalVariable::s_t != 3)
    {
        QAction* chart_note = new QAction(tr("Parameter setting"), this);
        chart_note->setStatusTip(tr("setting parameters"));
        optionMenu->addAction(chart_note);
        connect(chart_note, &QAction::triggered, this, &MainWindow::chartnodeSettingSlot);
    }

    if(GlobalVariable::version >=2)
    {
        QAction* diagnoseAction = new QAction(tr("Diagnose setting"), this);
        diagnoseAction->setStatusTip(tr("Diagnose setting"));
        optionMenu->addAction(diagnoseAction);
        connect(diagnoseAction, &QAction::triggered, this, &MainWindow::diagnoseSettingSlot);
    }

    QAction* serverparamsetting = new QAction(tr("Server parameters setting"), this);
    serverparamsetting->setStatusTip(tr("setting server parameters"));
    optionMenu->addAction(serverparamsetting);
    connect(serverparamsetting, &QAction::triggered, this, &MainWindow::serverparameterSettingSlot);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    QToolBar *windowToolBar = addToolBar(tr("Window"));
    connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);

    const QIcon claseallIcon = QIcon(":/images/closeAllWindow.png");
    closeAllAct = new QAction(claseallIcon,tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    windowToolBar->addAction(closeAllAct);
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    const QIcon tileIcon = QIcon(":/images/tile.png");
    tileAct = new QAction(tileIcon,tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    windowToolBar->addAction(tileAct);
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    const QIcon cascadeIcon = QIcon(":/images/casacade.png");
    cascadeAct = new QAction(cascadeIcon,tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    windowToolBar->addAction(cascadeAct);
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    /*
    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);
    */

    windowMenuSeparatorAct = new QAction(this);
    windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About us"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    //QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    //aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createStatusBar()
{
    //statusBar()->showMessage(tr("Ready"));

    QStatusBar *stateBar = this->statusBar();

    QCheckBox* isOnlineCheck = new QCheckBox();
    isOnlineCheck->setText(tr("Is Online"));
    stateBar->addWidget(isOnlineCheck);
    isOnlineCheck->setChecked(true);
    connect(isOnlineCheck,SIGNAL(stateChanged(int)),this,SLOT(sOnlineCheckedSlot(int)));

    //QCheckBox* sdataCheck = new QCheckBox();
    //sdataCheck->setText(tr("Save data"));
    //stateBar->addWidget(sdataCheck);
    //connect(sdataCheck,SIGNAL(stateChanged(int)),this,SLOT(sDataCheckedSlot(int)));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

MDISubWidget *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MDISubWidget *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const int uniqueId) const
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MDISubWidget *mdiChild = qobject_cast<MDISubWidget *>(window->widget());
        if (mdiChild->getUniqueId() == uniqueId)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    else
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::slotSplitterMoved(int pos, int index)
{
    qDebug()<<QString::number(pos);
    qDebug()<<QString::number(index);
    int splitterWidth = pSpliterMain->width() / 2;
    if(pos<=150)
    {
        int leftWidth = 150;
        QList<int> lsSize;
        lsSize<<leftWidth;
        lsSize<<pSpliterMain->width()-leftWidth;
        pSpliterMain->setSizes(lsSize);
    }
    else if(pos >= splitterWidth)
    {
        int leftWidth = splitterWidth;
        QList<int> lsSize;
        lsSize<<leftWidth;
        lsSize<<pSpliterMain->width()-leftWidth;
        pSpliterMain->setSizes(lsSize);
    }
}

void MainWindow::createtables()
{
    QString sql = "CREATE TABLE if not exists setting (id integer NOT NULL PRIMARY KEY,attr_value varchar(50) NOT NULL, description varchar(500))";
    db->createtable(sql);

    // setting init data
    {
        sql = "select * from setting where id=1";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (1,'1','com_enable')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=2";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (2,'1','show_enable')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=3";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (3,'0','virtual_data_enable')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=4";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (4,'0','desktop_capture_enable')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=5";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (5,'1800','wave_sample_interval')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=6";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (6,'10','freq_sample_interval')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=7";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (7,'2','measure_sample_interval')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=8";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (8,'30','sample_waiting')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=9";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (9,'127.0.0.1','server_ip')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=10";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (10,'50','server_waiting')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=11";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (11,'default deployment','group name')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=12";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (12,'3','data save days')";
            db->updatasql(sql);
        }
        sql = "select * from setting where id=13";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (13,'0','modbus server state')";
            db->updatasql(sql);
        }
        sql = "select * from setting where id=14";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (14,'0','server state')";
            db->updatasql(sql);
        }
        sql = "select * from setting where id=15";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (15,'0','alarm threshold')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=16";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (16,'','pipe color setting')";
            db->updatasql(sql);
        }

        sql = "select * from setting where id=17";
        if (db->queryCount(sql) <= 0)
        {
            sql = "insert into setting (id,attr_value,description) values (17,'0','is sync done')";
            db->updatasql(sql);
        }
    }

    sql = "CREATE TABLE if not exists bearingtype (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, model varchar(50) NOT NULL, btype varchar(50) NOT NULL, rin real NOT NULL, rout real NOT NULL, contact_angle real NULL,bearpitch real NULL,rotated real NULL,rotaten integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "select * from bearingtype where model='GeneralBear'";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into bearingtype (model,btype,rin,rout,contact_angle,bearpitch,rotated,rotaten) values ('GeneralBear','antifriction bearing',25,52,0,39.04,7.94,9)";
        db->updatasql(sql);
    }

    sql = "CREATE TABLE if not exists device (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, code varchar(50) NULL, name varchar(50) NULL, dmodel varchar(50) NULL, dtype varchar(50) NULL, ipaddress varchar(50) NULL, port integer NULL, isactive INTEGER NOT NULL  DEFAULT 1, treeid integer NULL, minterval integer NOT NULL DEFAULT 2,finterval integer NOT NULL DEFAULT 30,winterval integer NOT NULL DEFAULT 1800,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE devicetype (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, model varchar(50) NULL, dtype varchar(50) NULL, pipenum integer NOT NULL  DEFAULT (1), description varchar(2048) NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    // device type init data
    {
        if(GlobalVariable::s_t == 0 || GlobalVariable::s_t == 2)
        {
            sql = "select * from devicetype where model='CKYB0001TAGQ'";
            if (db->queryCount(sql) <= 0)
            {
                sql = "insert into devicetype (model,dtype,pipenum,description) values ('CKYB0001TAGQ','E',3,'电量采集器')";
                db->updatasql(sql);
            }
        }

        if(GlobalVariable::s_t == 0 || GlobalVariable::s_t == 20)
        {
            sql = "select * from devicetype where model='TW888'";
            if (db->queryCount(sql) <= 0)
            {
                sql = "insert into devicetype (model,dtype,pipenum,description) values ('TW888','E',1,'电量采集器')";
                db->updatasql(sql);
            }
        }

        if(GlobalVariable::s_t == 0 || GlobalVariable::s_t == 1 || GlobalVariable::s_t == 10)
        {
            sql = "select * from devicetype where model='CKYB3006LEHQ'";
            if (db->queryCount(sql) <= 0)
            {
                sql = "insert into devicetype (model,dtype,pipenum,description) values ('CKYB3006LEHQ','V',6,'振动采集器')";
                db->updatasql(sql);
            }
        }

        if(GlobalVariable::s_t == 0 || GlobalVariable::s_t == 3)
        {
            sql = "select * from devicetype where model='CKYB5003TEMQ'";
            if (db->queryCount(sql) <= 0)
            {
                sql = "insert into devicetype (model,dtype,pipenum,description) values ('CKYB5003TEMQ','T',12,'温度采集器')";
                db->updatasql(sql);
            }
        }
    }

    sql = "CREATE TABLE if not exists motor (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, mcode varchar(50) NOT NULL, name varchar(50) NOT NULL, motor_type varchar(50) NOT NULL, bearing_type varchar(50) NOT NULL,manufacture_date varchar(50) NULL, treeid integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists motorgroup (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, name varchar(100) NOT NULL, address varchar(200) NULL, macAddress varchar(100) NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists motortype (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, model varchar(50) NOT NULL, work_mode varchar(50) NOT NULL, power_rating real NULL, rated_voltage real NULL, rated_current real NULL, poleNums integer NULL, center_height real NULL, factor real NULL, insulate varchar(50) NULL, rotate real NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "select * from motortype where model='GeneralMotor'";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into motortype (model,work_mode,power_rating, rated_voltage, rated_current, poleNums, center_height, factor, insulate, rotate) values ('GeneralMotor','asynchronous',3,380,6.58,2,100,0.85,'F',2890)";
        db->updatasql(sql);
    }

    sql = "CREATE TABLE if not exists devicepipes (id integer NOT NULL PRIMARY KEY, dcode varchar(50) NULL, pipeCode integer NULL, name varchar(50) NULL, motor varchar(50) NULL, isactive INTEGER NOT NULL  DEFAULT 1, channel_type INTEGER, rms1 real NULL default 10, rms2 real NULL default 3, rms3 real NULL,treeid integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electriccharge (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, u real NULL,i real NULL, f real NULL, factor real NULL, p real NULL, q real NULL, s real NULL, others varchar(100) NULL, pqs varchar(100) NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electricanalysedata (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, upsequence real NULL,unsequence real NULL, uzsequence real NULL, ipsequence real NULL,insequence real NULL, izsequence real NULL, actpower real NULL, reactpower real NULL, apppower real NULL, factor real NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electricchargewave (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_data text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electricchargewavefreq (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_freq text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists temperature (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, temp real NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists vibrate (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, vibrate_e real NULL, speed_e real NULL, position_e real NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists vibratewave (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_data text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists vibratewavefreq (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_freq text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    //////////////////////////////

    sql = "CREATE TABLE if not exists motorTree (id integer NOT NULL PRIMARY KEY, type integer NOT NULL, title varchar(50) NOT NULL, pid integer NOT NULL default(0),wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists deviceTree (id integer NOT NULL PRIMARY KEY, type integer NOT NULL, title varchar(50) NOT NULL, pid integer NOT NULL default(0),wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists condition (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample real NOT NULL, rotate real NULL, interval real NULL, rksj datetime NOT NULL,dcode varchar(50) NOT NULL, channel varchar(50) NOT NULL, wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists motorcondition (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, edevice varchar(50) NULL,vdevice varchar(50) NULL, rotate real NULL, isManual int NULL,rksj datetime NOT NULL, mcode varchar(50) NOT NULL, wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists waverecord (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, name varchar(50) NOT NULL, kssj datetime NOT NULL,interval real NULL, rksj datetime NOT NULL, wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists deviceModel (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, name varchar(50) NOT NULL, type varchar(20) NOT NULL, rModel varchar(50) NOT NULL, wid integer NOT NULL default(0))";
    db->createtable(sql);

    ///// analyse result data
    sql = "CREATE TABLE if not exists acctimefeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, waveindex real NOT NULL, peakindex real NOT NULL, pulsindex real NOT NULL, kurtindex real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists accfreqfeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists spdtimefeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, waveindex real NOT NULL, peakindex real NOT NULL, pulsindex real NOT NULL, kurtindex real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists spdfreqfeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists envfreqfeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,freq13 real NULL,freq14 real NULL,freq15 real NULL,freq16 real NULL,freq17 real NULL,freq18 real NULL,freq19 real NULL,freq20 real NULL,freq21 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists voltimefeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists volfreqfeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists curtimefeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists curfreqfeat_record (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL)";
    db->createtable(sql);

    ///// analyse data
    sql = "CREATE TABLE if not exists acctimefeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, waveindex real NOT NULL, peakindex real NOT NULL, pulsindex real NOT NULL, kurtindex real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists accfreqfeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists spdtimefeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, waveindex real NOT NULL, peakindex real NOT NULL, pulsindex real NOT NULL, kurtindex real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists spdfreqfeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists envfreqfeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,freq13 real NULL,freq14 real NULL,freq15 real NULL,freq16 real NULL,freq17 real NULL,freq18 real NULL,freq19 real NULL,freq20 real NULL,freq21 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists voltimefeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists volfreqfeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists curtimefeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists curfreqfeat (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    ///// analyse result data
    sql = "CREATE TABLE if not exists acctimefeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, waveindex real NOT NULL, peakindex real NOT NULL, pulsindex real NOT NULL, kurtindex real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists accfreqfeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists spdtimefeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, waveindex real NOT NULL, peakindex real NOT NULL, pulsindex real NOT NULL, kurtindex real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists spdfreqfeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists envfreqfeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,freq13 real NULL,freq14 real NULL,freq15 real NULL,freq16 real NULL,freq17 real NULL,freq18 real NULL,freq19 real NULL,freq20 real NULL,freq21 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists voltimefeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists volfreqfeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists curtimefeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, amp real NOT NULL, rms real NOT NULL, pkpk real NOT NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists curfreqfeat_result (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, freq1 real NULL,freq2 real NULL,freq3 real NULL,freq4 real NULL, freq5 real NULL,freq6 real NULL,freq7 real NULL,freq8 real NULL,freq9 real NULL,freq10 real NULL,freq11 real NULL,freq12 real NULL,rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL,mcid integer NOT NULL)";
    db->createtable(sql);

    ///////////////////////////////
    sql = "CREATE TABLE if not exists motorconditiontable (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, u real NULL, i real NULL, rotate real NULL ,rksj datetime NOT NULL, mcode varchar(50) NOT NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists pathtracking (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, channel_tree_id_1 int NULL, channel_tree_id_2 int NULL, treeid integer NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists parktransform (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, vol_x text NULL, vol_y text NULL, vol_freq text NULL, cur_x text NULL, cur_y text NULL, cur_freq text NULL, rksj datetime NOT NULL,dcode varchar(50) NULL, mcode varchar(50) NOT NULL)";
    db->createtable(sql);
}

void MainWindow::devicetypemanager()
{
    DeviceType *type = new DeviceType(db);
    type->setAttribute(Qt::WA_DeleteOnClose);
    type->exec();
}

void MainWindow::motortypemanager()
{
    MotorType *motortype = new MotorType(db);
    motortype->setAttribute(Qt::WA_DeleteOnClose);
    motortype->exec();
}

void MainWindow::bearmanager()
{
    BearType *beartype = new BearType(db);
    beartype->setAttribute(Qt::WA_DeleteOnClose);
    beartype->exec();
}

void MainWindow::timeSendInfo()
{
    static int count = 0;

    if(!GlobalVariable::server_enable)
    {
        if (GlobalVariable::trans_queue.size()>0) {
            GlobalVariable::trans_queue.clear();
        }
        if (GlobalVariable::trans_queue_pri.size()>0) {
            GlobalVariable::trans_queue_pri.clear();
        }

        serverState->setAutoFillBackground(true);

        const QString m_red_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:red";
        serverState->setStyleSheet(m_red_SheetStyle);

        return;
    }
    if (!isconnected)
    {
        leftView->updateActionState(0);

        if (GlobalVariable::trans_queue.size()>0) {
            GlobalVariable::trans_queue.clear();
        }
        if (GlobalVariable::trans_queue_pri.size()>0)
        {
            GlobalVariable::trans_queue_pri.clear();
        }
        if (count > GlobalVariable::server_waiting)
        {
            if(nullptr != tcpSocket)
            {
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
                delete tcpSocket;
                tcpSocket = nullptr;
            }

            tcpSocket = new QTcpSocket(this);
            //检测链接信号
            connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
            //检测如果断开
            connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
            //检测如果有新可以读信号
            connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

            tcpSocket->connectToHost(GlobalVariable::server_ip,port);
            count = 0;
            isWaiting = false;
        }
        count++;

        if(nullptr != serverState)
        {
            serverState->setAutoFillBackground(true);

            const QString m_red_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:red";
            serverState->setStyleSheet(m_red_SheetStyle);
            //QPalette pal = serverState->palette();
            //pal.setColor(QPalette::Window,QColor(Qt::red));
            //serverState->setPalette(pal);
        }

        return;
    }

    if(nullptr != serverState)
    {
        serverState->setAutoFillBackground(true);
        const QString m_green_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:green";
        serverState->setStyleSheet(m_green_SheetStyle);
        //QPalette pal = serverState->palette();
        //pal.setColor(QPalette::Window,QColor(Qt::green));
        //serverState->setPalette(pal);
    }

    if(GlobalVariable::is_sync && GlobalVariable::sync_process == 0)
    {
        QString data = db->getProjectData(1);

        qDebug() << data;
        QByteArray sendBuffer;
        sendBuffer.resize(6);
        sendBuffer[0] = 0;
        sendBuffer[1] = 0xFF;
        sendBuffer[2] = 0xFF;
        sendBuffer[3] = 0;
        sendBuffer[4] = 0;
        sendBuffer[5] = 102;

        QByteArray compress_dataBuffer = qCompress(data.toUtf8());
        int compress_data_len = compress_dataBuffer.size();
        sendBuffer[3] = compress_data_len / 0xFF;
        sendBuffer[4] = compress_data_len % 0xFF;
        sendBuffer.append(compress_dataBuffer);
        GlobalVariable::trans_queue_pri.enqueue(sendBuffer);
        GlobalVariable::sync_process = 1;
    }

    if ((GlobalVariable::trans_queue.size()>0 || GlobalVariable::trans_queue_pri.size()>0) && !isWaiting) {
        QByteArray msg;
        if (GlobalVariable::trans_queue_pri.size() > 0)
        {
            msg = GlobalVariable::trans_queue_pri.dequeue();
            while(GlobalVariable::trans_queue.size()>MAX_QUEUE_NUM*2)
            {
                GlobalVariable::trans_queue.dequeue();
            }
        }
        else
        {
            msg.resize(6);
            msg[0] = 0;
            msg[1] = 0xFF;
            msg[2] = 0xFF;
            msg[3] = 0;
            msg[4] = 0;
            msg[5] = 127;

            int len_all = 20;
            bool hasdata = false;
            QByteArray dataBuffer;
            for(int i=0;i<20;i++)
            {
                dataBuffer[i] = 0;
            }
            for(int i=0;i<10;i++)
            {
                if(GlobalVariable::trans_queue.size()>0)
                {
                    QByteArray tmp = GlobalVariable::trans_queue.dequeue();
                    int tmp_len = tmp.size();
                    dataBuffer[i*2] = tmp_len / 0xFF;
                    dataBuffer[i*2+1] = tmp_len % 0xFF;
                    dataBuffer.append(tmp);
                    hasdata = true;
                }
            }
            if(!hasdata)
            {
                msg.resize(0);
            }
            else
            {
                QByteArray compress_dataBuffer = qCompress(dataBuffer);
                int compress_data_len = compress_dataBuffer.size();
                msg[3] = compress_data_len / 0xFF;
                msg[4] = compress_data_len % 0xFF;
                msg.append(compress_dataBuffer);
            }
        }
        //tcpSocket->write(msg.toLatin1(),msg.length())

        if(msg.size() > 0)
        {
            int slen = tcpSocket->write(msg);
            if (slen < msg.size())
            {
                qDebug() << "send failed";
            }
            else
            {
                tcpSocket->flush();
            }

            isWaiting = true;
        }

        count = 0;
    }
    else
    {

        if(isWaiting)
        {
            if (count > GlobalVariable::server_waiting)
            {
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
                delete tcpSocket;
                tcpSocket = nullptr;

                count = 0;
                isWaiting = false;
            }
            count++;
        }
    }
}

void MainWindow::slotConnected()
{
    isconnected = true;
}

void MainWindow::slotDisconnected()
{
    isconnected = false;
}

void MainWindow::dataReceived()
{
    isWaiting = false;
    //QTcpSocket *pSocket = qobject_cast<QTcpSocket*>(sender());
    if(nullptr != tcpSocket)
    {
        QByteArray by = tcpSocket->readAll();

        QString msg = QString::fromUtf8(by,by.length());
        //qDebug() << "Server msg :" << msg;
        int datalen = by.size();
        if(msg.startsWith("1#recordwave",Qt::CaseInsensitive))
        {
            QList<QString> keys = GlobalVariable::recordwave.keys();
            QList<QString>::ConstIterator it;
            for(it=keys.constBegin();it!=keys.constEnd();it++)
            {
                QString key = *it;
                GlobalVariable::recordwave[key] = 1;
            }
        }
        else if(msg.startsWith("102#sync",Qt::CaseInsensitive))
        {
            QStringList msgs = msg.split('#');
            if(msgs.size() >= 3)
            {
                bool tok;
                QString s_type_s = msgs[2];
                int s_type = s_type_s.toInt(&tok);
                if(s_type >= 1)
                    s_type++;
                else
                    return;

                QString state_s = msgs[3];
                int state_i = state_s.toInt(&tok);
                if(state_i == 1)
                {
                    //code is infused
                    QString code_s = msgs[4];
                    GlobalVariable::sync_process = GlobalVariable::sync_process | 0x80;
                    GlobalVariable::is_sync = false;

                    int self_type = s_type - 1;
                    QString show_msg = "";
                    switch (self_type) {
                    case 1:
                        show_msg = tr("Motor has code :") + code_s + tr(" is existed! ") + msgs[5];
                        break;
                    case 2:
                        show_msg = tr("Device has code :") + code_s + tr(" is existed! ") + msgs[5];
                        break;
                    case 3:
                        show_msg = tr("Motor Type has code :") + code_s + tr(" is existed! ") + msgs[5];
                        break;
                    case 4:
                        show_msg = tr("Bearing Type has code :") + code_s + tr(" is existed!") + msgs[5];
                        break;
                    case 5:
                        show_msg = tr("Device Type has code :") + code_s + tr(" is existed!") + msgs[5];
                        break;
                    default:
                        break;
                    }

                    int state_btn = QMessageBox::information(this, tr("Infomation"), show_msg,QMessageBox::Ok,QMessageBox::Cancel);

                    if(state_btn == QMessageBox::Ok)
                    {
                        QString data = db->getProjectData(s_type);

                        qDebug() << data;
                        QByteArray sendBuffer;
                        sendBuffer.resize(6);
                        sendBuffer[0] = 0;
                        sendBuffer[1] = 0xFF;
                        sendBuffer[2] = 0xFF;
                        sendBuffer[3] = 0;
                        sendBuffer[4] = 0;
                        sendBuffer[5] = 102;

                        QByteArray compress_dataBuffer = qCompress(data.toUtf8());
                        int compress_data_len = compress_dataBuffer.size();
                        sendBuffer[3] = compress_data_len / 0xFF;
                        sendBuffer[4] = compress_data_len % 0xFF;
                        sendBuffer.append(compress_dataBuffer);
                        GlobalVariable::trans_queue_pri.enqueue(sendBuffer);

                        int self_type = s_type - 1;
                        switch (self_type) {
                        case 1:
                            GlobalVariable::sync_process = GlobalVariable::sync_process | 0x2;
                            break;
                        case 2:
                            GlobalVariable::sync_process = GlobalVariable::sync_process | 0x4;
                            break;
                        case 3:
                            GlobalVariable::sync_process = GlobalVariable::sync_process | 0x8;
                            break;
                        case 4:
                            GlobalVariable::sync_process = GlobalVariable::sync_process | 0x10;
                            break;
                        case 5:
                            GlobalVariable::sync_process = GlobalVariable::sync_process | 0x20;
                            break;
                        case 6:
                            GlobalVariable::sync_process = GlobalVariable::sync_process | 0x40;
                            break;
                        default:
                            break;
                        }
                        if(s_type > 6)
                            GlobalVariable::is_sync = false;
                    }
                    else
                    {
                        GlobalVariable::is_sync = false;
                    }
                }
                else
                {
                    QString data = db->getProjectData(s_type);

                    qDebug() << data;
                    QByteArray sendBuffer;
                    sendBuffer.resize(6);
                    sendBuffer[0] = 0;
                    sendBuffer[1] = 0xFF;
                    sendBuffer[2] = 0xFF;
                    sendBuffer[3] = 0;
                    sendBuffer[4] = 0;
                    sendBuffer[5] = 102;

                    QByteArray compress_dataBuffer = qCompress(data.toUtf8());
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue_pri.enqueue(sendBuffer);

                    int self_type = s_type - 1;
                    QString sql="";
                    switch (self_type) {
                    case 1:
                        GlobalVariable::sync_process = GlobalVariable::sync_process | 0x2;
                        sql = "update motor set wid=1";
                        db->execSql(sql);
                        break;
                    case 2:
                        GlobalVariable::sync_process = GlobalVariable::sync_process | 0x4;
                        sql = "update device set wid=1";
                        db->execSql(sql);
                        break;
                    case 3:
                        GlobalVariable::sync_process = GlobalVariable::sync_process | 0x8;
                        sql = "update motortype set wid=1";
                        db->execSql(sql);
                        break;
                    case 4:
                        GlobalVariable::sync_process = GlobalVariable::sync_process | 0x10;
                        sql = "update bearingtype set wid=1";
                        db->execSql(sql);
                        break;
                    case 5:
                        GlobalVariable::sync_process = GlobalVariable::sync_process | 0x20;
                        sql = "update devicetype set wid=1";
                        db->execSql(sql);
                        break;
                    case 6:
                        GlobalVariable::sync_process = GlobalVariable::sync_process | 0x40;
                        break;
                    default:
                        break;
                    }
                    if(s_type > 6)
                        GlobalVariable::is_sync = false;
                }
            }
        }
        //qDebug() << "receive data lenght: " << datalen;
    }
}

void MainWindow::connectDevice()
{
    //sendThread = new SendDataThread();
    //sendThread->start();

    devicelink->connectDevice();

    serverIP = GlobalVariable::server_ip;
    port = 65134;

    if(GlobalVariable::server_enable)
    {
        tcpSocket = new QTcpSocket(this);
        //检测链接信号
        connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
        //检测如果断开
        connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
        //检测如果有新可以读信号
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

        tcpSocket->connectToHost(serverIP,port);
    }

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeDiagnose()));
    timer->start(BASE_INTERVAL*2);

    QTimer *timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(timeSendInfo()));
    timer1->start(BASE_INTERVAL);

    /*
    QByteArray sendBuffer;
    sendBuffer.resize(6);
    sendBuffer[0] = 0;
    sendBuffer[1] = 0xFF;
    sendBuffer[2] = 0xFF;
    sendBuffer[3] = 0;
    sendBuffer[4] = 0;
    sendBuffer[5] = 101;
    if (MainWindow::group_name.length()>0)
    {
        QByteArray compress_dataBuffer = qCompress(MainWindow::group_name.toUtf8());
        int compress_data_len = compress_dataBuffer.size();
        sendBuffer[3] = compress_data_len / 0xFF;
        sendBuffer[4] = compress_data_len % 0xFF;
        sendBuffer.append(compress_dataBuffer);
        MainWindow::trans_queue_pri.enqueue(sendBuffer);
    }
    */
}

void MainWindow::linkStateChangedSlot(int treeId,bool state)
{
    leftView->setDeviceLinkState(treeId,state);
}

void MainWindow::treeSelectedChangeSlot(int nodetype)
{
    TREENODETYPE itemType = (TREENODETYPE)nodetype;
    EditMotor->setEnabled(false);
    editDevice->setEnabled(false);

    switch (itemType) {
    case MOTORROOT:

        break;
    case MOTOR:
        EditMotor->setEnabled(true);
        break;
    case CHANNEL_PHYISIC:
        break;
    case MEASURE:
        break;
    case WAVE:
        break;
    case FREQENCY:
        break;
    case DEVICEROOT:
        break;
    case DEVICE:
        editDevice->setEnabled(true);
        break;
    default:
        break;
    }
}

void MainWindow::showRightControllerSlot(int pid,int id,QString title,int type,QString f_t,QString e_t,QString ts_t,int wid,int type_s)
{
    int ulen = mdiArea->subWindowList().size();
    offlinewave_ts = ts_t;
    int maxsubwindows = 3;
    if(1 == type_s)
    {
        maxsubwindows = 4;
    }
    if(ulen>maxsubwindows)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Max show window is limit to 4!"));
        return;
    }
    TREENODETYPE tn = (TREENODETYPE)type;

    QVector<QString> deviceCode;
    QVector<QString> channelCode;

    QString title_legend = "";
    bool tok;

    if(tn != PATH_TRACKING && tn != DIAGNOSE_REPORT)
    {
        QVector<ChannelInfo*>::ConstIterator cIt;
        cIt = GlobalVariable::channelInfos.constBegin();

        for(;cIt != GlobalVariable::channelInfos.constEnd(); cIt++)
        {
            ChannelInfo* ci = *cIt;
            if(ci->tree_id == pid)
            {
                deviceCode.append(ci->device_code);
                QString channel = ci->channel_code;
                channelCode.append(channel.trimmed());
                break;
            }
        }
    }
    else if(tn == PATH_TRACKING)
    {
        QString sql = "select * from pathtracking where treeid=" + QString::number(id);
        QVector<QString> channel_ids = db->execSql(sql);
        if(channel_ids.size()>0)
        {
            QVector<QString>::ConstIterator it;
            for(it=channel_ids.constBegin();it!=channel_ids.constEnd();it++)
            {
                QString v_id = *it;
                QStringList ids_items = v_id.split(',');
                if(ids_items.size()>=4)
                {
                    QString tmp = ids_items[1];
                    int x_id = tmp.toInt(&tok);
                    tmp = ids_items[2];
                    int y_id = tmp.toInt(&tok);

                    QVector<ChannelInfo*>::ConstIterator cIt;
                    cIt = GlobalVariable::channelInfos.constBegin();

                    for(;cIt != GlobalVariable::channelInfos.constEnd(); cIt++)
                    {
                        ChannelInfo* ci = *cIt;
                        if(ci->tree_id == x_id)
                        {
                            deviceCode.append(ci->device_code);
                            QString channel = ci->channel_code;
                            channelCode.append(channel.trimmed());
                            break;
                        }
                    }

                    cIt = GlobalVariable::channelInfos.constBegin();
                    for(;cIt != GlobalVariable::channelInfos.constEnd(); cIt++)
                    {
                        ChannelInfo* ci = *cIt;
                        if(ci->tree_id == y_id)
                        {
                            deviceCode.append(ci->device_code);
                            QString channel = ci->channel_code;
                            channelCode.append(channel.trimmed());
                            break;
                        }
                    }
                }
            }
        }
    }

    QVector<TreeNodeInfo*>::ConstIterator tnIt;
    for(tnIt=GlobalVariable::motorTreeInfos.constBegin();tnIt != GlobalVariable::motorTreeInfos.constEnd(); tnIt++)
    {
        TreeNodeInfo* ci = *tnIt;
        if(ci->id == pid)
        {
            title_legend = ci->title;
            break;
        }
    }

    if(1 == type_s)
    {
        //if (QMdiSubWindow *existing = findMdiChild(id)) {
            //mdiArea->setActiveSubWindow(existing);
        //}
        int tmp_base,tmp_cur;
        if(deviceCode.size()>0)
        {
            foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
                FeartureChartWidget *mdiChild = qobject_cast<FeartureChartWidget *>(window->widget());
                if(mdiChild == nullptr)
                {
                    SurfaceChart *mdiChild = qobject_cast<SurfaceChart *>(window->widget());
                    if(mdiChild == nullptr)
                    {
                        FaultWidget *mdiChild = qobject_cast<FaultWidget *>(window->widget());
                        if(mdiChild == nullptr)
                        {
                            ParkTransformationWidget *mdiChild = qobject_cast<ParkTransformationWidget *>(window->widget());
                            if(mdiChild == nullptr)
                            {

                            }
                            else
                            {
                                TREENODETYPE tn = leftView->getCurNodeType();
                                if(GlobalVariable::findNodeType(tn) ==2)
                                {
                                    QString mcode = mdiChild->getMcode();

                                    QString dcode = deviceCode[0];
                                    QString condi = db->getConditionByCodeTime(dcode,ts_t);
                                    QStringList con_items = condi.split(',');
                                    if(con_items.size() >= 3)
                                    {
                                        QString sql = "select vol_x,vol_y,vol_freq,cur_x,cur_y,cur_freq from parktransform where mcode='" + mcode + "' and rksj='" + ts_t + "'";
                                        QVector<QString> parks = db->execSql(sql,"#");
                                        if(parks.size()>0)
                                        {
                                            QVector<QString>::ConstIterator pit;
                                            for(pit=parks.constBegin();pit!=parks.constEnd();pit++)
                                            {
                                                QString row_str = *pit;
                                                QStringList row_items = row_str.split('#');
                                                if(row_items.size()>=6)
                                                {
                                                    mdiChild->setOfflineData(row_str,con_items[0],con_items[2]);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {

                        }
                        continue;
                    }
                    else
                    {
                        if (mdiChild->getAUXId() == pid)
                        {
                            QString off_data="";
                            TREENODETYPE chart_tn = mdiChild->getChartType();
                            QVector<QString> msgs;
                            QVector<QString>::ConstIterator cit;

                            QString dcode = deviceCode[0];
                            QString condi = db->getConditionByCodeTime(dcode,ts_t);
                            QStringList con_items = condi.split(',');
                            if(con_items.size() >= 3)
                            {
                                switch (chart_tn) {
                                case WATERFALL_ACC:
                                    msgs = db->getFreqsByInterval(VIBRATEFREQINFO,f_t,e_t,deviceCode[0],channelCode[0],GlobalVariable::waterfalldepthlimit);
                                    break;
                                case WATERFALL_SPD:
                                    msgs = db->getFreqsByInterval(VIBRATEFREQINFO_S,f_t,e_t,deviceCode[0],channelCode[0],GlobalVariable::waterfalldepthlimit);
                                    break;
                                case WATERFALL_VOL:
                                    if(leftView->getCurNodeType() == UWAVE || leftView->getCurNodeType() == UFREQENCY || leftView->getCurNodeType() == UMEASURE || leftView->getCurNodeType() == WATERFALL_VOL)
                                    {
                                        msgs = db->getFreqsByInterval(CHARGEFREQINFO,f_t,e_t,deviceCode[0],channelCode[0],GlobalVariable::waterfalldepthlimit);
                                    }
                                    break;
                                case WATERFALL_CUR:
                                    if(leftView->getCurNodeType() == IWAVE || leftView->getCurNodeType() == IFREQENCY || leftView->getCurNodeType() == IMEASURE || leftView->getCurNodeType() == WATERFALL_CUR)
                                    {
                                        msgs = db->getFreqsByInterval(CHARGEFREQINFO_I,f_t,e_t,deviceCode[0],channelCode[0],GlobalVariable::waterfalldepthlimit);
                                    }
                                    break;
                                default:
                                    break;
                                }
                                for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                                {
                                    QString item = *cit;
                                    off_data += item + ";";
                                }
                                if(off_data.length()>0)
                                {
                                    off_data = off_data.left(off_data.length()-1);
                                    mdiChild->setOfflineData(off_data,con_items[0],con_items[2]);
                                }
                            }
                        }
                    }
                    continue;
                }
                TREENODETYPE chart_tn = mdiChild->getChartType();
                if (mdiChild->getAUXId() == pid)
                {
                    QVector<QString>::ConstIterator cit;

                    QVector<TreeNodeInfo*>::ConstIterator tnIt;
                    int motor_id;
                    motor_id = 0;
                    for(tnIt=GlobalVariable::motorTreeInfos.constBegin();tnIt!=GlobalVariable::motorTreeInfos.constEnd();tnIt++)
                    {
                        TreeNodeInfo* tni = *tnIt;
                        if(tni->id == pid)
                        {
                            motor_id = tni->pid;
                            break;
                        }
                    }
                    QString mcode = GlobalVariable::findMotorCodeById(motor_id);
                    float rotate = this->getOfflineMotorRotate(mcode,ts_t);

                    QString off_data="";
                    QVector<QString> msgs;
                    switch (chart_tn) {
                    case MEASURE:
                        msgs = db->getMeasureByInterval(VIBRATEINFO,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count() == 2)
                                off_data += items[0] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case MEASURE_SPEED:
                        msgs = db->getMeasureByInterval(VIBRATEINFO,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count() == 2)
                                off_data += items[1] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case MEASURE_POS:
                        break;
                    case UMEASURE:
                        msgs = db->getMeasureByInterval(CHARGEINFO,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()==2)
                                off_data += items[0] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case IMEASURE:
                        msgs = db->getMeasureByInterval(CHARGEINFO,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()==2)
                                off_data += items[1] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case ACC_FREQ_HX:
                    case ACC_FREQ_1X:
                    case ACC_FREQ_2X:
                    case ACC_FREQ_3X:
                    case ACC_FREQ_4X:
                    case ACC_FREQ_5X:
                    case ACC_FREQ_6X:
                    case ACC_FREQ_7X:
                    case ACC_FREQ_8X:
                    case ACC_FREQ_9X:
                    case ACC_FREQ_10X:
                        tmp_base = (int)ACC_FREQ_HX;
                        tmp_cur = (int)chart_tn;
                        msgs = db->getMeasureByInterval(ACCFREQFEAT,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()==11)
                                off_data += items[tmp_cur-tmp_base] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case SPD_FREQ_HX:
                    case SPD_FREQ_1X:
                    case SPD_FREQ_2X:
                    case SPD_FREQ_3X:
                    case SPD_FREQ_4X:
                    case SPD_FREQ_5X:
                    case SPD_FREQ_6X:
                    case SPD_FREQ_7X:
                    case SPD_FREQ_8X:
                    case SPD_FREQ_9X:
                    case SPD_FREQ_10X:
                        tmp_base = (int)SPD_FREQ_HX;
                        tmp_cur = (int)chart_tn;
                        msgs = db->getMeasureByInterval(SPDFREQFEAT,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()==11)
                                off_data += items[tmp_cur-tmp_base] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case VOL_FREQ_HX:
                    case VOL_FREQ_1X:
                    case VOL_FREQ_2X:
                    case VOL_FREQ_3X:
                    case VOL_FREQ_4X:
                    case VOL_FREQ_5X:
                    case VOL_FREQ_6X:
                    case VOL_FREQ_7X:
                    case VOL_FREQ_8X:
                    case VOL_FREQ_9X:
                    case VOL_FREQ_10X:
                        tmp_base = (int)VOL_FREQ_HX;
                        tmp_cur = (int)chart_tn;
                        msgs = db->getMeasureByInterval(VOLFREQFEAT,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()==11)
                                off_data += items[tmp_cur-tmp_base] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case CUR_FREQ_HX:
                    case CUR_FREQ_1X:
                    case CUR_FREQ_2X:
                    case CUR_FREQ_3X:
                    case CUR_FREQ_4X:
                    case CUR_FREQ_5X:
                    case CUR_FREQ_6X:
                    case CUR_FREQ_7X:
                    case CUR_FREQ_8X:
                    case CUR_FREQ_9X:
                    case CUR_FREQ_10X:
                        tmp_base = (int)CUR_FREQ_HX;
                        tmp_cur = (int)chart_tn;
                        msgs = db->getMeasureByInterval(CURFREQFEAT,f_t,e_t,deviceCode[0],channelCode[0]);
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()==11)
                                off_data += items[tmp_cur-tmp_base] + ",";
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case ACTPOWER:
                    case REACTPOWER:
                    case APPPOWER:
                    case COSF:
                    case U_P_SEQUENCE:
                    case U_N_SEQUENCE:
                    case U_Z_SEQUENCE:
                    case I_P_SEQUENCE:
                    case I_N_SEQUENCE:
                    case I_Z_SEQUENCE:
                        off_data = "select * from electricanalysedata where rksj >= '" + f_t + "' and rksj <='" + e_t + "' and dcode='" + deviceCode[0] + "' and pipe=" + channelCode[0];
                        msgs = db->execSql(off_data);
                        off_data = "";
                        for(cit=msgs.constBegin();cit!=msgs.constEnd();cit++)
                        {
                            QString item = *cit;
                            QStringList items = item.split(',');
                            if(items.count()>=16)
                            {
                                int in_index = 0;
                                switch (chart_tn) {
                                case ACTPOWER:
                                    in_index = 7;
                                    break;
                                case REACTPOWER:
                                    in_index = 8;
                                    break;
                                case APPPOWER:
                                    in_index = 9;
                                    break;
                                case COSF:
                                    in_index = 10;
                                    break;
                                case U_P_SEQUENCE:
                                    in_index = 1;
                                    break;
                                case U_N_SEQUENCE:
                                    in_index = 2;
                                    break;
                                case U_Z_SEQUENCE:
                                    in_index = 3;
                                    break;
                                case I_P_SEQUENCE:
                                    in_index = 4;
                                    break;
                                case I_N_SEQUENCE:
                                    in_index = 5;
                                    break;
                                case I_Z_SEQUENCE:
                                    in_index = 6;
                                    break;
                                default:
                                    break;
                                }
                                off_data += items[in_index] + ",";
                            }
                        }
                        if(off_data.length()>0)
                        {
                            off_data = off_data.left(off_data.length()-1);
                            mdiChild->setOfflineData(off_data);
                        }
                        break;
                    case WAVE:
                    case FREQENCY:
                    case WAVE_A:
                    case SPEED_WAVE_A:
                    case WAVE_FREQ_A:
                    case SPEED_FREQ_A:
                    case EN_FREQ_A:
                    case DIS_WAVE_A:
                    case ACC_POWER:
                    case ACC_Cepstrum:
                    case SPD_POWER:
                    case SPD_Cepstrum:
                        {
                            QString t_s_data = "";
                            QString dcode = deviceCode[0];
                            QString condi = db->getConditionByCodeTime(dcode,ts_t);
                            QStringList con_items = condi.split(',');
                            if(con_items.size() >= 3)
                            {
                                t_s_data = db->getWaveById(VIBRATEWAVEINFO,wid);
                                if(t_s_data.length()>0)
                                {
                                    mdiChild->setOfflineData(t_s_data,con_items[0],con_items[2],rotate);
                                }
                            }
                        }
                        break;
                    case UWAVE:
                    case UFREQENCY:
                        if(leftView->getCurNodeType() == UWAVE || leftView->getCurNodeType() == UFREQENCY || leftView->getCurNodeType() == UMEASURE || leftView->getCurNodeType() == WATERFALL_VOL)
                        {
                            QString t_s_data = "";
                            QString dcode = deviceCode[0];
                            QString condi = db->getConditionByCodeTime(dcode,ts_t);
                            QStringList con_items = condi.split(',');
                            if(con_items.size() >= 3)
                            {
                                t_s_data = db->getWaveById(CHARGEWAVEINFO,wid);
                                if(t_s_data.length()>0)
                                {
                                    mdiChild->setOfflineData(t_s_data,con_items[0],con_items[2],rotate);
                                }
                            }
                        }
                        break;
                    case IWAVE:
                    case IFREQENCY:
                        if(leftView->getCurNodeType() == IWAVE || leftView->getCurNodeType() == IFREQENCY || leftView->getCurNodeType() == IMEASURE || leftView->getCurNodeType() == WATERFALL_CUR)
                        {
                            QString t_s_data = "";
                            QString dcode = deviceCode[0];
                            QString condi = db->getConditionByCodeTime(dcode,ts_t);
                            QStringList con_items = condi.split(',');
                            if(con_items.size() >= 3)
                            {
                                t_s_data = db->getWaveById(CHARGEWAVEINFO,wid);
                                if(t_s_data.length()>0)
                                {
                                    mdiChild->setOfflineData(t_s_data,con_items[0],con_items[2],rotate);
                                }
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
                else if(chart_tn == PATH_TRACKING)
                {
                    TREENODETYPE tn = leftView->getCurNodeType();
                    if(GlobalVariable::findNodeType(tn) == 1)
                    {
                        if(mdiChild->channel_code.contains(channelCode[0]) && mdiChild->device_code.contains(deviceCode[0])) //当前选择的通道是构成轨迹图所需通道的部分
                        {
                            QVector<QString>::ConstIterator cit;

                            QString mcode = GlobalVariable::findMotorCodeById(mdiChild->getAUXId());
                            float rotate = this->getOfflineMotorRotate(mcode,ts_t);

                            QString dcode = deviceCode[0];
                            QString condi = db->getConditionByCodeTime(dcode,ts_t);
                            QStringList con_items = condi.split(',');
                            if(con_items.size() >= 3)
                            {
                                QVector<QString>::ConstIterator in_sIt;
                                QString sql = "select sample_data,dcode,pipe from vibratewave where dcode='" + mdiChild->device_code[0] + "' and pipe='" + mdiChild->channel_code[0] + "' and rksj='" + ts_t + "' UNION select sample_data,dcode,pipe from vibratewave where dcode='" + mdiChild->device_code[1] + "' and pipe='" + mdiChild->channel_code[1] + "' and rksj='" + ts_t + "'";
                                //qDebug()<<sql;
                                QVector<QString> datas = db->execSql(sql,"#");

                                QString xAxis="";
                                QString yAxis="";
                                for(in_sIt=datas.constBegin();in_sIt!=datas.constEnd();in_sIt++)
                                {
                                    QString item = *in_sIt;
                                    QStringList item_v = in_sIt->split('#');
                                    if(item_v.length() >= 3)
                                    {
                                        QString data = item_v[0];
                                        QString dcode = item_v[1];
                                        QString channel = item_v[2];

                                        if(dcode.compare(mdiChild->device_code[0],Qt::CaseInsensitive) == 0 && channel.compare(mdiChild->channel_code[0],Qt::CaseInsensitive) == 0)
                                        {
                                            xAxis = data;
                                        }
                                        else if(dcode.compare(mdiChild->device_code[1],Qt::CaseInsensitive) == 0 && channel.compare(mdiChild->channel_code[1],Qt::CaseInsensitive) == 0)
                                        {
                                            yAxis = data;
                                        }
                                    }
                                }

                                if(xAxis.length()>0 && yAxis.length()>0)
                                {
                                    QString t_s_data = xAxis + "#" + yAxis;
                                    mdiChild->setOfflineData(t_s_data,con_items[0],con_items[2],rotate);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    else if(0 == type_s)
    {
        if (QMdiSubWindow *existing = findMdiChild(id)) {
            mdiArea->setActiveSubWindow(existing);
        }
        else
        {
            if(tn == WATERFALL_ACC || tn == WATERFALL_SPD || tn == WATERFALL_VOL || tn == WATERFALL_CUR)
            {
                SurfaceChart *child = new SurfaceChart();
                //child->setAttribute(Qt::WA_DeleteOnClose);
                child->setUniqueId(id);
                child->setAUXId(pid);
                child->setTitle(title);
                QString mcode = GlobalVariable::findMotorCodeByChannelId(pid);
                child->setMcode(mcode);
                child->setLegendBase(title_legend);
                child->setDeviceCode(deviceCode);
                child->setChannelCode(channelCode);
                child->setChartType(tn);
                mdiArea->addSubWindow(child)->resize(1000,400);
                child->showMaximized();
            }
            else if(tn == DIAGNOSE_REPORT)
            {
                FaultWidget *child = new FaultWidget();
                //child->setAttribute(Qt::WA_DeleteOnClose);
                child->setUniqueId(id);
                child->setAUXId(pid);
                child->setMcode(GlobalVariable::findMotorCodeById(pid));
                child->setTitle(title);
                mdiArea->addSubWindow(child)->resize(1000,400);
                child->showMaximized();
            }
            else if(tn == PARK_TRANSFORMATION)
            {
                ParkTransformationWidget *child = new ParkTransformationWidget();
                //child->setAttribute(Qt::WA_DeleteOnClose);
                child->setUniqueId(id);
                child->setAUXId(pid);
                child->setMcode(GlobalVariable::findMotorCodeById(pid));
                child->setTitle(title);
                mdiArea->addSubWindow(child)->resize(1000,400);
                child->showMaximized();
            }
            else
            {
                FeartureChartWidget *child = new FeartureChartWidget();
                //child->setAttribute(Qt::WA_DeleteOnClose);
                child->setUniqueId(id);
                child->setAUXId(pid);
                child->setTitle(title);
                QString mcode = GlobalVariable::findMotorCodeByChannelId(pid);
                child->setMcode(mcode);
                child->setLegendBase(title_legend);
                child->setDeviceCode(deviceCode);
                child->setChannelCode(channelCode);
                child->setChartType(tn);
                mdiArea->addSubWindow(child)->resize(1000,400);
                child->showMaximized();
            }
        }
    }
    //}
}

void MainWindow::timeDiagnose()
{
    if(GlobalVariable::version < 2)
    {
        return;
    }
    static int index = 0;
    static bool initingstudydata = false;
    static int diagnose_count = 0;
    static QString off_time = "";

    diagnose_count++;
    if(GlobalVariable::is_study_again)
    {
        QQueue<QString> sqls;
        QString sql = "delete from acctimefeat_result";
        sqls.enqueue(sql);
        sql = "delete from accfreqfeat_result";
        sqls.enqueue(sql);
        sql = "delete from spdtimefeat_result";
        sqls.enqueue(sql);
        sql = "delete from spdfreqfeat_result";
        sqls.enqueue(sql);
        sql = "delete from envfreqfeat_result";
        sqls.enqueue(sql);

        sql = "delete from voltimefeat_result";
        sqls.enqueue(sql);
        sql = "delete from volfreqfeat_result";
        sqls.enqueue(sql);
        sql = "delete from curtimefeat_result";
        sqls.enqueue(sql);
        sql = "delete from curfreqfeat_result";
        sqls.enqueue(sql);

        sql = "delete from acctimefeat";
        sqls.enqueue(sql);
        sql = "delete from accfreqfeat";
        sqls.enqueue(sql);
        sql = "delete from spdtimefeat";
        sqls.enqueue(sql);
        sql = "delete from spdfreqfeat";
        sqls.enqueue(sql);
        sql = "delete from envfreqfeat";
        sqls.enqueue(sql);

        sql = "delete from voltimefeat";
        sqls.enqueue(sql);
        sql = "delete from volfreqfeat";
        sqls.enqueue(sql);
        sql = "delete from curtimefeat";
        sqls.enqueue(sql);
        sql = "delete from curfreqfeat";
        sqls.enqueue(sql);

        db->execSql(sqls);
        GlobalVariable::is_study_again = false;
    }
    if(!GlobalVariable::is_study) //diagnoise process
    {
        diagnoseState->setText(tr("Diagnose Mode"));
        index = 0;
        if(!initingstudydata && (GlobalVariable::vibrate_diagnose.size() == 0 || GlobalVariable::electric_diagnose.size() == 0))
        {
            initingstudydata = true;
            GlobalVariable::vibrate_diagnose.clear();
            GlobalVariable::electric_diagnose.clear();

            QVector<QString> studyvibdatas = db->getVibrateStudyResultData();
            int datalen = studyvibdatas.size();

            QString tmp_s;
            float tmp_f;
            bool tok;

            int mcid=0;
            QString dcode="";
            QString channel = "";

            for(int k=0;k<datalen;k++)
            {
                QString item_vib = studyvibdatas.at(k);
                QStringList vib_items = item_vib.split(',');

                if(vib_items.length() == 60)
                {
                    tmp_s = vib_items[58];
                    mcid = tmp_s.toInt(&tok);
                    dcode = vib_items[56];
                    channel = vib_items[57];

                    SVibDiaFeatData svdfd;

                    // acc time feat
                    {
                        tmp_s = vib_items[0];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.AmpValue = tmp_f;

                        tmp_s = vib_items[1];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.RMSValue = tmp_f;

                        tmp_s = vib_items[2];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.PkPkValue = tmp_f;

                        tmp_s = vib_items[3];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.waveIndex = tmp_f;

                        tmp_s = vib_items[4];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.peakIndex = tmp_f;

                        tmp_s = vib_items[5];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.pulsIndex = tmp_f;

                        tmp_s = vib_items[6];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_TD.kurtIndex = tmp_f;
                    }

                    // acc freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = vib_items[7+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.accDia_FD.vibMultFrq[n] = tmp_f;
                    }

                    // spd time feat
                    {
                        tmp_s = vib_items[18];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.AmpValue = tmp_f;

                        tmp_s = vib_items[19];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.RMSValue = tmp_f;

                        tmp_s = vib_items[20];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.PkPkValue = tmp_f;

                        tmp_s = vib_items[21];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.waveIndex = tmp_f;

                        tmp_s = vib_items[22];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.peakIndex = tmp_f;

                        tmp_s = vib_items[23];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.pulsIndex = tmp_f;

                        tmp_s = vib_items[24];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_TD.kurtIndex = tmp_f;
                    }

                    // spd freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = vib_items[25+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.spdDia_FD.vibMultFrq[n] = tmp_f;
                    }

                    // env freq feat
                    for(int n=0;n<20;n++)
                    {
                        tmp_s = vib_items[36+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        svdfd.envDia_FD.envMultFrq[n] = tmp_f;
                    }

                    if(GlobalVariable::vibrate_diagnose.contains(mcid))
                    {
                        if(GlobalVariable::vibrate_diagnose[mcid].contains(dcode))
                        {
                            GlobalVariable::vibrate_diagnose[mcid][dcode][channel] = svdfd;
                        }
                        else
                        {
                            QMap<QString,SVibDiaFeatData> c_ss;
                            c_ss[channel] = svdfd;
                            GlobalVariable::vibrate_diagnose[mcid][dcode] = c_ss;
                        }
                    }
                    else
                    {
                        QMap<QString,QMap<QString,SVibDiaFeatData>> v_ss;
                        QMap<QString,SVibDiaFeatData> qss;
                        qss[channel] = svdfd;
                        v_ss[dcode] = qss;
                        GlobalVariable::vibrate_diagnose[mcid] = v_ss;
                    }
                }
            }

            //////////////////////////////////////////////////////////////////////////////
            QVector<QString> studyelcdatas = db->getElectricStudyResultData();
            datalen = studyelcdatas.size();

            for(int k=0;k<datalen;k++)
            {
                QString item_elc = studyelcdatas.at(k);
                QStringList elc_items = item_elc.split(',');

                if(elc_items.length() == 32)
                {
                    tmp_s = elc_items[30];
                    mcid = tmp_s.toInt(&tok);
                    dcode = elc_items[28];
                    channel = elc_items[29];

                    SElcDiaFeatData sedfd;

                    // vol time feat
                    {
                        tmp_s = elc_items[0];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.volDia_TD.AmpValue = tmp_f;

                        tmp_s = elc_items[1];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.volDia_TD.RMSValue = tmp_f;

                        tmp_s = elc_items[2];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.volDia_TD.PkPkValue = tmp_f;
                    }

                    // vol freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = elc_items[3+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.volDia_FD.elcMultFrq[n] = tmp_f;
                    }

                    // cur time feat
                    {
                        tmp_s = elc_items[14];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.curDia_TD.AmpValue = tmp_f;

                        tmp_s = elc_items[15];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.curDia_TD.RMSValue = tmp_f;

                        tmp_s = elc_items[16];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.curDia_TD.PkPkValue = tmp_f;
                    }

                    // cur freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = elc_items[17+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sedfd.curDia_FD.elcMultFrq[n] = tmp_f;
                    }

                    if(GlobalVariable::electric_diagnose.contains(mcid))
                    {
                        if(GlobalVariable::electric_diagnose[mcid].contains(dcode))
                        {
                            GlobalVariable::electric_diagnose[mcid][dcode][channel] = sedfd;
                        }
                        else
                        {
                            QMap<QString,SElcDiaFeatData> c_ss;
                            c_ss[channel] = sedfd;
                            GlobalVariable::electric_diagnose[mcid][dcode] = c_ss;
                        }
                    }
                    else
                    {
                        QMap<QString,QMap<QString,SElcDiaFeatData>> v_ss;
                        QMap<QString,SElcDiaFeatData> qss;
                        qss[channel] = sedfd;
                        v_ss[dcode] = qss;
                        GlobalVariable::electric_diagnose[mcid] = v_ss;
                    }
                }
            }
        }
        else //diagnose
        {
            if(GlobalVariable::isOnline) // 在线诊断
            {
                if(GlobalVariable::tw888charges.size()>0)
                {
                    QList<QString> d_codes = GlobalVariable::tw888charges.keys();
                    QList<QString>::ConstIterator cit;
                    for(cit=d_codes.constBegin();cit != d_codes.constEnd(); cit++)
                    {
                        QString d_code = *cit;
                        QVector<QString> m_codes = GlobalVariable::findMotorCodeByDeviceCode(d_code);
                        if(m_codes.size()>0)
                        {
                            QString m_code = m_codes[0];

                            FaultInfo fi;
                            fi.dcode = d_code;
                            fi.fault_electric_details = *GlobalVariable::tw888charges[d_code].head();
                            fi.sample_time = fi.fault_electric_details.rksj;
                            if(GlobalVariable::diagnose_result.contains(m_code))
                            {
                                GlobalVariable::diagnose_result[m_code][d_code] = fi;
                            }
                            else
                            {
                                QMap<QString,FaultInfo> fis;
                                fis[d_code] = fi;
                                GlobalVariable::diagnose_result[m_code] = fis;
                            }
                        }
                    }
                }
                if(GlobalVariable::vibrate_diagnose.size() > 0 && GlobalVariable::electric_diagnose.size() > 0)
                {
                    QList<int> keys = GlobalVariable::vibrate_diagnose.keys();
                    QList<int>::ConstIterator it;
                    for(it=keys.constBegin();it!=keys.constEnd();it++)
                    {
                        int key = *it;

                        bool flag = false;

                        QString mcode = "";

                        QMap<QString,QVector<MotorConditionTable>>::ConstIterator mctIt;
                        for(mctIt=GlobalVariable::motorConditionTables.constBegin();mctIt!=GlobalVariable::motorConditionTables.constEnd();mctIt++)
                        {
                            QString in_key = mctIt.key();
                            QVector<MotorConditionTable> mct_v = mctIt.value();
                            QVector<MotorConditionTable>::ConstIterator mctvIt;
                            for(mctvIt = mct_v.constBegin();mctvIt!=mct_v.constEnd();mctvIt++)
                            {
                                MotorConditionTable mct = *mctvIt;
                                if(mct.id == key)
                                {
                                    if(GlobalVariable::motorCondition.contains(in_key))
                                    {
                                        mcode = in_key;

                                        int rotate_s = GlobalVariable::findMotorRotateByCode(mcode);
                                        int rotate_threshold = 250;
                                        if(rotate_s > 0)
                                            rotate_threshold = rotate_s * GlobalVariable::diagnose_rotate_diff;
                                        if(qAbs(mct.rotate - GlobalVariable::motorCondition[in_key].rotate) < rotate_threshold)
                                        {
                                            flag = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        if(!flag)
                        {
                            continue; // 该工况条件下尚未学习，没有学习数据可供参考
                        }

                        QMap<QString,QMap<QString,SVibDiaFeatData>> d_diagnoses = GlobalVariable::vibrate_diagnose[key];
                        QList<QString> d_keys = d_diagnoses.keys();
                        QList<QString>::ConstIterator d_it;
                        for(d_it=d_keys.constBegin();d_it!=d_keys.constEnd();d_it++)
                        {
                            QString dcode = *d_it;
                            QMap<QString,SVibDiaFeatData> c_diagnose = GlobalVariable::vibrate_diagnose[key][dcode];
                            QList<QString> c_keys = c_diagnose.keys();
                            QList<QString>::ConstIterator c_it;
                            for(c_it=c_keys.constBegin();c_it!=c_keys.constEnd();c_it++)
                            {
                                QString channel = *c_it;

                                SVibDiaFeatData svdf = GlobalVariable::vibrate_diagnose[key][dcode][channel];

                                QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                                if(GlobalVariable::vibrate_analyse.contains(dcode))
                                {
                                    if(GlobalVariable::vibrate_analyse[dcode].contains(channel))
                                    {
                                        if(GlobalVariable::vibrate_analyse[dcode][channel].size()>0)
                                        {
                                            VibrateAnyseDataLib* info = GlobalVariable::vibrate_analyse[dcode][channel].back();

                                            // 赋值用户自定义参数
                                            int motortreeid = GlobalVariable::findMotorTreeIndexByCode(mcode);
                                            MotorInfo* mi = GlobalVariable::findMotorByTreeId(motortreeid);

                                            QVector<ChannelInfo*> m_channels = GlobalVariable::findChannelsByMotorId(motortreeid);
                                            QVector<ChannelInfo*>::ConstIterator cicIt;
                                            for(cicIt=m_channels.constBegin();cicIt!=m_channels.constEnd();cicIt++)
                                            {
                                                ChannelInfo* ci = *cicIt;
                                                if(ci->device_code.compare(dcode,Qt::CaseInsensitive) == 0 && ci->channel_code.compare(channel,Qt::CaseInsensitive) == 0)
                                                {
                                                    //svdf.accDia_TD.RMSValue = ci->rms1;
                                                    //svdf.spdDia_TD.RMSValue = ci->rms2;
                                                }
                                            }
                                            SVibDiaFeatData diag;
                                            for(int n=0;n<11;n++)
                                            {
                                                diag.accDia_FD.vibMultFrq[n] = info->accFreqFeat.vibMultFrq[n];
                                            }

                                            diag.accDia_TD.AmpValue = info->accTimeFeat.AmpValue;
                                            diag.accDia_TD.RMSValue = info->accTimeFeat.RMSValue;
                                            diag.accDia_TD.PkPkValue = info->accTimeFeat.PkPkValue;
                                            diag.accDia_TD.waveIndex = info->accTimeFeat.waveIndex;
                                            diag.accDia_TD.peakIndex = info->accTimeFeat.peakIndex;
                                            diag.accDia_TD.pulsIndex = info->accTimeFeat.pulsIndex;
                                            diag.accDia_TD.kurtIndex = info->accTimeFeat.kurtIndex;

                                            for(int n=0;n<11;n++)
                                            {
                                                diag.spdDia_FD.vibMultFrq[n] = info->spdFreqFeat.vibMultFrq[n];
                                            }

                                            diag.spdDia_TD.AmpValue = info->spdTimeFeat.AmpValue;
                                            diag.spdDia_TD.RMSValue = info->spdTimeFeat.RMSValue;
                                            diag.spdDia_TD.PkPkValue = info->spdTimeFeat.PkPkValue;
                                            diag.spdDia_TD.waveIndex = info->spdTimeFeat.waveIndex;
                                            diag.spdDia_TD.peakIndex = info->spdTimeFeat.peakIndex;
                                            diag.spdDia_TD.pulsIndex = info->spdTimeFeat.pulsIndex;
                                            diag.spdDia_TD.kurtIndex = info->spdTimeFeat.kurtIndex;

                                            for(int n=0;n<20;n++)
                                            {
                                                diag.envDia_FD.envMultFrq[n] = info->envFreqFeat.envMultFrq[n];
                                            }

                                            // call diagnose calculate
                                            CDiagnosis diags(&svdf);
                                            diags.vibDiagnosing(&diag);

                                            FaultInfo fi;
                                            fi.sample_time = info->sample_time;
                                            fi.dcode = dcode;
                                            fi.channel = channel;
                                            fi.fault_detail_thd = diags.thrshold;
                                            fi.fault_detail_cur = diags.curValue;
                                            fi.fault_detail_cof = diags.coeffcnt;
                                            fi.fault_detail_trigger = diags.bTrigger;
                                            for(int ss=0;ss<20;ss++)
                                            {
                                                int d_r = diags.diaRsult[ss];
                                                fi.fault_codes.append(d_r);
                                            }

                                            if(GlobalVariable::diagnose_result.contains(mcode))
                                            {
                                                GlobalVariable::diagnose_result[mcode][fi.toString()] = fi;
                                            }
                                            else
                                            {
                                                QMap<QString,FaultInfo> fis;
                                                fis[fi.toString()] = fi;
                                                GlobalVariable::diagnose_result[mcode] = fis;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    //////////////////////////////////////////////////////////////////

                    keys = GlobalVariable::electric_diagnose.keys();
                    for(it=keys.constBegin();it!=keys.constEnd();it++)
                    {
                        int key = *it;

                        bool flag = false;

                        QString mcode = "";

                        QMap<QString,QVector<MotorConditionTable>>::ConstIterator mctIt;
                        for(mctIt=GlobalVariable::motorConditionTables.constBegin();mctIt!=GlobalVariable::motorConditionTables.constEnd();mctIt++)
                        {
                            QString in_key = mctIt.key();
                            QVector<MotorConditionTable> mct_v = mctIt.value();
                            QVector<MotorConditionTable>::ConstIterator mctvIt;
                            for(mctvIt = mct_v.constBegin();mctvIt!=mct_v.constEnd();mctvIt++)
                            {
                                MotorConditionTable mct = *mctvIt;
                                if(mct.id == key)
                                {
                                    if(GlobalVariable::motorCondition.contains(in_key))
                                    {
                                        mcode = in_key;
                                        int rotate_s = GlobalVariable::findMotorRotateByCode(mcode);
                                        int rotate_threshold = 250;
                                        if(rotate_s > 0)
                                            rotate_threshold = rotate_s * GlobalVariable::diagnose_rotate_diff;

                                        if(qAbs(mct.rotate - GlobalVariable::motorCondition[in_key].rotate) < rotate_threshold)
                                        {
                                            flag = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        if(!flag)
                        {
                            continue;
                        }

                        QMap<QString,QMap<QString,SElcDiaFeatData>> d_diagnoses = GlobalVariable::electric_diagnose[key];
                        QList<QString> d_keys = d_diagnoses.keys();
                        QList<QString>::ConstIterator d_it;
                        for(d_it=d_keys.constBegin();d_it!=d_keys.constEnd();d_it++)
                        {
                            QString dcode = *d_it;
                            QMap<QString,SElcDiaFeatData> c_diagnose = GlobalVariable::electric_diagnose[key][dcode];
                            QList<QString> c_keys = c_diagnose.keys();
                            QList<QString>::ConstIterator c_it;
                            for(c_it=c_keys.constBegin();c_it!=c_keys.constEnd();c_it++)
                            {
                                QString channel = *c_it;

                                SElcDiaFeatData sedf = GlobalVariable::electric_diagnose[key][dcode][channel];

                                QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
                                if(GlobalVariable::electric_analyse.contains(dcode))
                                {
                                    if(GlobalVariable::electric_analyse[dcode].contains(channel))
                                    {
                                        if(GlobalVariable::electric_analyse[dcode][channel].size() > 0)
                                        {
                                            ElectricAnyseDataLib* info = GlobalVariable::electric_analyse[dcode][channel].back();

                                            SElcDiaFeatData diag;
                                            for(int n=0;n<11;n++)
                                            {
                                                diag.volDia_FD.elcMultFrq[n] = info->volFreqFeat.elcMultFrq[n];
                                            }

                                            diag.volDia_TD.AmpValue = info->volTimeFeat.AmpValue;
                                            diag.volDia_TD.RMSValue = info->volTimeFeat.RMSValue;
                                            diag.volDia_TD.PkPkValue = info->volTimeFeat.PkPkValue;

                                            for(int n=0;n<11;n++)
                                            {
                                                diag.curDia_FD.elcMultFrq[n] = info->curFreqFeat.elcMultFrq[n];
                                            }

                                            diag.curDia_TD.AmpValue = info->curTimeFeat.AmpValue;
                                            diag.curDia_TD.RMSValue = info->curTimeFeat.RMSValue;
                                            diag.curDia_TD.PkPkValue = info->curTimeFeat.PkPkValue;

                                            // call diagnose calculate
                                        }
                                    }
                                }
                            }
                        }
                    }

                    //诊断数据上传
                    if(GlobalVariable::is_sync_done && GlobalVariable::server_enable && (diagnose_count %4) == 0)
                    {
                        if(isconnected)
                        {
                            //打包诊断数据上传
                            QList<QString>::ConstIterator it,in_it;
                            QList<QString> keys = GlobalVariable::diagnose_result.keys();
                            for(it=keys.constBegin();it!=keys.constEnd();it++)
                            {
                                QString mcode = *it;
                                QMap<QString,FaultInfo> fis = GlobalVariable::diagnose_result[mcode];
                                QList<QString> in_keys = fis.keys();

                                QMap<int,int> degrees;
                                QMap<int,int> fault_codes;
                                for(int i=0;i<20;i++)
                                {
                                    degrees[i] = 0;
                                    fault_codes[i] = 0;
                                }
                                for(in_it=in_keys.constBegin();in_it!=in_keys.constEnd();in_it++)
                                {
                                    QString key = *in_it;
                                    FaultInfo fi = fis[key];

                                    int in_index=0;
                                    QVector<int>::ConstIterator iIt;
                                    for(iIt=fi.fault_codes.constBegin();iIt!=fi.fault_codes.constEnd();iIt++)
                                    {
                                        int value = *iIt;
                                        if(value != -1) // fault is existed
                                        {
                                            int fault_code_1 = value/1000;
                                            int fault_code_2 = (value - fault_code_1 * 1000)/100;
                                            int fault_degree = (value - fault_code_1 * 1000 - fault_code_2 * 100)/10;

                                            if(fault_degree > degrees[in_index])
                                            {
                                                degrees[in_index] = fault_degree;
                                                fault_codes[in_index] = value;
                                            }
                                        }
                                        in_index++;
                                    }
                                }
                                QString cur_time = QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);
                                QByteArray sendBuffer;
                                sendBuffer.resize(6);
                                sendBuffer[0] = 0;
                                sendBuffer[1] = 0xFF;
                                sendBuffer[2] = 0xFF;
                                sendBuffer[3] = 0;
                                sendBuffer[4] = 0;
                                sendBuffer[5] = 80;
                                QByteArray dataBuffer;

                                QString diag_msg = mcode + "#";
                                for(int i=0;i<20;i++)
                                {
                                    diag_msg += QString::number(fault_codes[i]) + ";";
                                }
                                diag_msg = diag_msg.left(diag_msg.length()-1);
                                diag_msg += "#" + cur_time;
                                dataBuffer.append(diag_msg.toUtf8());

                                if (dataBuffer.size()>0)
                                {
                                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                                    int compress_data_len = compress_dataBuffer.size();
                                    sendBuffer[3] = compress_data_len / 0xFF;
                                    sendBuffer[4] = compress_data_len % 0xFF;
                                    sendBuffer.append(compress_dataBuffer);
                                    GlobalVariable::trans_queue_pri.enqueue(sendBuffer);
                                    //qDebug() << "diagnose info";
                                }
                            }
                        }
                    }
                }
            }
            else //离线诊断
            {
                try
                {
                    if(offlinewave_ts.compare(off_time,Qt::CaseInsensitive) == 0)
                    {
                        return;
                    }
                    else
                    {
                        off_time = offlinewave_ts;
                    }
                    QDateTime sample_time = QDateTime::fromString(offlinewave_ts,GlobalVariable::dtFormat);

                    if(!sample_time.isValid())
                    {
                        return;
                    }
                    QVector<QString> vibRecords = db->getVibrateRecordData(sample_time.toString(GlobalVariable::dtFormat));
                    QMap<QString,QMap<QString,SVibDiaFeatData>> vibofffeat;

                    int datalen = vibRecords.size();

                    QString tmp_s;
                    float tmp_f;
                    bool tok;

                    QString dcode="";
                    QString channel = "";

                    for(int k=0;k<datalen;k++)
                    {
                        QString item_vib = vibRecords.at(k);
                        QStringList vib_items = item_vib.split(',');

                        if(vib_items.length() == 59)
                        {
                            dcode = vib_items[56];
                            channel = vib_items[57];

                            SVibDiaFeatData svdfd;

                            // acc time feat
                            {
                                tmp_s = vib_items[0];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.AmpValue = tmp_f;

                                tmp_s = vib_items[1];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.RMSValue = tmp_f;

                                tmp_s = vib_items[2];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.PkPkValue = tmp_f;

                                tmp_s = vib_items[3];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.waveIndex = tmp_f;

                                tmp_s = vib_items[4];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.peakIndex = tmp_f;

                                tmp_s = vib_items[5];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.pulsIndex = tmp_f;

                                tmp_s = vib_items[6];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_TD.kurtIndex = tmp_f;
                            }

                            // acc freq feat
                            for(int n=0;n<11;n++)
                            {
                                tmp_s = vib_items[7+n];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.accDia_FD.vibMultFrq[n] = tmp_f;
                            }

                            // spd time feat
                            {
                                tmp_s = vib_items[18];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.AmpValue = tmp_f;

                                tmp_s = vib_items[19];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.RMSValue = tmp_f;

                                tmp_s = vib_items[20];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.PkPkValue = tmp_f;

                                tmp_s = vib_items[21];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.waveIndex = tmp_f;

                                tmp_s = vib_items[22];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.peakIndex = tmp_f;

                                tmp_s = vib_items[23];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.pulsIndex = tmp_f;

                                tmp_s = vib_items[24];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_TD.kurtIndex = tmp_f;
                            }

                            // spd freq feat
                            for(int n=0;n<11;n++)
                            {
                                tmp_s = vib_items[25+n];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.spdDia_FD.vibMultFrq[n] = tmp_f;
                            }

                            // env freq feat
                            for(int n=0;n<20;n++)
                            {
                                tmp_s = vib_items[36+n];
                                tmp_f = tmp_s.toFloat(&tok);
                                svdfd.envDia_FD.envMultFrq[n] = tmp_f;
                            }

                            if(vibofffeat.contains(dcode))
                            {
                                vibofffeat[dcode][channel] = svdfd;
                            }
                            else
                            {
                                QMap<QString,SVibDiaFeatData> v_ss;
                                v_ss[channel] = svdfd;
                                vibofffeat[dcode] = v_ss;
                            }
                        }
                    }

                    //////////////////////////////////////////////////////////////////////////////
                    QVector<QString> elcRecords = db->getElectricRecordData(sample_time.toString(GlobalVariable::dtFormat));
                    QMap<QString,QMap<QString,SElcDiaFeatData>> elcofffeat;

                    datalen = elcRecords.size();

                    for(int k=0;k<datalen;k++)
                    {
                        QString item_elc = elcRecords.at(k);
                        QStringList elc_items = item_elc.split(',');

                        if(elc_items.length() == 31)
                        {
                            dcode = elc_items[28];
                            channel = elc_items[29];

                            SElcDiaFeatData sedfd;

                            // vol time feat
                            {
                                tmp_s = elc_items[0];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.volDia_TD.AmpValue = tmp_f;

                                tmp_s = elc_items[1];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.volDia_TD.RMSValue = tmp_f;

                                tmp_s = elc_items[2];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.volDia_TD.PkPkValue = tmp_f;
                            }

                            // vol freq feat
                            for(int n=0;n<11;n++)
                            {
                                tmp_s = elc_items[3+n];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.volDia_FD.elcMultFrq[n] = tmp_f;
                            }
                            // cur time feat
                            {
                                tmp_s = elc_items[14];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.curDia_TD.AmpValue = tmp_f;

                                tmp_s = elc_items[15];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.curDia_TD.RMSValue = tmp_f;

                                tmp_s = elc_items[16];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.curDia_TD.PkPkValue = tmp_f;
                            }

                            // cur freq feat
                            for(int n=0;n<11;n++)
                            {
                                tmp_s = elc_items[17+n];
                                tmp_f = tmp_s.toFloat(&tok);
                                sedfd.curDia_FD.elcMultFrq[n] = tmp_f;
                            }

                            if(elcofffeat.contains(dcode))
                            {
                                elcofffeat[dcode][channel] = sedfd;
                            }
                            else
                            {
                                QMap<QString,SElcDiaFeatData> v_ss;
                                v_ss[channel] = sedfd;
                                elcofffeat[dcode] = v_ss;
                            }
                        }
                    }

                    if(GlobalVariable::vibrate_diagnose.size() > 0 && GlobalVariable::electric_diagnose.size() > 0)
                    {
                        QList<int> keys = GlobalVariable::vibrate_diagnose.keys();
                        QList<int>::ConstIterator it;
                        for(it=keys.constBegin();it!=keys.constEnd();it++)
                        {
                            int key = *it;

                            bool flag = false;

                            QString mcode = "";

                            QMap<QString,QVector<MotorConditionTable>>::ConstIterator mctIt;
                            for(mctIt=GlobalVariable::motorConditionTables.constBegin();mctIt!=GlobalVariable::motorConditionTables.constEnd();mctIt++)
                            {
                                QString in_key = mctIt.key();
                                QVector<MotorConditionTable> mct_v = mctIt.value();
                                QVector<MotorConditionTable>::ConstIterator mctvIt;
                                for(mctvIt = mct_v.constBegin();mctvIt!=mct_v.constEnd();mctvIt++)
                                {
                                    MotorConditionTable mct = *mctvIt;
                                    if(mct.id == key)
                                    {
                                        mcode = in_key;
                                        int rotate_s = GlobalVariable::findMotorRotateByCode(mcode);
                                        int rotate_threshold = 250;
                                        if(rotate_s > 0)
                                            rotate_threshold = rotate_s * GlobalVariable::diagnose_rotate_diff;
                                        float m_rotate = this->getOfflineMotorRotate(mcode,offlinewave_ts);

                                        if(qAbs(mct.rotate - m_rotate) < rotate_threshold)
                                        {
                                            flag = true;
                                            break;
                                        }
                                    }
                                }
                            }

                            if(!flag)
                            {
                                continue; // 该工况条件下尚未学习，没有学习数据可供参考
                            }

                            QMap<QString,QMap<QString,SVibDiaFeatData>> d_diagnoses = GlobalVariable::vibrate_diagnose[key];
                            QList<QString> d_keys = d_diagnoses.keys();
                            QList<QString>::ConstIterator d_it;
                            for(d_it=d_keys.constBegin();d_it!=d_keys.constEnd();d_it++)
                            {
                                QString dcode = *d_it;
                                QMap<QString,SVibDiaFeatData> c_diagnose = GlobalVariable::vibrate_diagnose[key][dcode];
                                QList<QString> c_keys = c_diagnose.keys();
                                QList<QString>::ConstIterator c_it;
                                for(c_it=c_keys.constBegin();c_it!=c_keys.constEnd();c_it++)
                                {
                                    QString channel = *c_it;

                                    SVibDiaFeatData svdf = GlobalVariable::vibrate_diagnose[key][dcode][channel];

                                    if(vibofffeat.contains(dcode))
                                    {
                                        if(vibofffeat[dcode].contains(channel))
                                        {
                                            // 赋值用户自定义参数
                                            int motortreeid = GlobalVariable::findMotorTreeIndexByCode(mcode);
                                            MotorInfo* mi = GlobalVariable::findMotorByTreeId(motortreeid);

                                            QVector<ChannelInfo*> m_channels = GlobalVariable::findChannelsByMotorId(motortreeid);
                                            QVector<ChannelInfo*>::ConstIterator cicIt;
                                            for(cicIt=m_channels.constBegin();cicIt!=m_channels.constEnd();cicIt++)
                                            {
                                                ChannelInfo* ci = *cicIt;
                                                if(ci->device_code.compare(dcode,Qt::CaseInsensitive) == 0 && ci->channel_code.compare(channel,Qt::CaseInsensitive) == 0)
                                                {
                                                    //svdf.accDia_TD.RMSValue = ci->rms1;
                                                    //svdf.spdDia_TD.RMSValue = ci->rms2;
                                                }
                                            }
                                            SVibDiaFeatData diag = vibofffeat[dcode][channel];

                                            // call diagnose calculate
                                            CDiagnosis diags(&svdf);
                                            diags.vibDiagnosing(&diag);


                                            FaultInfo fi;
                                            fi.dcode = dcode;
                                            fi.channel = channel;
                                            fi.fault_detail_thd = diags.thrshold;
                                            fi.fault_detail_cur = diags.curValue;
                                            fi.fault_detail_cof = diags.coeffcnt;
                                            fi.fault_detail_trigger = diags.bTrigger;
                                            for(int ss=0;ss<20;ss++)
                                            {
                                                int d_r = diags.diaRsult[ss];
                                                fi.fault_codes.append(d_r);
                                            }

                                            if(GlobalVariable::diagnose_result.contains(mcode))
                                            {
                                                GlobalVariable::diagnose_result[mcode][fi.toString()] = fi;
                                            }
                                            else
                                            {
                                                QMap<QString,FaultInfo> fis;
                                                fis[fi.toString()] = fi;
                                                GlobalVariable::diagnose_result[mcode] = fis;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        //////////////////////////////////////////////////////////////////

                        keys = GlobalVariable::electric_diagnose.keys();
                        for(it=keys.constBegin();it!=keys.constEnd();it++)
                        {
                            int key = *it;

                            bool flag = false;

                            QString mcode = "";

                            QMap<QString,QVector<MotorConditionTable>>::ConstIterator mctIt;
                            for(mctIt=GlobalVariable::motorConditionTables.constBegin();mctIt!=GlobalVariable::motorConditionTables.constEnd();mctIt++)
                            {
                                QString in_key = mctIt.key();
                                QVector<MotorConditionTable> mct_v = mctIt.value();
                                QVector<MotorConditionTable>::ConstIterator mctvIt;
                                for(mctvIt = mct_v.constBegin();mctvIt!=mct_v.constEnd();mctvIt++)
                                {
                                    MotorConditionTable mct = *mctvIt;
                                    if(mct.id == key)
                                    {
                                        mcode = in_key;
                                        int rotate_s = GlobalVariable::findMotorRotateByCode(mcode);
                                        int rotate_threshold = 250;
                                        if(rotate_s > 0)
                                            rotate_threshold = rotate_s * GlobalVariable::diagnose_rotate_diff;

                                        float m_rotate = this->getOfflineMotorRotate(mcode,offlinewave_ts);

                                        if(qAbs(mct.rotate - m_rotate) < rotate_threshold)
                                        {
                                            flag = true;
                                            break;
                                        }
                                    }
                                }
                            }

                            if(!flag)
                            {
                                continue;
                            }

                            QMap<QString,QMap<QString,SElcDiaFeatData>> d_diagnoses = GlobalVariable::electric_diagnose[key];
                            QList<QString> d_keys = d_diagnoses.keys();
                            QList<QString>::ConstIterator d_it;
                            for(d_it=d_keys.constBegin();d_it!=d_keys.constEnd();d_it++)
                            {
                                QString dcode = *d_it;
                                QMap<QString,SElcDiaFeatData> c_diagnose = GlobalVariable::electric_diagnose[key][dcode];
                                QList<QString> c_keys = c_diagnose.keys();
                                QList<QString>::ConstIterator c_it;
                                for(c_it=c_keys.constBegin();c_it!=c_keys.constEnd();c_it++)
                                {
                                    QString channel = *c_it;

                                    SElcDiaFeatData sedf = GlobalVariable::electric_diagnose[key][dcode][channel];

                                    if(elcofffeat.contains(dcode))
                                    {
                                        if(elcofffeat[dcode].contains(channel))
                                        {
                                            SElcDiaFeatData diag = elcofffeat[dcode][channel];

                                            // call diagnose calculate
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                catch(QException ex)
                {

                }
            }
        }
    }
    else //study progress
    {
        diagnoseState->setText(tr("Study Mode"));

        if(GlobalVariable::study_time * 60 * 30 < index)
        {
            GlobalVariable::is_study = false;

            // start study calculate
            QVector<QString> studyvibdatas = db->getVibrateStudyData();
            //QVector<QString> studyelcdatas = db->getElectricStudyData();
            int datalen = studyvibdatas.size();

            //SDiaFeatData sfd[datalen];

            QMap<QString,QVector<SVibDiaFeatData*>> studyViblists;

            QString tmp_s;
            float tmp_f;
            bool tok;

            for(int k=0;k<datalen;k++)
            {
                QString item_vib = studyvibdatas.at(k);
                QStringList vib_items = item_vib.split(',');

                if(vib_items.length() == 60)
                {
                    SVibDiaFeatData* sfd_item = new SVibDiaFeatData;

                    QString key = vib_items[58] + "-" + vib_items[56] + "-" + vib_items[57];

                    if(studyViblists.contains(key))
                    {
                        studyViblists[key].push_back(sfd_item);
                    }
                    else
                    {
                        QVector<SVibDiaFeatData*> sdf_list;
                        sdf_list.push_back(sfd_item);
                        studyViblists[key] = sdf_list;
                    }

                    // acc time feat
                    {
                        tmp_s = vib_items[0];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.AmpValue = tmp_f;

                        tmp_s = vib_items[1];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.RMSValue = tmp_f;

                        tmp_s = vib_items[2];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.PkPkValue = tmp_f;

                        tmp_s = vib_items[3];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.waveIndex = tmp_f;

                        tmp_s = vib_items[4];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.peakIndex = tmp_f;

                        tmp_s = vib_items[5];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.pulsIndex = tmp_f;

                        tmp_s = vib_items[6];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_TD.kurtIndex = tmp_f;
                    }

                    // acc freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = vib_items[7+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->accDia_FD.vibMultFrq[n] = tmp_f;
                    }

                    // spd time feat
                    {
                        tmp_s = vib_items[18];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.AmpValue = tmp_f;

                        tmp_s = vib_items[19];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.RMSValue = tmp_f;

                        tmp_s = vib_items[20];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.PkPkValue = tmp_f;

                        tmp_s = vib_items[21];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.waveIndex = tmp_f;

                        tmp_s = vib_items[22];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.peakIndex = tmp_f;

                        tmp_s = vib_items[23];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.pulsIndex = tmp_f;

                        tmp_s = vib_items[24];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_TD.kurtIndex = tmp_f;
                    }

                    // spd freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = vib_items[25+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->spdDia_FD.vibMultFrq[n] = tmp_f;
                    }

                    // env freq feat
                    for(int n=0;n<20;n++)
                    {
                        tmp_s = vib_items[36+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->envDia_FD.envMultFrq[n] = tmp_f;
                    }
                }
            }

            QList<QString> keyList = studyViblists.keys();
            QList<QString>::const_iterator it;
            for(it=keyList.constBegin();it!=keyList.constEnd();it++)
            {
                QString key = *it;
                QVector<SVibDiaFeatData*> inkeyList = studyViblists[key];
                QVector<SVibDiaFeatData*>::const_iterator sfIt;
                int sflen = inkeyList.size();
                SVibDiaFeatData sfd[sflen];
                int count_i=0;
                for(sfIt=inkeyList.constBegin();sfIt!=inkeyList.constEnd();sfIt++)
                {
                    sfd[count_i] = *(*sfIt);
                    count_i++;
                }

                SVibDiaFeatData r_out;
                CDiagnosis diag(sfd,sflen,&r_out);
                //diag.studying();

                // write data to database
                QStringList key_items = key.split('-');
                if(key_items.size() == 3)
                {
                    QString dcode = key_items[1];
                    QString mcid = key_items[0];
                    QString channel = key_items[2];

                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                    QQueue<QString> sqls;

                    float ampvalue = qIsFinite(r_out.accDia_TD.AmpValue) ? r_out.accDia_TD.AmpValue : 0.0;
                    float rmsvalue = qIsFinite(r_out.accDia_TD.RMSValue) ? r_out.accDia_TD.RMSValue : 0.0;
                    float pkpkvalue = qIsFinite(r_out.accDia_TD.PkPkValue) ? r_out.accDia_TD.PkPkValue : 0.0;
                    float waveindex = qIsFinite(r_out.accDia_TD.waveIndex) ? r_out.accDia_TD.waveIndex : 0.0;
                    float peakindex = qIsFinite(r_out.accDia_TD.peakIndex) ? r_out.accDia_TD.peakIndex : 0.0;
                    float pulsindex = qIsFinite(r_out.accDia_TD.pulsIndex) ? r_out.accDia_TD.pulsIndex : 0.0;
                    float kurtindex = qIsFinite(r_out.accDia_TD.kurtIndex) ? r_out.accDia_TD.kurtIndex : 0.0;

                    QString sql = "delete from acctimefeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from accfreqfeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from spdtimefeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from spdfreqfeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from envfreqfeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);

                    db->execSql(sqls);

                    sqls.clear();

                    sql = "insert into acctimefeat_result (amp,rms,pkpk,waveindex,peakindex,pulsindex,kurtindex,rksj,dcode,mcode,pipe,mcid) values (";
                    sql += QString::number(ampvalue,10,4) + "," + QString::number(rmsvalue,10,4) + "," + QString::number(pkpkvalue,10,4) + "," \
                         + QString::number(waveindex,10,4) + "," + QString::number(peakindex,10,4) + "," + QString::number(pulsindex,10,4) + "," + QString::number(kurtindex,10,4) + ",'" \
                            + StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    sql = "insert into accfreqfeat_result (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                    for(int n=0;n<11;n++)
                    {
                        sql += QString::number(r_out.accDia_FD.vibMultFrq[n],10,4) + ",";
                    }
                    sql += "'"+ StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    ampvalue = qIsFinite(r_out.spdDia_TD.AmpValue) ? r_out.spdDia_TD.AmpValue : 0.0;
                    rmsvalue = qIsFinite(r_out.spdDia_TD.RMSValue) ? r_out.spdDia_TD.RMSValue : 0.0;
                    pkpkvalue = qIsFinite(r_out.spdDia_TD.PkPkValue) ? r_out.spdDia_TD.PkPkValue : 0.0;
                    waveindex = qIsFinite(r_out.spdDia_TD.waveIndex) ? r_out.spdDia_TD.waveIndex : 0.0;
                    peakindex = qIsFinite(r_out.spdDia_TD.peakIndex) ? r_out.spdDia_TD.peakIndex : 0.0;
                    pulsindex = qIsFinite(r_out.spdDia_TD.pulsIndex) ? r_out.spdDia_TD.pulsIndex : 0.0;
                    kurtindex = qIsFinite(r_out.spdDia_TD.kurtIndex) ? r_out.spdDia_TD.kurtIndex : 0.0;

                    sql = "insert into spdtimefeat_result (amp,rms,pkpk,waveindex,peakindex,pulsindex,kurtindex,rksj,dcode,mcode,pipe,mcid) values (";
                    sql += QString::number(ampvalue,10,4) + "," + QString::number(rmsvalue,10,4) + "," + QString::number(pkpkvalue,10,4) + "," \
                         + QString::number(waveindex,10,4) + "," + QString::number(peakindex,10,4) + "," + QString::number(pulsindex,10,4) + "," + QString::number(kurtindex,10,4) + ",'" \
                            + StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    sql = "insert into spdfreqfeat_result (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                    for(int n=0;n<11;n++)
                    {
                        sql += QString::number(r_out.spdDia_FD.vibMultFrq[n],10,4) + ",";
                    }
                    sql += "'"+ StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    sql = "insert into envfreqfeat_result (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,freq12,freq13,freq14,freq15,freq16,freq17,freq18,freq19,freq20,rksj,dcode,mcode,pipe,mcid) values (";
                    for(int n=0;n<20;n++)
                    {
                        float tmp_f = r_out.envDia_FD.envMultFrq[n];
                        if (qIsNaN(tmp_f) || !qIsFinite(tmp_f))
                        {
                            tmp_f = 0;
                        }
                        sql += QString::number(tmp_f,10,4) + ",";
                    }
                    sql += "'"+ StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    if (sqls.size()>0)
                    {
                        db->execSql(sqls);
                    }
                }
            }

            for(it=keyList.constBegin();it!=keyList.constEnd();it++)
            {
                QVector<SVibDiaFeatData*> inkeyList = studyViblists[*it];
                QVector<SVibDiaFeatData*>::const_iterator sfIt;
                for(sfIt=inkeyList.constBegin();sfIt!=inkeyList.constEnd();sfIt++)
                {
                    SVibDiaFeatData* tmp = *sfIt;
                    delete tmp;
                }
            }

            studyViblists.clear();
            /////////////////////////////////////////////////////////////////////

            QVector<QString> studyelcdatas = db->getElectricStudyData();
            datalen = studyelcdatas.size();

            QMap<QString,QVector<SElcDiaFeatData*>> studyElclists;

            for(int k=0;k<datalen;k++)
            {
                QString item_elc = studyelcdatas.at(k);
                QStringList elc_items = item_elc.split(',');

                if(elc_items.length() == 32)
                {
                    SElcDiaFeatData* sfd_item = new SElcDiaFeatData;

                    QString key = elc_items[30] + "-" + elc_items[28] + "-" + elc_items[29];

                    if(studyElclists.contains(key))
                    {
                        studyElclists[key].push_back(sfd_item);
                    }
                    else
                    {
                        QVector<SElcDiaFeatData*> sdf_list;
                        sdf_list.push_back(sfd_item);
                        studyElclists[key] = sdf_list;
                    }
                    // vol time feat
                    {
                        tmp_s = elc_items[0];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->volDia_TD.AmpValue = tmp_f;

                        tmp_s = elc_items[1];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->volDia_TD.RMSValue = tmp_f;

                        tmp_s = elc_items[2];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->volDia_TD.PkPkValue = tmp_f;
                    }

                    // vol freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = elc_items[3+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->volDia_FD.elcMultFrq[n] = tmp_f;
                    }

                    // cur time feat
                    {
                        tmp_s = elc_items[14];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->curDia_TD.AmpValue = tmp_f;

                        tmp_s = elc_items[15];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->curDia_TD.RMSValue = tmp_f;

                        tmp_s = elc_items[16];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->curDia_TD.PkPkValue = tmp_f;
                    }

                    // cur freq feat
                    for(int n=0;n<11;n++)
                    {
                        tmp_s = elc_items[17+n];
                        tmp_f = tmp_s.toFloat(&tok);
                        sfd_item->curDia_FD.elcMultFrq[n] = tmp_f;
                    }
                }
            }

            keyList = studyElclists.keys();
            for(it=keyList.constBegin();it!=keyList.constEnd();it++)
            {
                QString key = *it;
                QVector<SElcDiaFeatData*> inkeyList = studyElclists[key];
                QVector<SElcDiaFeatData*>::const_iterator sfIt;
                int sflen = inkeyList.size();
                SElcDiaFeatData sfd[sflen];
                int count_i=0;
                for(sfIt=inkeyList.constBegin();sfIt!=inkeyList.constEnd();sfIt++)
                {
                    sfd[count_i] = *(*sfIt);
                    count_i++;
                }

                SElcDiaFeatData r_out;
                CDiagnosis diag(sfd,sflen,&r_out);
                //diag.elcDtaSstudying();

                // write data to database
                QStringList key_items = key.split('-');
                if(key_items.size() == 3)
                {
                    QString dcode = key_items[1];
                    QString mcid = key_items[0];
                    QString channel = key_items[2];

                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                    QQueue<QString> sqls;

                    QString sql = "delete from voltimefeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from volfreqfeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from curtimefeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);
                    sql = "delete from curfreqfeat_result where mcid=" + mcid + " and dcode='" + dcode + "' and pipe=" + channel;
                    sqls.enqueue(sql);

                    db->execSql(sqls);

                    sqls.clear();

                    sql = "insert into voltimefeat_result (amp,rms,pkpk,rksj,dcode,mcode,pipe,mcid) values (";
                    sql += QString::number(r_out.volDia_TD.AmpValue,10,4) + "," + QString::number(r_out.volDia_TD.RMSValue,10,4) + "," + QString::number(r_out.volDia_TD.PkPkValue,10,4) + ",'" \
                            + StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    sql = "insert into volfreqfeat_result (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                    for(int n=0;n<11;n++)
                    {
                        sql += QString::number(r_out.volDia_FD.elcMultFrq[n],10,4) + ",";
                    }
                    sql += "'"+ StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    sql = "insert into curtimefeat_result (amp,rms,pkpk,rksj,dcode,mcode,pipe,mcid) values (";
                    sql += QString::number(r_out.volDia_TD.AmpValue,10,4) + "," + QString::number(r_out.volDia_TD.RMSValue,10,4) + "," + QString::number(r_out.volDia_TD.PkPkValue,10,4) + ",'" \
                            + StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    sql = "insert into curfreqfeat_result (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                    for(int n=0;n<11;n++)
                    {
                        sql += QString::number(r_out.curDia_FD.elcMultFrq[n],10,4) + ",";
                    }
                    sql += "'"+ StrCurrentTime + "','" + dcode + "','0','" + channel + "'," + mcid + ")";
                    sqls.enqueue(sql);

                    if (sqls.size()>0)
                    {
                        db->execSql(sqls);
                    }
                }
            }

            for(it=keyList.constBegin();it!=keyList.constEnd();it++)
            {
                QVector<SElcDiaFeatData*> inkeyList = studyElclists[*it];
                QVector<SElcDiaFeatData*>::const_iterator sfIt;
                for(sfIt=inkeyList.constBegin();sfIt!=inkeyList.constEnd();sfIt++)
                {
                    SElcDiaFeatData* tmp = *sfIt;
                    delete tmp;
                }
            }

            studyElclists.clear();

            GlobalVariable::vibrate_diagnose.clear();
            GlobalVariable::electric_diagnose.clear();
            initingstudydata = false;
        }
    }

    index++;
}

void MainWindow::sDataCheckedSlot(int state)
{
    //qDebug() << "sava data: " << QString::number(state);
    if(0==state)
    {
        GlobalVariable::isSaveData = false;
    }
    else
    {
        GlobalVariable::isSaveData = true;
    }
}

void MainWindow::sOnlineCheckedSlot(int state)
{
    if(0==state)
    {
        GlobalVariable::isOnline = false;
        databaseAct->setEnabled(true);
        openAct->setEnabled(true);
        recordwaveAct->setEnabled(false);

        devicelink->disConnectDevice();
        leftView->switchMode(1);

        DatabaseManager *dbm = new DatabaseManager(db);
        connect(dbm,SIGNAL(enterOfflineWithInterval(QString,QString,QString)),this,SLOT(enterOfflineWithIntervalSlot(QString,QString,QString)));
        dbm->setmode(1);
        dbm->setAttribute(Qt::WA_DeleteOnClose);
        dbm->exec();
    }
    else
    {
        GlobalVariable::isOnline = true;
        databaseAct->setEnabled(false);
        openAct->setEnabled(false);
        recordwaveAct->setEnabled(true);
        leftView->switchMode(0);
    }
}

void MainWindow::enterOfflineWithIntervalSlot(QString start_e,QString end_e,QString title)
{
    QDateTime dt_s = QDateTime::fromString(start_e,GlobalVariable::dtFormat);
    QDateTime dt_e = QDateTime::fromString(end_e,GlobalVariable::dtFormat);

    leftView->setOffDatetime(dt_s,dt_e,title);
}

void MainWindow::databaseSlot()
{
    DatabaseManager *dbm = new DatabaseManager(db);
    connect(dbm,SIGNAL(enterOfflineWithInterval(QString,QString,QString)),this,SLOT(enterOfflineWithIntervalSlot(QString,QString,QString)));
    dbm->setAttribute(Qt::WA_DeleteOnClose);
    dbm->exec();
}

void MainWindow::recoreWaveSlot()
{
    RecordWaveWidget *recordWave = new RecordWaveWidget;
    connect(recordWave,SIGNAL(recorwWaveWithInfo(int,QString)),this,SLOT(recorwWaveWithInfoSlot(int,QString)));
    recordWave->setAttribute(Qt::WA_DeleteOnClose);
    recordWave->show();
}

void MainWindow::recorwWaveWithInfoSlot(int interval,QString name)
{
    //GlobalVariable::isSaveData = true;

    QDateTime dt = QDateTime::currentDateTime();
    QString dt_str = dt.toString(GlobalVariable::dtFormat);
    GlobalVariable::rwavetime = dt_str;
    GlobalVariable::rwaveinterval = interval;
    QString sql = "insert into waverecord(name,kssj,interval,rksj) values('" + name + "','" + dt_str + "'," + QString::number(interval) + ",'" + dt_str + "')";
    db->updatasql(sql);
    //QTimer::singleShot(interval*1000, this, SLOT(recoreWaveTimeoutDevice()));
}

void MainWindow::recoreWaveTimeoutDevice()
{
    //GlobalVariable::isSaveData = false;
}

void MainWindow::diagnoseSettingSlot()
{
    DiagnoseSetting* ds = new DiagnoseSetting();
    ds->setAttribute(Qt::WA_DeleteOnClose);
    ds->exec();
}

void MainWindow::chartnodeSettingSlot()
{
    ParamSettingDialog* psd = new ParamSettingDialog();
    psd->setAttribute(Qt::WA_DeleteOnClose);
    psd->exec();
}

void MainWindow::serverStateChangerSlot(bool state)
{
    int param = 0x8000;
    if(!state)
    {
        param = 0;
        if(tcpSocket != nullptr)
        {
            tcpSocket->disconnectFromHost();
            tcpSocket->close();
            delete tcpSocket;
            tcpSocket = nullptr;
        }
    }
    leftView->updateActionState(param);
}

void MainWindow::serverparameterSettingSlot()
{
    ServerParameterSetting* sps = new ServerParameterSetting(db);
    connect(sps,SIGNAL(serverStateChange(bool)),this,SLOT(serverStateChangerSlot(bool)));
    sps->setAttribute(Qt::WA_DeleteOnClose);
    sps->exec();
}

void MainWindow::fftparamSettingSlot()
{
    FFTParamSettingDialog* fft = new FFTParamSettingDialog();
    fft->setAttribute(Qt::WA_DeleteOnClose);
    fft->exec();
}

float MainWindow::getOfflineMotorRotate(QString code,QString rksj)
{
    QVector<QString>::ConstIterator cit;
    QString sql = "select * from motorcondition where mcode='" + code + "' and rksj<='" + rksj + "' order by rksj desc limit 1";
    QVector<QString> m_conditions = db->execSql(sql);

    float rotate = 0.0;
    bool tok;
    for(cit=m_conditions.constBegin();cit!=m_conditions.constEnd();cit++)
    {
        QString mc = *cit;
        QStringList mc_items = mc.split(',');
        if(mc_items.size() >= 7)
        {
            QString tmp = mc_items[4];
            int isManual = tmp.toInt(&tok);
            if(isManual == 1)
            {
                tmp = mc_items[3];
                rotate = tmp.toFloat(&tok);
                break;
            }
            else
            {
                QString ddcode = mc_items[1];
                ddcode = ddcode.trimmed();
                if(ddcode.length()>0)
                {
                    QString condi = db->getConditionByCodeTime(ddcode,rksj);
                    QStringList con_items = condi.split(',');
                    if(con_items.size() >= 3)
                    {
                        tmp = con_items[1];
                        rotate = tmp.toFloat(&tok);
                        break;
                    }
                }
                ddcode = mc_items[2];
                ddcode = ddcode.trimmed();
                if(ddcode.length()>0)
                {
                    QString condi = db->getConditionByCodeTime(ddcode,rksj);
                    QStringList con_items = condi.split(',');
                    if(con_items.size() >= 3)
                    {
                        tmp = con_items[1];
                        rotate = tmp.toFloat(&tok);
                        break;
                    }
                }
            }
        }
    }

    if(qAbs(rotate) < 0.0001)
    {
        rotate = GlobalVariable::motorCondition[code].rotate;
    }

    return rotate;
}

MainWindow::~MainWindow()
{
    saveThread->setState(true);
    //saveAnalyseThread->setState(true);
    int count = 2000;
    while(count>0)
    {
        count--;
    }

    db->close();
    delete db;

    saveThread->terminate();
    saveThread->wait();

    delete serverState;

    if(nullptr != tcpSocket)
    {
        if(tcpSocket->isOpen())
        {
            tcpSocket->disconnectFromHost();
            tcpSocket->close();
        }

        delete tcpSocket;
        tcpSocket = nullptr;
    }

    //saveAnalyseThread->terminate();
    //saveAnalyseThread->wait();

    //sendThread->terminate();
    //sendThread->wait();

    /*
    int len = deviceStates.count();
    for(int i=0;i<len;i++)
    {
        QLabel *tmp = deviceStates.at(i);
        delete tmp;
    }
    deviceStates.clear();

    delete serverState;

    if(nullptr != tcpSocket)
    {
        tcpSocket->disconnectFromHost();
        tcpSocket->close();
        delete tcpSocket;
        tcpSocket = nullptr;
    }
    */
}
