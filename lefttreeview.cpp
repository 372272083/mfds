#include "lefttreeview.h"

#include "sqlitedb.h"
#include <QTreeWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QMenu>
#include <QtGui/QGuiApplication>
#include <QQueue>
#include <QMap>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
//#include <QDateTime>

#include "deviceeditdialog.h"
#include "globalvariable.h"
#include "deviceinfo.h"
#include "motorinfo.h"
#include "channelinfo.h"
#include "treenodeinfo.h"

#include "motoredit.h"
#include "devicepipeedit.h"

#include "cmieedeviceinfo.h"
#include "cmievdeviceinfo.h"
#include "cmietdeviceinfo.h"
#include "tw888deviceinfo.h"
#include "freqxselectdialog.h"

#include "motorconditiontable.h"
#include "pathtrackingchannelselect.h"
#include "syncdatainfo.h"

#include "constants.h"

LeftTreeView::LeftTreeView(SqliteDB *db,QWidget *parent) : QWidget(parent),m_db(db),run_mode(0)
{

    //this->setStyleSheet(QString::fromUtf8("border:1px solid red"));

    QSplitter* vbox = new QSplitter(this);

    vbox->setOrientation(Qt::Vertical);
    vbox->setContentsMargins(1,2,2,1);

    motorTree = new QTreeWidget();
    motorTree->setColumnCount(1);
    motorTree->setHeaderHidden(true);

    connect(motorTree, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(itemMotorPressedSlot(QTreeWidgetItem*,int)));

    connect(motorTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showSelectedChart(QTreeWidgetItem*,int)));

    //motorRoot = new QTreeWidgetItem(motorTree,QStringList(QString(tr("Sample Data"))),0);

    //motorRoot->setExpanded(true);

    deviceTree = new QTreeWidget();
    deviceTree->setColumnCount(1);
    deviceTree->setHeaderHidden(true);

    connect(deviceTree, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(itemDevicePressedSlot(QTreeWidgetItem*,int)));

    connect(deviceTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showDeviceSelectedChart(QTreeWidgetItem*,int)));

    deviceRoot = nullptr;
    motorRoot = nullptr;
    //deviceRoot = new QTreeWidgetItem(deviceTree,QStringList(QString(tr("Devices"))),0);
    //deviceRoot->setIcon(0,QIcon(":/images/devices.png"));

    QWidget* motorWidget = new QWidget(this);
    QVBoxLayout* motorWidgetLayout = new QVBoxLayout();
    motorWidgetLayout->setContentsMargins(0,0,0,0);
    motorWidget->setLayout(motorWidgetLayout);

    m_pSearchLineEdit = new QLineEdit();
    QPushButton *pSearchButton = new QPushButton(this);

    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setFixedSize(22, 22);
    pSearchButton->setToolTip(QStringLiteral("搜索"));
    pSearchButton->setStyleSheet("QPushButton{border-image:url(:/images/icon_search_normal); background:transparent;} \
                                         QPushButton:hover{border-image:url(:/images/icon_search_hover)} \
                                         QPushButton:pressed{border-image:url(:/images/icon_search_press)}");

    //防止文本框输入内容位于按钮之下
    QMargins margins = m_pSearchLineEdit->textMargins();
    m_pSearchLineEdit->setTextMargins(margins.left(), margins.top(), pSearchButton->width(), margins.bottom());
    m_pSearchLineEdit->setPlaceholderText(QStringLiteral("请输入搜索内容"));

    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(pSearchButton);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setContentsMargins(0, 0, 0, 0);
    m_pSearchLineEdit->setLayout(pSearchLayout);

    connect(pSearchButton, SIGNAL(clicked(bool)), this, SLOT(search()));

    motorWidgetLayout->addWidget(m_pSearchLineEdit);
    motorWidgetLayout->addWidget(motorTree);
    motorWidget->setLayout(motorWidgetLayout);

    vbox->addWidget(motorWidget);

    QWidget* deviceWidget = new QWidget(this);
    QVBoxLayout* deviceWidgetLayout = new QVBoxLayout();
    deviceWidgetLayout->setContentsMargins(0,0,0,0);
    deviceWidget->setLayout(deviceWidgetLayout);
    deviceTitle = new QLabel(this);
    deviceTitle->setText(tr("Device List"));
    deviceWidgetLayout->addWidget(deviceTitle);
    deviceWidgetLayout->addWidget(deviceTree);

    vbox->addWidget(deviceWidget);

    vbox->setStretchFactor(0,3);
    vbox->setStretchFactor(1,1);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(25,5,1,2);
    vlayout->addWidget(vbox);
    this->setLayout(vlayout);

    //copyActoin = new QAction(QString::fromLocal8Bit("&Copy"),this);
    //pasteActoin = new QAction(QString::fromLocal8Bit("&Paste"),this);

    newMotor = new QAction((tr("&New Monitor Device")),this);
    connect(newMotor, &QAction::triggered, this, &LeftTreeView::newMotorSlot);

    newDevice = new QAction((tr("&New Device")),this);
    connect(newDevice, &QAction::triggered, this, &LeftTreeView::newDeviceSlot);

    editActoin = new QAction((tr("&Edit")),this);
    connect(editActoin, &QAction::triggered, this, &LeftTreeView::editSlot);
    deleteActoin = new QAction((tr("&Delete")),this);
    connect(deleteActoin, &QAction::triggered, this, &LeftTreeView::deleteSlot);

    newPhyisicChannel = new QAction((tr("&Physical Channel")),this);
    connect(newPhyisicChannel, &QAction::triggered, this, &LeftTreeView::newPhysicChannelSlot);
    newComposeChannel = new QAction((tr("&Compose Channel")),this);
    newCompareChannel = new QAction((tr("&Compare Channel")),this);

    newFeatureChart = new QAction((tr("&Accelerate RMS Trend")),this);
    connect(newFeatureChart, &QAction::triggered, this, &LeftTreeView::newFeatureSlot);
    newSpeedFeatureChart = new QAction((tr("&Speed RMS Trend")),this);
    connect(newSpeedFeatureChart, &QAction::triggered, this, &LeftTreeView::newSpeedFeatureSlot);
    newFrequencyChart = new QAction((tr("&Accelerate Frequency")),this);
    connect(newFrequencyChart, &QAction::triggered, this, &LeftTreeView::newFrequencySlot);
    newWaveChart = new QAction((tr("&Accelerate Wave")),this);
    connect(newWaveChart, &QAction::triggered, this, &LeftTreeView::newWaveSlot);

    newSpeedChart = new QAction((tr("&Speed Wave")),this);
    connect(newSpeedChart, &QAction::triggered, this, &LeftTreeView::newSpdWaveSlot);
    newSpeedFreqChart = new QAction((tr("&Speed Frequency")),this);
    connect(newSpeedFreqChart, &QAction::triggered, this, &LeftTreeView::newSpdFrequencySlot);
    newEnFreqChart = new QAction((tr("&Accelerate Envelope")),this);
    connect(newEnFreqChart, &QAction::triggered, this, &LeftTreeView::newEnvFrequencySlot);

    newAccPowerFreqChart = new QAction((tr("&Accelerate Power Spectrum")),this);
    connect(newAccPowerFreqChart, &QAction::triggered, this, &LeftTreeView::newAccPowerSpectrumSlot);
    newSpdPowerFreqChart = new QAction((tr("&Speed Power Spectrum")),this);
    connect(newSpdPowerFreqChart, &QAction::triggered, this, &LeftTreeView::newSpdPowerSpectrumSlot);
    newAccCepstrumChart = new QAction((tr("&Accelerate Cepstrum")),this);
    connect(newAccCepstrumChart, &QAction::triggered, this, &LeftTreeView::newAccCepstrumSlot);
    newSpdCepstrumChart = new QAction((tr("&Speed Cepstrum")),this);
    connect(newSpdCepstrumChart, &QAction::triggered, this, &LeftTreeView::newSpdCepstrumSlot);

    newPosFeatureChart =  new QAction((tr("&Position RMS Trend")),this);
    connect(newPosFeatureChart, &QAction::triggered, this, &LeftTreeView::newPosTrendSlot);
    newDisChart =  new QAction((tr("&Position Wave")),this);
    connect(newDisChart, &QAction::triggered, this, &LeftTreeView::newPosWaveSlot);

    newUFeatureChart = new QAction((tr("&U RMS Trend")),this);
    connect(newUFeatureChart, &QAction::triggered, this, &LeftTreeView::newUFeatureSlot);
    newIFeatureChart = new QAction((tr("&I RMS Trend")),this);
    connect(newIFeatureChart, &QAction::triggered, this, &LeftTreeView::newIFeatureSlot);
    newUWaveChart = new QAction((tr("&U Wave")),this);
    connect(newUWaveChart, &QAction::triggered, this, &LeftTreeView::newUWaveSlot);
    newIWaveChart = new QAction((tr("&I Wave")),this);
    connect(newIWaveChart, &QAction::triggered, this, &LeftTreeView::newIWaveSlot);
    newUFreqencyChart = new QAction((tr("&U Freqency")),this);
    connect(newUFreqencyChart, &QAction::triggered, this, &LeftTreeView::newUFreqencySlot);
    newIFreqencyChart = new QAction((tr("&I Freqency")),this);
    connect(newIFreqencyChart, &QAction::triggered, this, &LeftTreeView::newIFreqencySlot);

    newactPowerChart = new QAction((tr("Act Power")),this);
    connect(newactPowerChart, &QAction::triggered, this, &LeftTreeView::newactPowerSlot);
    newreactPowerChart = new QAction((tr("React Power")),this);
    connect(newreactPowerChart, &QAction::triggered, this, &LeftTreeView::newreactPowerSlot);
    newappPowerChart = new QAction((tr("App Power")),this);
    connect(newappPowerChart, &QAction::triggered, this, &LeftTreeView::newappPowerSlot);
    newcosfChart = new QAction((tr("CosF")),this);
    connect(newcosfChart, &QAction::triggered, this, &LeftTreeView::newcosfSlot);

    newUPSequenceChart = new QAction((tr("Voltage Positive Power")),this);
    connect(newUPSequenceChart, &QAction::triggered, this, &LeftTreeView::newupsequenceSlot);
    newUNSequenceChart = new QAction((tr("Voltage Negative Power")),this);
    connect(newUNSequenceChart, &QAction::triggered, this, &LeftTreeView::newunsequenceSlot);
    newUZSequenceChart = new QAction((tr("Voltage Zero Power")),this);
    connect(newUZSequenceChart, &QAction::triggered, this, &LeftTreeView::newuzsequenceSlot);

    newIPSequenceChart = new QAction((tr("Current Positive Power")),this);
    connect(newIPSequenceChart, &QAction::triggered, this, &LeftTreeView::newipsequenceSlot);
    newINSequenceChart = new QAction((tr("Current Negative Power")),this);
    connect(newINSequenceChart, &QAction::triggered, this, &LeftTreeView::newinsequenceSlot);
    newIZSequenceChart = new QAction((tr("Current Zero Power")),this);
    connect(newIZSequenceChart, &QAction::triggered, this, &LeftTreeView::newizsequenceSlot);

    newFreqXChart = new QAction((tr("&X Freqency Trend")),this);
    connect(newFreqXChart, &QAction::triggered, this, &LeftTreeView::newXFreqSlot);

    ddeleteActoin = new QAction((tr("Delete")),this);
    connect(ddeleteActoin, &QAction::triggered, this, &LeftTreeView::ddeleteSlot);
    dexportActoin = new QAction((tr("Export Data")),this);
    connect(dexportActoin, &QAction::triggered, this, &LeftTreeView::dexportSlot);

    waterfallaccAction = new QAction((tr("Acc Water Fall Plot")),this);
    connect(waterfallaccAction, &QAction::triggered, this, &LeftTreeView::waterfallaccSlot);

    waterfallspdAction = new QAction((tr("Speed Water Fall Plot")),this);
    connect(waterfallspdAction, &QAction::triggered, this, &LeftTreeView::waterfallspdSlot);

    waterfallvolAction = new QAction((tr("Voltage Water Fall Plot")),this);
    connect(waterfallvolAction, &QAction::triggered, this, &LeftTreeView::waterfallvolSlot);

    waterfallcurAction = new QAction((tr("Current Water Fall Plot")),this);
    connect(waterfallcurAction, &QAction::triggered, this, &LeftTreeView::waterfallcurSlot);

    newPathAction = new QAction((tr("Path tracking")),this);
    connect(newPathAction, &QAction::triggered, this, &LeftTreeView::pathtrackingSlot);
    newReportAction = new QAction((tr("Diagnose Result")),this);
    connect(newReportAction, &QAction::triggered, this, &LeftTreeView::diagnosereportSlot);
    newParkAction = new QAction((tr("Park Transformation")),this);
    connect(newParkAction, &QAction::triggered, this, &LeftTreeView::parktransformationSlot);

    newTemperatureChart = new QAction((tr("Measure")),this);
    connect(newTemperatureChart, &QAction::triggered, this, &LeftTreeView::tempMeasureSlot);

    syncMotorData = new QAction((tr("Sync Motor Data")),this);
    connect(syncMotorData, &QAction::triggered, this, &LeftTreeView::syncMotorDataSlot);
}

