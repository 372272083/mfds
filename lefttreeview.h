#ifndef LEFTTREEVIEW_H
#define LEFTTREEVIEW_H

#include <QWidget>
#include <QDateTime>
#include "constants.h"

class SqliteDB;
class QTreeWidget;
class QLineEdit;
class QTreeWidgetItem;
class QAction;
class QLabel;
//class QDateTime;

class LeftTreeView : public QWidget
{
    Q_OBJECT
public:
    explicit LeftTreeView(SqliteDB *db,QWidget *parent = 0);

    void setupMotorTree();
    void setupDeviceTree();
    void setDeviceLinkState(int,bool);

    void switchMode(int mode);
    void setOffDatetime(QDateTime,QDateTime,QString title="");
    TREENODETYPE getCurNodeType();

    void updateActionState(int);

private:
    SqliteDB* m_db;

    QTreeWidget* motorTree;
    QTreeWidgetItem* motorRoot;

    QTreeWidget* deviceTree;
    QTreeWidgetItem* deviceRoot;

    QLineEdit* m_pSearchLineEdit;
    QLabel* deviceTitle;

    int run_mode;
    QString offline_title;
    QDateTime off_dt;
    QDateTime off_e_dt;

    //QAction *copyActoin;
    //QAction *pasteActoin;

    QAction *newMotor;
    QAction *newDevice;

    QAction *editActoin;
    QAction *deleteActoin;

    QAction *ddeleteActoin;
    QAction *dexportActoin;

    QAction *waterfallaccAction;
    QAction *waterfallspdAction;
    QAction *waterfallvolAction;
    QAction *waterfallcurAction;

    //QAction *newChannel;
    QAction *newPhyisicChannel;
    QAction *newComposeChannel;
    QAction *newCompareChannel;

    QAction *newPathAction;
    QAction *newReportAction;
    QAction *newParkAction;

    //QAction *newChart;
    QAction *newFreqXChart;
    QAction *newFeatureChart;
    QAction *newSpeedFeatureChart;
    QAction *newPosFeatureChart;
    QAction *newFrequencyChart;
    QAction *newWaveChart;
    QAction *newSpeedChart;
    QAction *newDisChart;
    QAction *newSpeedFreqChart;
    QAction *newEnFreqChart;
    QAction *newAccPowerFreqChart;
    QAction *newSpdPowerFreqChart;
    QAction *newAccCepstrumChart;
    QAction *newSpdCepstrumChart;

    QAction *newUFeatureChart;
    QAction *newIFeatureChart;
    QAction *newUWaveChart;
    QAction *newIWaveChart;
    QAction *newUFreqencyChart;
    QAction *newIFreqencyChart;

    QAction *newactPowerChart;
    QAction *newreactPowerChart;
    QAction *newappPowerChart;
    QAction *newcosfChart;

    QAction *newUPSequenceChart;
    QAction *newUNSequenceChart;
    QAction *newUZSequenceChart;

    QAction *newIPSequenceChart;
    QAction *newINSequenceChart;
    QAction *newIZSequenceChart;

    QAction *newTemperatureChart;

    QAction *syncMotorData;

    int currentSelectId;
    TREENODETYPE currentSelectType;

    int offline_cur_table;

private:
    QTreeWidgetItem* findTreeItem(QTreeWidgetItem*,int);
    void handlerShowClick(int id,QString title,int);
    void setupDatalist(int);
signals:
    void showRightController(int,int,QString,int,QString,QString,QString,int,int);
    void treeSelectedChange(int);

public slots:
    void search();
    void itemMotorPressedSlot(QTreeWidgetItem *, int);
    void itemDevicePressedSlot(QTreeWidgetItem *, int);

    void newMotorSlot();
    void newDeviceSlot();

    void deivceEditStateChangeSlot(int id,QString title);
    void motorEditStateChangeSlot(int id,QString title);
    void channelEditStateChangeSlot(int,QString,int,int);

    void newPhysicChannelSlot();

    void newFeatureSlot();
    void newSpeedFeatureSlot();
    void newFrequencySlot();
    void newWaveSlot();

    void newSpdWaveSlot();
    void newSpdFrequencySlot();
    void newEnvFrequencySlot();

    void newAccPowerSpectrumSlot();
    void newSpdPowerSpectrumSlot();
    void newAccCepstrumSlot();
    void newSpdCepstrumSlot();

    void newPosTrendSlot();
    void newPosWaveSlot();

    void newUFeatureSlot();
    void newIFeatureSlot();
    void newUWaveSlot();
    void newIWaveSlot();
    void newUFreqencySlot();
    void newIFreqencySlot();
    void newXFreqSlot();

    void newactPowerSlot();
    void newreactPowerSlot();
    void newappPowerSlot();
    void newcosfSlot();

    void newupsequenceSlot();
    void newunsequenceSlot();
    void newuzsequenceSlot();

    void newipsequenceSlot();
    void newinsequenceSlot();
    void newizsequenceSlot();

    void waterfallaccSlot();
    void waterfallspdSlot();
    void waterfallvolSlot();
    void waterfallcurSlot();

    void pathtrackingSlot();
    void diagnosereportSlot();
    void parktransformationSlot();

    void tempMeasureSlot();

    void itemSelectedSlot(TREENODETYPE,QString);
    void pathTrackingSelectedSlot(int,int);

    void showSelectedChart(QTreeWidgetItem*,int);
    void showDeviceSelectedChart(QTreeWidgetItem*,int);

    void deleteSlot();
    void editSlot();

    void ddeleteSlot();
    void dexportSlot();

    void syncMotorDataSlot();
};

#endif // LEFTTREEVIEW_H
