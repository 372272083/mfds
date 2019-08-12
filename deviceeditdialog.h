#ifndef DEVICEEDITDIALOG_H
#define DEVICEEDITDIALOG_H

#include <QDialog>
class SqliteDB;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QTabWidget;

class DeviceEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceEditDialog(SqliteDB *db,QWidget *parent = 0);
    ~DeviceEditDialog();

    void setmode(int value);

private:
    SqliteDB *m_db;
    int mode;

    QLineEdit *codeEdit;
    QLineEdit *nameEdit;
    QComboBox *typeCombo;
    QPushButton *typeEditBtn;
    QLineEdit *ipaddressText;
    QLineEdit *portEdit;

    QLineEdit *measureIntervalEdit;
    QLineEdit *freqIntervalEdit;
    QLineEdit *waveIntervalEdit;

    QComboBox *sampleNumEdit;
    QComboBox *sampleIntervalEdit;

    //QLabel *pmodeLabel;
    //QCheckBox *deviceModeCB;

    int ratiodialog_index;

    QString deviceType;

    QLineEdit *channel1Edit;
    QLineEdit *channel2Edit;
    QLineEdit *channel3Edit;
    QLineEdit *channel4Edit;
    QLineEdit *channel5Edit;
    QLineEdit *channel6Edit;

    QLabel *channel1Label;
    QLabel *channel2Label;
    QLabel *channel3Label;
    QLabel *channel4Label;
    QLabel *channel5Label;
    QLabel *channel6Label;

    QTabWidget *mainWidget;

    QLabel *pfreqLabel;
    QLabel *pfintervalMetricLabel;

    QLabel *prinLabel;
    QLabel *pwintervalMetricLabel;

    QLabel *psampleNumLabel;
    QLabel *psampleIntervalLabel;

    QLabel *pmeasureLabel;
    QLabel *pmintervalMetricLabel;

    //QStringList motorList;
    //QStringList deviceModelList;

    float ratio1;
    float ratio2;
    float ratio3;
    float ratio4;
    float ratio5;
    float ratio6;

    int sample_num;
    float sample_interval;

    bool run_mode;

    bool editdevice();

signals:
    void deivceEditStateChange(int,QString);

public slots:
    void ok();
    void cancel();

    void deviceSelectChanged(const QString &);
};

#endif // DEVICEEDITDIALOG_H