void LeftTreeView::setOffDatetime(QDateTime start,QDateTime end,QString title)
{
    off_dt = start;
    off_e_dt = end;

    offline_title = title;

    QString dshow = tr("Data List") + QObject::tr("<font color = red>[%1]</font>").arg(title);
    deviceTitle->setText(dshow);

    switch (currentSelectType) {
    case MOTORROOT:
        break;
    case MOTOR:
        break;
    case CHANNEL_PHYISIC:
    case CHANNEL_COMPARE:
    case CHANNEL_COMPOSE:
        break;
    case MEASURE:
    case MEASURE_SPEED:
    case MEASURE_POS:
    case WAVE:
    case FREQENCY:
    case WAVE_A:
    case SPEED_WAVE_A:
    case WAVE_FREQ_A:
    case SPEED_FREQ_A:
    case EN_FREQ_A:
    case UMEASURE:
    case IMEASURE:
    case UWAVE:
    case IWAVE:
    case UFREQENCY:
    case IFREQENCY:
        if(1 == run_mode)
        {
            QTreeWidgetItem* tItem = motorTree->currentItem();
            if(tItem != nullptr)
                setupDatalist(tItem->parent()->type());
        }
        break;
    default:
        break;
    }
}

void LeftTreeView::switchMode(int mode)
{
    this->run_mode = mode;
    offline_title = "";
    deviceTitle->setText(tr("Data List"));
    if(0 == run_mode) // online mode
    {
        deviceTitle->setText(tr("Device List"));
    }
    else // offline mode
    {
        deviceTitle->setText(tr("Data List"));
    }

    this->setupDeviceTree();
}

void LeftTreeView::search()
{
    QString strText = m_pSearchLineEdit->text();
    if (!strText.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("搜索"), QStringLiteral("搜索内容为%1").arg(strText));
    }
}

void LeftTreeView::itemMotorPressedSlot(QTreeWidgetItem *pressedItem, int column)
{
    if (!(QGuiApplication::mouseButtons() & Qt::RightButton))
    {
        //QTreeWidgetItem* curItem=motorTree->currentItem();  //获取当前被点击的节点
        if(pressedItem==NULL)
            return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

        int id = pressedItem->type();

        if(0 == id)
        {
            return;
        }

        currentSelectId = 0;

        QVector<TreeNodeInfo*>::ConstIterator cIt;
        for(cIt=GlobalVariable::motorTreeInfos.constBegin();cIt!=GlobalVariable::motorTreeInfos.constEnd();cIt++)
        {
            TreeNodeInfo* item = *cIt;
            if(item->id == id)
            {
                TREENODETYPE itemType = item->type;
                currentSelectType = itemType;
                currentSelectId = id;

                switch (itemType) {
                case MOTORROOT:
                    if(1 == run_mode)
                    {
                        setupDeviceTree();
                    }
                    break;
                case MOTOR:
                    if(1 == run_mode)
                    {
                        setupDeviceTree();
                    }
                    break;
                case CHANNEL_PHYISIC:
                case CHANNEL_COMPARE:
                case CHANNEL_COMPOSE:
                    if(1 == run_mode)
                    {
                        setupDeviceTree();
                    }
                    break;
                case MEASURE:
                case MEASURE_SPEED:
                case MEASURE_POS:
                case WAVE:
                case FREQENCY:
                case WAVE_A:
                case SPEED_WAVE_A:
                case WAVE_FREQ_A:
                case SPEED_FREQ_A:
                case EN_FREQ_A:
                case DIS_WAVE_A:
                case UMEASURE:
                case IMEASURE:
                case UWAVE:
                case IWAVE:
                case UFREQENCY:
                case IFREQENCY:
                case WATERFALL_ACC:
                case WATERFALL_SPD:
                case WATERFALL_VOL:
                case WATERFALL_CUR:
                case ACC_POWER:
                case SPD_POWER:
                case ACC_Cepstrum:
                case SPD_Cepstrum:
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
                case TEMP_MEASURE:
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
                case PARK_TRANSFORMATION:
                    if(1 == run_mode)
                    {
                        setupDatalist(pressedItem->parent()->type());
                    }
                    break;
                case DIAGNOSE_REPORT:
                case PATH_TRACKING:
                    if(1 == run_mode)
                    {
                        setupDeviceTree();
                    }
                    break;
                default:
                    break;
                }
                break;

                emit treeSelectedChange(itemType);
            }
        }
        return;
    }

    //QTreeWidgetItem* curItem=motorTree->currentItem();  //获取当前被点击的节点
    if(pressedItem==NULL)
        return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

    int id = pressedItem->type();

    if(0 == id)
    {
        QMenu menu(motorTree);
        menu.addAction(newMotor);
        menu.exec(QCursor::pos());  //在当前鼠标位置显示
        return;
    }

    currentSelectId = 0;

    QVector<TreeNodeInfo*>::ConstIterator cIt;
    for(cIt=GlobalVariable::motorTreeInfos.constBegin();cIt!=GlobalVariable::motorTreeInfos.constEnd();cIt++)
    {
        TreeNodeInfo* item = *cIt;
        if(item->id == id)
        {
            TREENODETYPE itemType = item->type;
            emit treeSelectedChange(itemType);
            currentSelectType = itemType;
            currentSelectId = id;

            QString deviceCode = GlobalVariable::findDeviceCodeByChannelTreeId(id);
            QString deviceType = GlobalVariable::findDeviceTypeByCode(deviceCode);

            QPoint pos;
            QMenu menu(motorTree);

            QMenu* submenu;

            switch (itemType) {
            case MOTORROOT:
                menu.addAction(newMotor);
                menu.exec(QCursor::pos());  //在当前鼠标位置显示
                if(1==run_mode)
                {
                    setupDeviceTree();
                }
                break;
            case MOTOR:
                //menu.addAction(copyActoin);
                //menu.addAction(pasteActoin);
                //menu.addSeparator();
                menu.addAction(editActoin);
                menu.addAction(deleteActoin);
                menu.addSeparator();
                if(GlobalVariable::is_sync_done)
                {
                    syncMotorData->setEnabled(false);
                }
                else
                {
                    syncMotorData->setEnabled(true);
                }
                menu.addAction(syncMotorData);
                menu.addSeparator();
                submenu = menu.addMenu(tr("New Channel"));
                //menu.addAction(newChannel);
                submenu->addAction(newPhyisicChannel);
                //submenu->addAction(newComposeChannel);
                //submenu->addAction(newCompareChannel);

                menu.addSeparator();
                /*
                if(GlobalVariable::s_t == 1 || GlobalVariable::s_t == 0)
                {
                    menu.addAction(newPathAction);
                }
                */
                if(GlobalVariable::version >= 2)
                {
                    menu.addAction(newReportAction);
                }

                menu.exec(QCursor::pos());  //在当前鼠标位置显示
                if(1==run_mode)
                {
                    setupDeviceTree();
                }
                break;
            case CHANNEL_PHYISIC:
            case CHANNEL_COMPARE:
            case CHANNEL_COMPOSE:
                if(deviceType.compare("V",Qt::CaseInsensitive) == 0)
                {
                    //menu.addAction(copyActoin);
                    //menu.addAction(pasteActoin);
                    //menu.addSeparator();
                    menu.addAction(editActoin);
                    menu.addAction(deleteActoin);
                    menu.addSeparator();
                    submenu = menu.addMenu(tr("New Chart"));
                    //menu.addAction(newChannel);
                    submenu->addAction(newFeatureChart);
                    submenu->addAction(newFrequencyChart);
                    submenu->addAction(newWaveChart);
                    submenu->addAction(newEnFreqChart);
                    submenu->addAction(newAccPowerFreqChart);
                    submenu->addAction(newAccCepstrumChart);
                    submenu->addSeparator();
                    submenu->addAction(newSpeedFeatureChart);
                    submenu->addAction(newSpeedChart);
                    submenu->addAction(newSpeedFreqChart);
                    submenu->addAction(newSpdPowerFreqChart);
                    submenu->addAction(newSpdCepstrumChart);

                    submenu->addSeparator();
                    submenu->addAction(newPosFeatureChart);
                    submenu->addAction(newDisChart);

                    submenu->addSeparator();
                    submenu->addAction(newFreqXChart);

                    submenu->addSeparator();
                    submenu->addAction(waterfallaccAction);
                    submenu->addAction(waterfallspdAction);

                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                }
                else if(deviceType.compare("E",Qt::CaseInsensitive) == 0)
                {
                    //menu.addAction(copyActoin);
                    //menu.addAction(pasteActoin);
                    //menu.addSeparator();
                    menu.addAction(editActoin);
                    menu.addAction(deleteActoin);
                    menu.addSeparator();
                    submenu = menu.addMenu(tr("New Chart"));
                    //menu.addAction(newChannel);
                    submenu->addAction(newactPowerChart);
                    submenu->addAction(newreactPowerChart);
                    submenu->addAction(newappPowerChart);
                    submenu->addAction(newcosfChart);
                    submenu->addSeparator();
                    submenu->addAction(newUPSequenceChart);
                    submenu->addAction(newUNSequenceChart);
                    submenu->addAction(newUZSequenceChart);
                    submenu->addAction(newUFeatureChart);
                    submenu->addAction(newUWaveChart);
                    submenu->addAction(newUFreqencyChart);
                    submenu->addSeparator();
                    submenu->addAction(newIPSequenceChart);
                    submenu->addAction(newINSequenceChart);
                    submenu->addAction(newIZSequenceChart);
                    submenu->addAction(newIFeatureChart);
                    submenu->addAction(newIWaveChart);
                    submenu->addAction(newIFreqencyChart);
                    submenu->addSeparator();
                    submenu->addAction(newFreqXChart);
                    submenu->addSeparator();
                    if(GlobalVariable::s_t == 2 || GlobalVariable::s_t == 0)
                    {
                        submenu->addAction(newParkAction);
                    }
                    submenu->addSeparator();
                    submenu->addAction(waterfallvolAction);
                    submenu->addAction(waterfallcurAction);

                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                }
                else if(deviceType.compare("T",Qt::CaseInsensitive) == 0)
                {
                    //menu.addAction(copyActoin);
                    //menu.addAction(pasteActoin);
                    //menu.addSeparator();
                    menu.addAction(editActoin);
                    menu.addAction(deleteActoin);
                    menu.addSeparator();
                    submenu = menu.addMenu(tr("New Chart"));
                    //menu.addAction(newChannel);
                    submenu->addAction(newTemperatureChart);

                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                }
                else
                {
                    menu.addAction(editActoin);
                    menu.addAction(deleteActoin);
                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                }
                //if(1 == run_mode)
                //{
                //    setupDatalist(currentSelectId);
                //}
                if(1==run_mode)
                {
                    setupDeviceTree();
                }
                break;
            case MEASURE:
            case MEASURE_SPEED:
            case MEASURE_POS:
            case WAVE:
            case FREQENCY:
            case WAVE_A:
            case SPEED_WAVE_A:
            case WAVE_FREQ_A:
            case SPEED_FREQ_A:
            case EN_FREQ_A:
            case DIS_WAVE_A:
            case UMEASURE:
            case IMEASURE:
            case UWAVE:
            case IWAVE:
            case UFREQENCY:
            case IFREQENCY:
            case WATERFALL_ACC:
            case WATERFALL_SPD:
            case WATERFALL_VOL:
            case WATERFALL_CUR:
            case ACC_POWER:
            case SPD_POWER:
            case ACC_Cepstrum:
            case SPD_Cepstrum:
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
            case TEMP_MEASURE:
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
            case PARK_TRANSFORMATION:
                if(1 == run_mode)
                {
                    setupDatalist(pressedItem->parent()->type());
                }
                else
                {
                    //menu.addAction(copyActoin);
                    //menu.addSeparator();
                    //menu.addAction(editActoin);
                    menu.addAction(deleteActoin);
                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                }
                break;
            case PATH_TRACKING:
            case DIAGNOSE_REPORT:
                if(1 != run_mode)
                {
                    menu.addAction(deleteActoin);
                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                }
                else
                {
                    setupDeviceTree();
                }
                break;
            default:
                break;
            }
            break;
        }
    }
}

void LeftTreeView::updateActionState(int state)
{
    int state_s = state & 0xFFFF;
    int server_state = state_s & 0x8000;
    if(server_state > 0)
    {
        syncMotorData->setEnabled(true);
    }
    else
    {
        syncMotorData->setEnabled(false);
    }
}

void LeftTreeView::setupDatalist(int channel_id)
{
    setupDeviceTree();

    if(offline_title.length() == 0)
    {
        QMessageBox::information(this, QStringLiteral("Information  "), tr("Must select a data segment to show!"));
        return;
    }

    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt = GlobalVariable::channelInfos.constBegin();cIt!=GlobalVariable::channelInfos.constEnd();cIt++)
    {
        ChannelInfo* ci = *cIt;
        if(ci->tree_id == channel_id)
        {
            QString dcode = ci->device_code;
            QString channel = ci->channel_code;

            offline_cur_table = 0;

            QString dtype_in = GlobalVariable::findDeviceTypeByCode(dcode);
            if(dtype_in.compare("E",Qt::CaseInsensitive) == 0)
            {
                offline_cur_table = 0;
            }
            else if(dtype_in.compare("V",Qt::CaseInsensitive) == 0)
            {
                offline_cur_table = 1;
            }

            QString start_e = off_dt.toString(GlobalVariable::dtFormat);
            QString end_e = off_e_dt.toString(GlobalVariable::dtFormat);

            QVector<QString> timelist;

            if(0 == offline_cur_table)
            {
                switch(currentSelectType)
                {
                case UMEASURE:
                case UFREQENCY:
                case UWAVE:
                case WATERFALL_VOL:
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
                case U_P_SEQUENCE:
                case U_N_SEQUENCE:
                case U_Z_SEQUENCE:
                    timelist = m_db->getWaveTimeList(CHARGEWAVEINFO,dcode,channel,start_e,end_e);
                    break;
                case IWAVE:
                case IFREQENCY:
                case IMEASURE:
                case WATERFALL_CUR:
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
                case I_P_SEQUENCE:
                case I_N_SEQUENCE:
                case I_Z_SEQUENCE:
                    timelist = m_db->getWaveTimeList(CHARGEWAVEINFO_I,dcode,channel,start_e,end_e);
                    break;
                }
            }
            else
            {
                timelist = m_db->getWaveTimeList(VIBRATEWAVEINFO,dcode,channel,start_e,end_e);
            }

            bool tok;
            QVector<QString>::ConstIterator icIt;
            icIt = timelist.constBegin();
            for(;icIt != timelist.constEnd();icIt++)
            {
                QString item = *icIt;
                QStringList item_ss = item.split(',');
                if(item_ss.size() == 2)
                {
                    int item_id = item_ss[0].toInt(&tok);
                    QTreeWidgetItem* node = new QTreeWidgetItem(deviceTree,QStringList(item_ss[1]),item_id);
                    node->setIcon(0,QIcon(":/images/wave_i.png"));
                }
            }
            break;
        }
    }
}

void LeftTreeView::itemDevicePressedSlot(QTreeWidgetItem *pressedItem, int column)
{
    //QMessageBox::information(this, QStringLiteral("搜索"), tr("2"));
    if (!(QGuiApplication::mouseButtons() & Qt::RightButton))
    {
        if(pressedItem==NULL)
            return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

        int id = pressedItem->type();

        if(0 == id)
        {
            return;
        }

        currentSelectId = 0;

        QVector<TreeNodeInfo*>::ConstIterator cIt;
        for(cIt=GlobalVariable::deviceTreeInfos.constBegin();cIt!=GlobalVariable::deviceTreeInfos.constEnd();cIt++)
        {
            TreeNodeInfo* item = *cIt;
            if(item->id == id)
            {
                TREENODETYPE itemType = item->type;
                emit treeSelectedChange(itemType);
            }
        }
        return;
    }

    //QTreeWidgetItem* curItem=deviceTree->currentItem();  //获取当前被点击的节点
    if(pressedItem==NULL)
        return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

    int id = pressedItem->type();

    if(0 == id)
    {
        QMenu menu(deviceTree);
        menu.addAction(newDevice);
        menu.exec(QCursor::pos());  //在当前鼠标位置显示
        return;
    }

    if(run_mode == 0)
    {
        currentSelectId = 0;

        QVector<TreeNodeInfo*>::ConstIterator cIt;
        for(cIt=GlobalVariable::deviceTreeInfos.constBegin();cIt!=GlobalVariable::deviceTreeInfos.constEnd();cIt++)
        {
            TreeNodeInfo* item = *cIt;
            if(item->id == id)
            {
                TREENODETYPE itemType = item->type;
                emit treeSelectedChange(itemType);
                currentSelectType = itemType;
                currentSelectId = id;

                QPoint pos;
                QMenu menu(deviceTree);

                QMenu* submenu;

                switch (itemType) {
                case DEVICEROOT:
                    menu.addAction(newDevice);
                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                    break;
                case DEVICE:
                    //menu.addAction(copyActoin);
                    //menu.addAction(pasteActoin);
                    //menu.addSeparator();
                    menu.addAction(editActoin);
                    menu.addAction(deleteActoin);
                    menu.exec(QCursor::pos());  //在当前鼠标位置显示
                    break;
                default:
                    break;
                }
                break;
            }
        }
    }
    else
    {
        currentSelectId = pressedItem->type();
        QMenu menu(deviceTree);
        menu.addAction(ddeleteActoin);
        menu.addAction(dexportActoin);
        menu.exec(QCursor::pos());  //在当前鼠标位置显示
    }
}

void LeftTreeView::newDeviceSlot()
{
    DeviceEditDialog *devieDialog = new DeviceEditDialog(m_db);
    devieDialog->setmode(-1);
    devieDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(devieDialog,SIGNAL(deivceEditStateChange(int,QString)),this,SLOT(deivceEditStateChangeSlot(int,QString)));
    devieDialog->exec();
}

void LeftTreeView::newMotorSlot()
{
    MotorEdit *motoredit = new MotorEdit(m_db);
    motoredit->setmode(-1);
    motoredit->setAttribute(Qt::WA_DeleteOnClose);
    connect(motoredit,SIGNAL(motorEditStateChange(int,QString)),this,SLOT(motorEditStateChangeSlot(int,QString)));
    motoredit->exec();
}

void LeftTreeView::setupDeviceTree()
{
    // device tree construct
    if(deviceRoot != nullptr)
    {
        int clen = deviceRoot->childCount();

        for(int i=clen-1;i>=0;i--)
        {
            QTreeWidgetItem *sitem = deviceRoot->child(i);
            deviceRoot->removeChild(sitem);
            delete sitem;
        }

        delete deviceRoot;
        deviceRoot = nullptr;
    }
    deviceTree->clear();
    if(GlobalVariable::isOnline)
    {
        bool tok;
        QVector<QString>::ConstIterator cIt;
        QVector<TreeNodeInfo*>::ConstIterator mtiIt;

        deviceRoot = new QTreeWidgetItem(deviceTree,QStringList(QString(tr("Devices"))),0);
        deviceRoot->setIcon(0,QIcon(":/images/devices.png"));

        int tmpLen = GlobalVariable::deviceTreeInfos.size();
        if(0 == tmpLen)
        {
            for(int i=tmpLen-1;i>=0;i--)
            {
                TreeNodeInfo* tni = GlobalVariable::deviceTreeInfos[i];
                delete tni;
                GlobalVariable::deviceTreeInfos.removeAt(i);
            }
            GlobalVariable::deviceTreeInfos.clear();

            tmpLen = GlobalVariable::deviceInfos.size();
            for(int i=tmpLen-1;i>=0;i--)
            {
                TreeNodeInfo* tni = GlobalVariable::deviceInfos[i];
                delete tni;
                GlobalVariable::deviceInfos.removeAt(i);
            }
            GlobalVariable::deviceInfos.clear();

            //deviceRoot = new QTreeWidgetItem(deviceTree,QStringList(QString(tr("Devices"))),0);
            //deviceRoot->setIcon(0,QIcon(":/images/devices.png"));

            QString sql = "select * from deviceTree";
            QVector<QString> devicetrees = m_db->execSql(sql);
            sql = "select * from device";
            QVector<QString> devices = m_db->execSql(sql);

            QMap<int,QString> deviceItems;
            for(cIt=devices.constBegin();cIt!=devices.constEnd();cIt++)
            {
                QString row_str = *cIt;
                QStringList row_items = row_str.split(',');
                if(row_items.count()>=12)
                {
                    QString tree_id_str = row_items.at(8);
                    int tree_id = tree_id_str.toInt(&tok);
                    if(tok)
                    {
                        deviceItems.insert(tree_id,row_str);
                    }
                }
            }

            for(cIt=devicetrees.constBegin();cIt!=devicetrees.constEnd();cIt++)
            {
                QString row_str = *cIt;
                QStringList row_items = row_str.split(',');
                if(row_items.count()>=4)
                {
                    QString id_str = row_items.at(0);
                    int id = id_str.toInt(&tok);
                    QString type_str = row_items.at(1);
                    int type = type_str.toInt(&tok);
                    QString title = row_items.at(2);
                    QString pid_str = row_items.at(3);
                    int pid = pid_str.toInt(&tok);

                    TreeNodeInfo *mti = new TreeNodeInfo();
                    mti->id = id;
                    mti->type = (TREENODETYPE)type;
                    mti->title = title;
                    mti->pid = pid;

                    GlobalVariable::deviceTreeInfos.append(mti);

                    if(id > GlobalVariable::maxDeviceTreeId)
                    {
                        GlobalVariable::maxDeviceTreeId = id;
                    }
                }
            }

            for(mtiIt=GlobalVariable::deviceTreeInfos.constBegin();mtiIt!=GlobalVariable::deviceTreeInfos.constEnd();mtiIt++)
            {
                TreeNodeInfo* tmp = *mtiIt;
                if(tmp->type == DEVICE)
                {
                    /*
                    DeviceInfo* di = new DeviceInfo();
                    di->tree_id = tmp->id;
                    di->type = DEVICE;
                    di->title = tmp->title;
                    di->pid = tmp->pid;
                    */

                    if(deviceItems.contains(tmp->id))
                    {
                        QString device_info_s = deviceItems[tmp->id];
                        QStringList device_items = device_info_s.split(',');
                        QString deviceModel = device_items[3];

                        QString id_str = device_items[0];
                        int id = id_str.toInt(&tok);
                        QString port_str = device_items[6];

                        if (deviceModel.compare("CKYB0001TAGQ") == 0) //e7 electirc sample device
                        {
                            CMIEEDeviceInfo *eInfo = new CMIEEDeviceInfo();
                            eInfo->init();
                            eInfo->tree_id = tmp->id;
                            eInfo->type = DEVICE;
                            eInfo->pid = tmp->pid;
                            eInfo->deviceCode = device_items[1];
                            eInfo->id = id;
                            eInfo->title = tmp->title;
                            eInfo->ipAddress = device_items[5];
                            eInfo->ipPort = port_str.toInt(&tok);;
                            eInfo->deviceType = device_items[4];
                            eInfo->deviceModel = deviceModel;

                            QString tmp_str = device_items[9];
                            eInfo->measure_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[10];
                            eInfo->freq_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[11];
                            eInfo->wave_interval = tmp_str.toInt(&tok);

                            if(id > GlobalVariable::maxDeviceId)
                            {
                                GlobalVariable::maxDeviceId = id;
                            }

                            GlobalVariable::deviceInfos.append(eInfo);
                            QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(tmp->title),eInfo->tree_id);
                            deviceNode->setIcon(0,QIcon(":/images/outline.png"));
                            GlobalVariable::recordwave[eInfo->deviceCode] = 0;
                        }
                        else if (deviceModel.compare("CKYB3006LEHQ") == 0) //e7 vibrate sample device
                        {
                            CMIEVDeviceInfo *vInfo = new CMIEVDeviceInfo();
                            vInfo->init();
                            vInfo->tree_id = tmp->id;
                            vInfo->type = DEVICE;
                            vInfo->pid = tmp->pid;
                            vInfo->deviceCode = device_items[1];
                            vInfo->id = id;
                            vInfo->title = tmp->title;
                            vInfo->ipAddress = device_items[5];
                            vInfo->ipPort = port_str.toInt(&tok);;
                            vInfo->deviceType = device_items[4];
                            vInfo->deviceModel = deviceModel;

                            QString tmp_str = device_items[9];
                            vInfo->measure_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[10];
                            vInfo->freq_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[11];
                            vInfo->wave_interval = tmp_str.toInt(&tok);


                            if(id > GlobalVariable::maxDeviceId)
                            {
                                GlobalVariable::maxDeviceId = id;
                            }

                            GlobalVariable::deviceInfos.append(vInfo);
                            QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(tmp->title),vInfo->tree_id);
                            deviceNode->setIcon(0,QIcon(":/images/outline.png"));

                            GlobalVariable::recordwave[vInfo->deviceCode] = 0;
                        }
                        else if (deviceModel.compare("CKYB5003TEMQ") == 0) //e7 vibrate sample device
                        {
                            CMIETDeviceInfo *tInfo = new CMIETDeviceInfo();

                            tInfo->init();
                            tInfo->tree_id = tmp->id;
                            tInfo->type = DEVICE;
                            tInfo->pid = tmp->pid;
                            tInfo->deviceCode = device_items[1];
                            tInfo->id = id;
                            tInfo->title = tmp->title;
                            tInfo->ipAddress = device_items[5];
                            tInfo->ipPort = port_str.toInt(&tok);;
                            tInfo->deviceType = device_items[4];
                            tInfo->deviceModel = deviceModel;

                            QString tmp_str = device_items[9];
                            tInfo->measure_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[10];
                            tInfo->freq_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[11];
                            tInfo->wave_interval = tmp_str.toInt(&tok);

                            if(id > GlobalVariable::maxDeviceId)
                            {
                                GlobalVariable::maxDeviceId = id;
                            }

                            GlobalVariable::deviceInfos.append(tInfo);
                            QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(tmp->title),tInfo->tree_id);
                            deviceNode->setIcon(0,QIcon(":/images/outline.png"));
                        }
                        else if (deviceModel.compare("TW888") == 0) //e7 vibrate sample device
                        {
                            TW888DeviceInfo *twInfo = new TW888DeviceInfo();

                            twInfo->init();
                            twInfo->tree_id = tmp->id;
                            twInfo->type = DEVICE;
                            twInfo->pid = tmp->pid;
                            twInfo->deviceCode = device_items[1];
                            twInfo->id = id;
                            twInfo->title = tmp->title;
                            twInfo->ipAddress = device_items[5];
                            twInfo->ipPort = port_str.toInt(&tok);;
                            twInfo->deviceType = device_items[4];
                            twInfo->deviceModel = deviceModel;

                            QString tmp_str = device_items[9];
                            twInfo->measure_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[10];
                            twInfo->freq_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[11];
                            twInfo->wave_interval = tmp_str.toInt(&tok);

                            if(id > GlobalVariable::maxDeviceId)
                            {
                                GlobalVariable::maxDeviceId = id;
                            }

                            GlobalVariable::deviceInfos.append(twInfo);
                            QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(tmp->title),twInfo->tree_id);
                            deviceNode->setIcon(0,QIcon(":/images/outline.png"));
                        }
                        else
                        {
                            DeviceInfo *info = new DeviceInfo();

                            info->init();
                            info->tree_id = tmp->id;
                            info->type = DEVICE;
                            info->pid = tmp->pid;
                            info->deviceCode = device_items[1];
                            info->id = id;
                            info->title = tmp->title;
                            info->ipAddress = device_items[5];
                            info->ipPort = port_str.toInt(&tok);;
                            info->deviceType = device_items[4];
                            info->deviceModel = deviceModel;

                            QString tmp_str = device_items[9];
                            info->measure_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[10];
                            info->freq_interval = tmp_str.toInt(&tok);
                            tmp_str = device_items[11];
                            info->wave_interval = tmp_str.toInt(&tok);

                            if(id > GlobalVariable::maxDeviceId)
                            {
                                GlobalVariable::maxDeviceId = id;
                            }

                            GlobalVariable::deviceInfos.append(info);
                            QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(tmp->title),info->tree_id);
                            deviceNode->setIcon(0,QIcon(":/images/outline.png"));
                        }
                    }
                }
            }
        }
        else //switch from offline mode,just construct device tree
        {
            for(mtiIt=GlobalVariable::deviceTreeInfos.constBegin();mtiIt!=GlobalVariable::deviceTreeInfos.constEnd();mtiIt++)
            {
                TreeNodeInfo* tmp = *mtiIt;
                QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(tmp->title),tmp->id);
                deviceNode->setIcon(0,QIcon(":/images/outline.png"));
            }
        }

        deviceRoot->setExpanded(true);
    }
    else
    {

    }
}

void LeftTreeView::setupMotorTree()
{
    bool tok;
    QVector<QString>::ConstIterator cIt;
    QVector<TreeNodeInfo*>::ConstIterator mtiIt;

    int tmpLen = GlobalVariable::motorTreeInfos.size();
    for(int i=tmpLen-1;i>=0;i--)
    {
        TreeNodeInfo* tni = GlobalVariable::motorTreeInfos[i];
        delete tni;
        GlobalVariable::motorTreeInfos.removeAt(i);
    }
    GlobalVariable::motorTreeInfos.clear();

    tmpLen = GlobalVariable::motorInfos.size();
    for(int i=tmpLen-1;i>=0;i--)
    {
        TreeNodeInfo* tni = GlobalVariable::motorInfos[i];
        delete tni;
        GlobalVariable::motorInfos.removeAt(i);
    }
    GlobalVariable::motorInfos.clear();

    tmpLen = GlobalVariable::channelInfos.size();
    for(int i=tmpLen-1;i>=0;i--)
    {
        TreeNodeInfo* tni = GlobalVariable::channelInfos[i];
        delete tni;
        GlobalVariable::channelInfos.removeAt(i);
    }
    GlobalVariable::channelInfos.clear();

    motorTree->clear();

    motorRoot = new QTreeWidgetItem(motorTree,QStringList(QString(tr("Monitor Device"))),0);

    ///////////////////////////////////////////////////////
    QString sql = "select * from motorTree";
    QVector<QString> motortrees = m_db->execSql(sql);
    sql = "select m.id,m.mcode,m.name,m.motor_type,m.bearing_type,m.manufacture_date,m.treeid,mt.work_mode,mt.power_rating,mt.rated_voltage,mt.rated_current,mt.poleNums,mt.center_height,mt.factor,mt.insulate,mt.rotate,bt.btype,bt.rin,bt.rout,bt.contact_angle,bt.bearpitch,bt.rotated,bt.rotaten from motor m inner join motortype mt on m.motor_type = mt.model inner join bearingtype bt on m.bearing_type = bt.model;";
    QVector<QString> motors = m_db->execSql(sql);

    QMap<int,QString> motorItems;

    for(cIt=motors.constBegin();cIt!=motors.constEnd();cIt++)
    {
        QString row_str = *cIt;
        QStringList row_items = row_str.split(',');
        if(row_items.count()>=23)
        {
            QString tree_id_str = row_items.at(6);
            int tree_id = tree_id_str.toInt(&tok);
            if(tok)
            {
                motorItems.insert(tree_id,row_str);
            }
        }
    }

    sql = "select * from devicepipes";
    QVector<QString> channels = m_db->execSql(sql);

    QMap<int,QString> channelItems;
    for(cIt=channels.constBegin();cIt!=channels.constEnd();cIt++)
    {
        QString row_str = *cIt;
        QStringList row_items = row_str.split(',');
        if(row_items.count()>=11)
        {
            QString tree_id_str = row_items.at(10);
            int tree_id = tree_id_str.toInt(&tok);
            if(tok)
            {
                channelItems.insert(tree_id,row_str);
            }
        }
    }

    //QVector<QString> noParentNodes;
    for(cIt=motortrees.constBegin();cIt!=motortrees.constEnd();cIt++)
    {
        QString row_str = *cIt;
        QStringList row_items = row_str.split(',');
        if(row_items.count()>=4)
        {
            QString id_str = row_items.at(0);
            int id = id_str.toInt(&tok);
            QString type_str = row_items.at(1);
            int type = type_str.toInt(&tok);
            QString title = row_items.at(2);
            QString pid_str = row_items.at(3);
            int pid = pid_str.toInt(&tok);

            TreeNodeInfo *mti = new TreeNodeInfo();
            mti->id = id;
            mti->type = (TREENODETYPE)type;
            mti->title = title;
            mti->pid = pid;

            GlobalVariable::motorTreeInfos.append(mti);

            if(id > GlobalVariable::maxMotorTreeId)
            {
                GlobalVariable::maxMotorTreeId = id;
            }
        }
    }

    for(mtiIt=GlobalVariable::motorTreeInfos.constBegin();mtiIt!=GlobalVariable::motorTreeInfos.constEnd();mtiIt++)
    {
        TreeNodeInfo* tmp = *mtiIt;
        if(tmp->type == MOTOR)
        {
            MotorInfo* mi = new MotorInfo();
            mi->tree_id = tmp->id;
            mi->type = MOTOR;
            mi->title = tmp->title;
            mi->pid = tmp->pid;

            if(motorItems.contains(tmp->id))
            {
                QString motor_info_s = motorItems[tmp->id];
                QStringList motor_items = motor_info_s.split(',');
                mi->name = motor_items[2];
                mi->code = motor_items[1];
                QString id_str = motor_items[0];
                mi->id = id_str.toInt(&tok);
                mi->beartype = motor_items[4];
                mi->motortype = motor_items[3];
                mi->work_mode = motor_items[7];
                QString tmp = motor_items[8];
                mi->power = tmp.toFloat(&tok);
                tmp = motor_items[9];
                mi->voltage = tmp.toFloat(&tok);
                tmp = motor_items[10];
                mi->current = tmp.toFloat(&tok);
                tmp = motor_items[13];
                mi->factor = tmp.toFloat(&tok);
                tmp = motor_items[14];
                mi->insulate = tmp;
                tmp = motor_items[15];
                mi->rotate = tmp.toFloat(&tok);
                mi->btype = motor_items[16];
                tmp = motor_items[17];
                mi->rin = tmp.toFloat(&tok);
                tmp = motor_items[18];
                mi->rout = tmp.toFloat(&tok);
                tmp = motor_items[19];
                mi->contact_angle = tmp.toFloat(&tok);

                tmp = motor_items[20];
                mi->pitch = tmp.toFloat(&tok);
                tmp = motor_items[21];
                mi->roller_d = tmp.toFloat(&tok);
                tmp = motor_items[22];
                mi->roller_n = tmp.toFloat(&tok);

                MotorConditionTable mct;
                mct.u = mi->voltage;
                mct.i = mi->current;
                mct.rotate = mi->rotate;
                GlobalVariable::motorCondition[mi->code] = mct;
            }

            GlobalVariable::motorInfos.append(mi);

            if(mi->id > GlobalVariable::maxMotorId)
            {
                GlobalVariable::maxMotorId = mi->id;
            }

            QTreeWidgetItem* motor = new QTreeWidgetItem(motorRoot,QStringList(tmp->title),tmp->id);
            motor->setIcon(0,QIcon(":/images/motor.png"));
        }
    }

    for(mtiIt=GlobalVariable::motorTreeInfos.constBegin();mtiIt!=GlobalVariable::motorTreeInfos.constEnd();mtiIt++)
    {
        TreeNodeInfo* tmp = *mtiIt;
        if(tmp->type == CHANNEL_PHYISIC)
        {
            ChannelInfo* ci = new ChannelInfo();
            ci->tree_id = tmp->id;
            ci->type = CHANNEL_PHYISIC;
            ci->title = tmp->title;
            ci->pid = tmp->pid;

            if(channelItems.contains(ci->tree_id))
            {
                QString channel_info_s = channelItems[tmp->id];
                QStringList row_items = channel_info_s.split(',');

                QString id_str = row_items.at(0);
                int id = id_str.toInt(&tok);
                QString dcode_str = row_items.at(1);
                QString channel_code = row_items.at(2);
                QString name = row_items.at(3);
                QString motorcode = row_items.at(4);
                QString active_str = row_items.at(5);
                int active = active_str.toInt(&tok);
                QString channel_type_str = row_items.at(6);
                int channel_type = channel_type_str.toInt(&tok);

                QString rms1_s = row_items.at(7);
                float rms1_f = rms1_s.toFloat(&tok);

                QString rms2_s = row_items.at(8);
                float rms2_f = rms2_s.toFloat(&tok);

                //QString rms3_s = row_items.at(10);
                //float rms3_f = rms3_s.toInt(&tok);

                ci->id = id;
                ci->device_code = dcode_str;
                ci->channel_code = channel_code;
                ci->motor_code = motorcode;
                ci->channel_type = channel_type;
                ci->isactive = active;
                ci->rms1 = rms1_f;
                ci->rms2 = rms2_f;

                QString dname = GlobalVariable::findDeviceByCode(dcode_str)->title;

                QTreeWidgetItem* selectedItem = this->findTreeItem(motorRoot,tmp->pid);
                if(selectedItem != nullptr)
                {
                    QString nodeTitle = tmp->title + QObject::tr("[%1]").arg(dname);
                    QTreeWidgetItem* channelNode = new QTreeWidgetItem(selectedItem,QStringList(nodeTitle),tmp->id);
                    //channelNode->setText(0,nodeTitle);
                    QString dtype_in = GlobalVariable::findDeviceTypeByCode(dcode_str);
                    if(dtype_in.compare("E",Qt::CaseInsensitive) == 0)
                    {
                        channelNode->setIcon(0,QIcon(":/images/channel-e.png"));
                    }
                    else if(dtype_in.compare("V",Qt::CaseInsensitive) == 0)
                    {
                        channelNode->setIcon(0,QIcon(":/images/channel.png"));
                    }
                    else if(dtype_in.compare("T",Qt::CaseInsensitive) == 0)
                    {
                        channelNode->setIcon(0,QIcon(":/images/channel-t.jpg"));
                    }
                }
                if(id > GlobalVariable::maxChannelId)
                {
                    GlobalVariable::maxChannelId = id;
                }
            }

            GlobalVariable::channelInfos.append(ci);
        }
    }

    for(mtiIt=GlobalVariable::motorTreeInfos.constBegin();mtiIt!=GlobalVariable::motorTreeInfos.constEnd();mtiIt++)
    {
        TreeNodeInfo* tmp = *mtiIt;
        int type_int = (int)tmp->type;
        if(type_int >= 8)
        {
            QTreeWidgetItem* selectedItem = this->findTreeItem(motorRoot,tmp->pid);
            if(selectedItem != nullptr)
            {
                QTreeWidgetItem* channelNode = new QTreeWidgetItem(selectedItem,QStringList(tmp->title),tmp->id);
                channelNode->setIcon(0,QIcon(":/images/itemicon.png"));
            }
        }
    }

    motorRoot->setExpanded(true);
}

void LeftTreeView::setDeviceLinkState(int id,bool state)
{
    if(run_mode == 0)
    {
        QTreeWidgetItem* item = this->findTreeItem(deviceRoot,id);
        if(item != nullptr)
        {
            if(state)
            {
                item->setIcon(0,QIcon(":/images/online.png"));
            }
            else
            {
                item->setIcon(0,QIcon(":/images/outline.png"));
            }
        }
    }
}

void LeftTreeView::deivceEditStateChangeSlot(int id,QString title)
{
    int childCount = deviceRoot->childCount();

    bool flag = false;
    for(int i=0;i<childCount;i++)
    {
        QTreeWidgetItem* item = deviceRoot->child(i);
        if(id == item->type())
        {
            flag = true;
            break;
        }
    }

    if(!flag)
    {
        QTreeWidgetItem* deviceNode = new QTreeWidgetItem(deviceRoot,QStringList(title),id);
        deviceNode->setIcon(0,QIcon(":/images/online.png"));
        deviceRoot->setExpanded(true);
    }
    else
    {
        QTreeWidgetItem* item = this->findTreeItem(deviceRoot,id);
        if(item != nullptr)
        {
            item->setText(0,title);
        }
    }
}

void LeftTreeView::motorEditStateChangeSlot(int id,QString title)
{
    int childCount = motorRoot->childCount();

    bool flag = false;
    for(int i=0;i<childCount;i++)
    {
        QTreeWidgetItem* item = motorRoot->child(i);
        if(id == item->type())
        {
            flag = true;
            break;
        }
    }

    if(!flag)
    {
        QTreeWidgetItem* deviceNode = new QTreeWidgetItem(motorRoot,QStringList(title),id);
        deviceNode->setIcon(0,QIcon(":/images/motor.png"));
        motorRoot->setExpanded(true);
    }
    else
    {
        QTreeWidgetItem* item = this->findTreeItem(motorRoot,id);
        if(item != nullptr)
        {
            item->setText(0,title);
        }
    }
}

void LeftTreeView::editSlot()
{
    QTreeWidgetItem* selectedItem = nullptr;
    QString sql;
    QString tmp;
    QQueue<QString> sqls;

    if(currentSelectId > 0)
    {
        switch (currentSelectType) {
        case MOTORROOT:

            break;
        case MOTOR:
            {
                MotorEdit *motoredit = new MotorEdit(m_db);
                motoredit->setmode(currentSelectId);
                motoredit->setAttribute(Qt::WA_DeleteOnClose);
                connect(motoredit,SIGNAL(motorEditStateChange(int,QString)),this,SLOT(motorEditStateChangeSlot(int,QString)));
                motoredit->exec();
            }
            break;
        case CHANNEL_PHYISIC:
            {
                DevicePipeEdit *typeEdit = new DevicePipeEdit(m_db);
                typeEdit->setmode(currentSelectId);
                typeEdit->setMotorId(currentSelectId);
                connect(typeEdit,SIGNAL(channelEditStateChange(int,QString,int,int)),this,SLOT(channelEditStateChangeSlot(int,QString,int,int)));
                typeEdit->setAttribute(Qt::WA_DeleteOnClose);
                typeEdit->exec();
            }
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
            {
                DeviceEditDialog *devieDialog = new DeviceEditDialog(m_db);
                devieDialog->setmode(currentSelectId);
                devieDialog->setAttribute(Qt::WA_DeleteOnClose);
                connect(devieDialog,SIGNAL(deivceEditStateChange(int,QString)),this,SLOT(deivceEditStateChangeSlot(int,QString)));
                devieDialog->exec();
            }
            break;
        default:
            break;
        }
    }
}

void LeftTreeView::deleteSlot()
{
    if(1 == run_mode)
    {
        QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Can not delete item in offline mode"));
        return;
    }

    QTreeWidgetItem* selectedItem = nullptr;
    QString sql;
    QString tmp;
    QString tmp2;
    QQueue<QString> sqls;
    int count;

    QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {
        if(currentSelectId > 0)
        {
            switch (currentSelectType) {
            case MOTORROOT:

                break;
            case MOTOR:
                sql = "select * from motortree where pid=" + QString::number(currentSelectId);
                if(m_db->queryCount(sql) > 0)
                {
                    QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Please Delete sub items!"));
                }
                else
                {
                    tmp2 = GlobalVariable::findMotorCodeById(currentSelectId);
                    if(tmp2.length()>0)
                    {
                        tmp = "delete from motortree where id=" + QString::number(currentSelectId);
                        sqls.enqueue(tmp);
                        tmp = "delete from motor where treeid=" + QString::number(currentSelectId);
                        sqls.enqueue(tmp);

                        QString mcode = GlobalVariable::findMotorCodeById(currentSelectId);
                        if(mcode.length()>0)
                        {
                            tmp = "delete from motorconditiontable where mcode='" + mcode + "'";
                            sqls.enqueue(tmp);
                        }

                        if(m_db->execSql(sqls))
                        {
                            selectedItem = this->findTreeItem(motorRoot,currentSelectId);
                            if(selectedItem != nullptr)
                            {
                                if(selectedItem->parent() != nullptr)
                                {
                                    selectedItem->parent()->removeChild(selectedItem);
                                }
                                //motorRoot->removeChild(selectedItem);
                            }

                            if(GlobalVariable::findMotorIndexByTreeId(currentSelectId) >= 0)
                            {
                                GlobalVariable::motorInfos.removeAt(GlobalVariable::findMotorIndexByTreeId(currentSelectId));
                            }
                        }
                        else
                        {
                            QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Delete item failed!"));
                        }
                    }
                }
                break;
            case CHANNEL_PHYISIC:
                sql = "select * from motortree where pid=" + QString::number(currentSelectId);
                if(m_db->queryCount(sql) > 0)
                {
                    QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Please Delete sub items!"));
                }
                else
                {
                    sql = "delete from devicepipes where treeid=" + QString::number(currentSelectId);
                    sqls.enqueue(sql);
                    tmp = "delete from motortree where id=" + QString::number(currentSelectId);
                    sqls.enqueue(tmp);

                    if(m_db->execSql(sqls))
                    {
                        selectedItem = this->findTreeItem(motorRoot,currentSelectId);
                        if(selectedItem != nullptr)
                        {
                            if(selectedItem->parent() != nullptr)
                            {
                                selectedItem->parent()->removeChild(selectedItem);
                            }
                            //motorRoot->removeChild(selectedItem);
                        }
                        count = GlobalVariable::channelInfos.count();
                        for(int i=count-1;i>=0;i--)
                        {
                            ChannelInfo* ci = GlobalVariable::channelInfos[i];
                            if(ci->tree_id == currentSelectId)
                            {
                                GlobalVariable::channelInfos.removeAt(i);
                            }
                        }
                    }
                    else
                    {
                        QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Delete item failed!"));
                    }
                }
                break;
            case MEASURE:
            case MEASURE_SPEED:
            case MEASURE_POS:
            case WAVE:
            case FREQENCY:
            case WAVE_A:
            case SPEED_WAVE_A:
            case WAVE_FREQ_A:
            case SPEED_FREQ_A:
            case EN_FREQ_A:
            case UMEASURE:
            case IMEASURE:
            case UWAVE:
            case IWAVE:
            case UFREQENCY:
            case IFREQENCY:
            case WATERFALL_ACC:
            case WATERFALL_SPD:
            case WATERFALL_VOL:
            case WATERFALL_CUR:
            case DIS_WAVE_A:
            case ACC_POWER:
            case ACC_Cepstrum:
            case SPD_POWER:
            case SPD_Cepstrum:
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
            case DIAGNOSE_REPORT:
            case TEMP_MEASURE:
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
            case PARK_TRANSFORMATION:
                sql = "delete from motortree where id=" + QString::number(currentSelectId);
                sqls.enqueue(sql);

                if(m_db->execSql(sqls))
                {
                    selectedItem = this->findTreeItem(motorRoot,currentSelectId);
                    if(selectedItem != nullptr)
                    {
                        if(selectedItem->parent() != nullptr)
                        {
                            selectedItem->parent()->removeChild(selectedItem);
                        }
                    }
                }
                else
                {
                    QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Delete item failed!"));
                }
                break;
            case PATH_TRACKING:
                sql = "delete from motortree where id=" + QString::number(currentSelectId);
                sqls.enqueue(sql);
                sql = "delete from pathtracking where treeid=" + QString::number(currentSelectId);
                sqls.enqueue(sql);

                if(m_db->execSql(sqls))
                {
                    selectedItem = this->findTreeItem(motorRoot,currentSelectId);
                    if(selectedItem != nullptr)
                    {
                        if(selectedItem->parent() != nullptr)
                        {
                            selectedItem->parent()->removeChild(selectedItem);
                        }
                    }
                }
                else
                {
                    QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Delete item failed!"));
                }
                break;
                break;
            case DEVICEROOT:

                break;
            case DEVICE:
                //判断该设备是否在使用。
                tmp = GlobalVariable::findDeviceCodeByTreeId(currentSelectId);
                if(tmp.length()>0)
                {
                    sql = "select * from devicepipes where dcode='" + tmp + "'";
                    if(m_db->queryCount(sql) > 0)
                    {
                        QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("This device is using!"));
                    }
                    else
                    {
                        sql = "delete from devicetree where id=" + QString::number(currentSelectId);
                        sqls.enqueue(sql);
                        tmp = "delete from device where treeid=" + QString::number(currentSelectId);
                        sqls.enqueue(tmp);

                        if(m_db->execSql(sqls))
                        {
                            selectedItem = this->findTreeItem(deviceRoot,currentSelectId);
                            if(selectedItem != nullptr)
                            {
                                deviceRoot->removeChild(selectedItem);
                            }
                            if(GlobalVariable::findDeviceByTreeId(currentSelectId) >= 0)
                            {
                                GlobalVariable::deviceInfos.removeAt(GlobalVariable::findDeviceByTreeId(currentSelectId));
                            }
                            if(GlobalVariable::findDeviceTreeIndexById(currentSelectId) >= 0)
                            {
                                GlobalVariable::deviceTreeInfos.removeAt(GlobalVariable::findDeviceTreeIndexById(currentSelectId));
                            }
                        }
                        else
                        {
                            QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Delete item failed!"));
                        }
                    }
                }
                else
                {
                    QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Delete item failed!"));
                }

                break;
            default:
                break;
            }
        }
    }
}

QTreeWidgetItem* LeftTreeView::findTreeItem(QTreeWidgetItem* treeNode,int type)
{
    int count = treeNode->childCount();
    for(int i=0;i<count;i++)
    {
        QTreeWidgetItem* item = treeNode->child(i);
        if(item->type() == type)
        {
            return item;
        }
        else if(item->childCount()>0)
        {
            QTreeWidgetItem* itemsub = findTreeItem(item,type);
            if(itemsub != nullptr)
            {
                return itemsub;
            }
        }
    }
    return nullptr;
}

void LeftTreeView::newPhysicChannelSlot()
{
    DevicePipeEdit *typeEdit = new DevicePipeEdit(m_db);
    typeEdit->setmode(-1);
    typeEdit->setMotorId(currentSelectId);
    connect(typeEdit,SIGNAL(channelEditStateChange(int,QString,int,int)),this,SLOT(channelEditStateChangeSlot(int,QString,int,int)));
    typeEdit->setAttribute(Qt::WA_DeleteOnClose);
    typeEdit->exec();
}

void LeftTreeView::channelEditStateChangeSlot(int tree_id,QString title,int pid, int channel_type)
{
    QTreeWidgetItem* selectedpipeItem = this->findTreeItem(motorRoot,tree_id);
    if(selectedpipeItem == nullptr)
    {
        QTreeWidgetItem* selectedItem = this->findTreeItem(motorRoot,pid);
        if(selectedItem != nullptr)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(selectedItem,QStringList(title),tree_id);
            if(channel_type <= 30)
            {
                if(channel_type <= 20)
                {
                    item->setIcon(0,QIcon(":/images/channel.png"));
                }
                else
                {
                    item->setIcon(0,QIcon(":/images/channel-e.png"));
                }
            }
            else
            {
                item->setIcon(0,QIcon(":/images/channel-t.jpg"));
            }

            item->setExpanded(true);
        }
        else
        {
            QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Create Channel failed!"));
        }
    }
    else
    {
        selectedpipeItem->setText(0,title);
    }
}

void LeftTreeView::newPosTrendSlot()
{
    this->handlerShowClick(currentSelectId,tr("Postion RMS Trend"),MEASURE_POS);
}

void LeftTreeView::newPosWaveSlot()
{
    this->handlerShowClick(currentSelectId,tr("Postion Wave"),DIS_WAVE_A);
}

void LeftTreeView::newFeatureSlot()
{
    this->handlerShowClick(currentSelectId,tr("Accelerate RMS Trend"),MEASURE);
}

void LeftTreeView::newSpeedFeatureSlot()
{
    this->handlerShowClick(currentSelectId,tr("Speed RMS Trend"),MEASURE_SPEED);
}

void LeftTreeView::newFrequencySlot()
{
    this->handlerShowClick(currentSelectId,tr("Accelerate Frequency"),FREQENCY);
}

void LeftTreeView::newWaveSlot()
{
    this->handlerShowClick(currentSelectId,tr("Accelerate Wave"),WAVE);
}

void LeftTreeView::newSpdWaveSlot()
{
    this->handlerShowClick(currentSelectId,tr("Speed Wave"),SPEED_WAVE_A);
}

void LeftTreeView::newSpdFrequencySlot()
{
    this->handlerShowClick(currentSelectId,tr("Speed Frequency"),SPEED_FREQ_A);
}

void LeftTreeView::newAccPowerSpectrumSlot()
{
    this->handlerShowClick(currentSelectId,tr("Accelerate Power Spectrum"),ACC_POWER);
}

void LeftTreeView::newSpdPowerSpectrumSlot()
{
    this->handlerShowClick(currentSelectId,tr("Speed Power Spectrum"),SPD_POWER);
}

void LeftTreeView::newAccCepstrumSlot()
{
    this->handlerShowClick(currentSelectId,tr("Accelerate Cepstrum"),ACC_Cepstrum);
}

void LeftTreeView::newSpdCepstrumSlot()
{
    this->handlerShowClick(currentSelectId,tr("Speed Cepstrum"),SPD_Cepstrum);
}

void LeftTreeView::newEnvFrequencySlot()
{
    this->handlerShowClick(currentSelectId,tr("Accelerate Envelop"),EN_FREQ_A);
}

void LeftTreeView::newUFeatureSlot()
{
    this->handlerShowClick(currentSelectId,tr("U RMS Trend"),UMEASURE);
}

void LeftTreeView::newIFeatureSlot()
{
    this->handlerShowClick(currentSelectId,tr("I RMS Trend"),IMEASURE);
}

void LeftTreeView::newUWaveSlot()
{
    this->handlerShowClick(currentSelectId,tr("U Wave"),UWAVE);
}

void LeftTreeView::newIWaveSlot()
{
    this->handlerShowClick(currentSelectId,tr("I Wave"),IWAVE);
}

void LeftTreeView::newUFreqencySlot()
{
    this->handlerShowClick(currentSelectId,tr("U Freqency"),UFREQENCY);
}

void LeftTreeView::newIFreqencySlot()
{
    this->handlerShowClick(currentSelectId,tr("I Freqency"),IFREQENCY);
}

void LeftTreeView::newactPowerSlot()
{
    this->handlerShowClick(currentSelectId,tr("Act Power"),ACTPOWER);
}

void LeftTreeView::newreactPowerSlot()
{
    this->handlerShowClick(currentSelectId,tr("React Power"),REACTPOWER);
}

void LeftTreeView::newappPowerSlot()
{
    this->handlerShowClick(currentSelectId,tr("App Power"),APPPOWER);
}

void LeftTreeView::newcosfSlot()
{
    this->handlerShowClick(currentSelectId,tr("CosF"),COSF);
}

void LeftTreeView::newupsequenceSlot()
{
    this->handlerShowClick(currentSelectId,tr("Voltage Positive Sequence"),U_P_SEQUENCE);
}

void LeftTreeView::newunsequenceSlot()
{
    this->handlerShowClick(currentSelectId,tr("Voltage Negative Sequence"),U_N_SEQUENCE);
}

void LeftTreeView::newuzsequenceSlot()
{
    this->handlerShowClick(currentSelectId,tr("Voltage Zero Sequence"),U_Z_SEQUENCE);
}

void LeftTreeView::newipsequenceSlot()
{
    this->handlerShowClick(currentSelectId,tr("Current Positive Sequence"),I_P_SEQUENCE);
}

void LeftTreeView::newinsequenceSlot()
{
    this->handlerShowClick(currentSelectId,tr("Current Negative Sequence"),I_N_SEQUENCE);
}

void LeftTreeView::newizsequenceSlot()
{
    this->handlerShowClick(currentSelectId,tr("Current Zero Sequence"),I_Z_SEQUENCE);
}

void LeftTreeView::itemSelectedSlot(TREENODETYPE tn,QString title)
{
    this->handlerShowClick(currentSelectId,title,tn);
}

void LeftTreeView::pathTrackingSelectedSlot(int x_tree_id,int y_tree_id)
{
    QString title = tr("Path tracking");
    QString x_title = GlobalVariable::findChannelNameByTreeId(x_tree_id);
    QString y_title = GlobalVariable::findChannelNameByTreeId(y_tree_id);
    QString title_name = title + "[" + x_title + " " + y_title + "]";

    QString sql = "insert into pathtracking(channel_tree_id_1,channel_tree_id_2,treeid) values(" + QString::number(x_tree_id) + "," + QString::number(y_tree_id) + "," + QString::number(GlobalVariable::maxMotorTreeId + 1) + ")";
    if(m_db->updatasql(sql))
    {
        this->handlerShowClick(currentSelectId,title_name,PATH_TRACKING);
    }
    else
    {
        QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Create Path tracking failed!"));
    }
}

void LeftTreeView::waterfallaccSlot()
{
    this->handlerShowClick(currentSelectId,tr("Acc Water Fall Plot"),WATERFALL_ACC);
}

void LeftTreeView::waterfallspdSlot()
{
    this->handlerShowClick(currentSelectId,tr("Speed Water Fall Plot"),WATERFALL_SPD);
}

void LeftTreeView::waterfallvolSlot()
{
    this->handlerShowClick(currentSelectId,tr("Voltage Water Fall Plot"),WATERFALL_VOL);
}

void LeftTreeView::waterfallcurSlot()
{
    this->handlerShowClick(currentSelectId,tr("Current Water Fall Plot"),WATERFALL_CUR);
}

void LeftTreeView::pathtrackingSlot()
{
    PathTrackingChannelSelect *fxsd = new PathTrackingChannelSelect;
    fxsd->setAttribute(Qt::WA_DeleteOnClose);
    fxsd->setmode(currentSelectId);
    connect(fxsd,SIGNAL(channelSelected(int,int)),this,SLOT(pathTrackingSelectedSlot(int,int)));

    fxsd->exec();
}

void LeftTreeView::parktransformationSlot()
{
    this->handlerShowClick(currentSelectId,tr("Park Transformation"),PARK_TRANSFORMATION);
}

void LeftTreeView::tempMeasureSlot()
{
    this->handlerShowClick(currentSelectId,tr("Measure"),TEMP_MEASURE);
}

void LeftTreeView::syncMotorDataSlot()
{
    /*
    QString data = m_db->getProjectData();

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
    */

    SyncDataInfo *sdi = new SyncDataInfo;
    sdi->setAttribute(Qt::WA_DeleteOnClose);

    sdi->exec();
}

void LeftTreeView::diagnosereportSlot()
{
    this->handlerShowClick(currentSelectId,tr("Diagnose Result"),DIAGNOSE_REPORT);
}

void LeftTreeView::newXFreqSlot()
{
    FreqXSelectDialog *fxsd = new FreqXSelectDialog;
    fxsd->setAttribute(Qt::WA_DeleteOnClose);
    connect(fxsd,SIGNAL(itemSelected(TREENODETYPE,QString)),this,SLOT(itemSelectedSlot(TREENODETYPE,QString)));

    QString deviceCode = GlobalVariable::findDeviceCodeByChannelTreeId(currentSelectId);
    QString deviceType = GlobalVariable::findDeviceTypeByCode(deviceCode);

    if(deviceType.compare("V",Qt::CaseInsensitive) == 0)
    {
        fxsd->setMode(0);
    }
    else if(deviceType.compare("E",Qt::CaseInsensitive) == 0)
    {
        fxsd->setMode(1);
    }
    else
    {

    }

    fxsd->exec();
}
void LeftTreeView::handlerShowClick(int id,QString title,int type)
{
    QTreeWidgetItem* selectedItem = this->findTreeItem(motorRoot,id);
    if(selectedItem != nullptr)
    {
        GlobalVariable::maxMotorTreeId++;
        QTreeWidgetItem* item = new QTreeWidgetItem(selectedItem,QStringList(title),GlobalVariable::maxMotorTreeId);

        TreeNodeInfo* tni = new TreeNodeInfo();
        tni->id = GlobalVariable::maxMotorTreeId;
        tni->type = (TREENODETYPE)type;
        tni->pid = selectedItem->type();
        tni->title = title;

        item->setIcon(0,QIcon(":/images/itemicon.png"));

        QString sql = "insert into motortree (id,type,title,pid) values (" + QString::number(tni->id) + "," + QString::number(type) +",'" + tni->title + "'," + QString::number(tni->pid) + ")";

        if(m_db->updatasql(sql))
        {
            GlobalVariable::motorTreeInfos.append(tni);
        }
        else
        {
            delete tni;
            QMessageBox::information(this, QStringLiteral("Infomation"), QStringLiteral("Create Measure failed!"));
            selectedItem->removeChild(item);
        }

        selectedItem->setExpanded(true);
    }
}

void LeftTreeView::showDeviceSelectedChart(QTreeWidgetItem* ditem,int)
{
    if(!GlobalVariable::isOnline)
    {
        QTreeWidgetItem* item = motorTree->currentItem();
        int type = item->type();

        QVector<TreeNodeInfo*>::ConstIterator mtiIt;
        for(mtiIt=GlobalVariable::motorTreeInfos.constBegin();mtiIt!=GlobalVariable::motorTreeInfos.constEnd();mtiIt++)
        {
            TreeNodeInfo* tni = *mtiIt;
            if(type == tni->id)
            {
                if(tni->type >= 8)
                {
                    QTreeWidgetItem* pItem = item->parent();

                    int wave_id = ditem->type();
                    //QString t_s_data = "";
                    QString dcode = GlobalVariable::findDeviceCodeByChannelTreeId(item->parent()->type());
                    /*
                    if(dcode.length() > 0)
                    {
                        QString dtype = GlobalVariable::findDeviceTypeByCode(dcode);
                        if(dtype.compare("E",Qt::CaseInsensitive) == 0)
                        {
                            t_s_data = m_db->getWaveById(CHARGEWAVEINFO,wave_id);
                        }
                        else if(dtype.compare("V",Qt::CaseInsensitive) == 0)
                        {
                            t_s_data = m_db->getWaveById(VIBRATEWAVEINFO,wave_id);
                        }
                    }
                    */

                    QString c_code = GlobalVariable::findChannelCodeByTreeId(item->parent()->type());

                    emit showRightController(pItem->type(),item->type(), tni->title,((int)tni->type),off_dt.toString(GlobalVariable::dtFormat),off_e_dt.toString(GlobalVariable::dtFormat),ditem->text(0),wave_id,1);
                }
                break;
            }
        }
    }
}

void LeftTreeView::showSelectedChart(QTreeWidgetItem* item,int)
{
    int type = item->type();

    QVector<TreeNodeInfo*>::ConstIterator mtiIt;
    for(mtiIt=GlobalVariable::motorTreeInfos.constBegin();mtiIt!=GlobalVariable::motorTreeInfos.constEnd();mtiIt++)
    {
        TreeNodeInfo* tni = *mtiIt;
        if(type == tni->id)
        {
            if(tni->type >= 8)
            {
                QTreeWidgetItem* pItem = item->parent();

                emit showRightController(pItem->type(),item->type(), tni->title,((int)tni->type),"","","",0,0);
            }
            break;
        }
    }
}

void LeftTreeView::ddeleteSlot()
{
    QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Are you sure delete this item?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {
        QString sql = "";
        if(0 == offline_cur_table)
        {
            sql = "delete from electricchargewave where id=" + QString::number(currentSelectId);
        }
        else
        {
            sql = "delete from vibratewave where id=" + QString::number(currentSelectId);
        }

        if(sql.length()>0 && m_db->updatasql(sql))
        {
            int nindex = -1;
            int topCount = deviceTree->topLevelItemCount();
            for(int i=0;i<topCount;i++)
            {
                QTreeWidgetItem* item = deviceTree->topLevelItem(i);
                if(item->type() == currentSelectId)
                {
                    nindex = i;
                    break;
                }
            }
            if(nindex >=0)
            {
                deviceTree->takeTopLevelItem(nindex);
            }
        }
    }
}

void LeftTreeView::dexportSlot()
{
    QString t_s_data="";
    QString filename = "./" + QString::number(currentSelectId);
    if(0 == offline_cur_table)
    {
        t_s_data = m_db->getWaveById(CHARGEWAVEINFO,currentSelectId);
        filename += "-E";
    }
    else
    {
        t_s_data = m_db->getWaveById(VIBRATEWAVEINFO,currentSelectId);
        filename += "-V";
    }

    filename += ".csv";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), filename, tr("Txt file(*.csv);;All file(*.*)"));//getOpenFileName(this, tr("select file"),"./",tr("Txt file(*.txt);;All file(*.*)"));
    if(fileName.isEmpty())
    {
        QMessageBox mesg;
        mesg.warning(this,"Warning","Open file failed！");
        return;
    }
    else
    {
        QFile csvFile(fileName);

        bool fileopen = csvFile.open(QIODevice::ReadWrite);

        QStringList::const_iterator constIterator;
        //chartFreq->clearGraph(0);
        bool tok;
        QStringList waves = t_s_data.split(",");
        for (constIterator = waves.constBegin(); constIterator != waves.constEnd();++constIterator)
        {
            QString item = *constIterator;

            if(fileopen)
            {
                QString v_e_str = item + "\n";
                csvFile.write(v_e_str.toUtf8());
            }
        }

        csvFile.close();

        QMessageBox::information(this, tr("Infomation"), tr("Export Data finished!"));
    }
}

TREENODETYPE LeftTreeView::getCurNodeType()
{
    return currentSelectType;
}
