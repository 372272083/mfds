#ifndef DEVICEPIPEEDIT_H
#define DEVICEPIPEEDIT_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QComboBox;
class SqliteDB;
class QLabel;

class DevicePipeEdit : public QDialog
{
    Q_OBJECT
public:
    explicit DevicePipeEdit(SqliteDB *db,QWidget *parent = 0);
    void setmode(int value);
    void setmodel(QList<QString> values);

    void setMotorId(int motor_id);
private:
    int mode;
    int motor_id;

    int diagnose_tab_index;

    QLineEdit *pipeNameEdit;
    QComboBox *deviceCombo;
    QComboBox *channelCombo;

    QLineEdit *acc_ampThresholdEdit;
    QLineEdit *acc_rmsThresholdEdit;
    QLineEdit *acc_pkpkThresholdEdit;

    QLineEdit *spd_ampThresholdEdit;
    QLineEdit *spd_rmsThresholdEdit;
    QLineEdit *spd_pkpkThresholdEdit;

    QLabel *paccampLabel;
    QLabel *paccrmsLabel;
    QLabel *paccpkpkLabel;

    QLabel *pspdampLabel;
    QLabel *pspdrmsLabel;
    QLabel *pspdpkpkLabel;

    QLabel *paccMetricLabel;
    QLabel *paccrmsMetricLabel;
    QLabel *paccpkpkMetricLabel;

    QLabel *pspdMetricLabel;
    QLabel *pspdrmsMetricLabel;
    QLabel *pspdpkpkMetricLabel;

    SqliteDB* m_db;

    QString dcode;
    QString pipecode;

    bool editpipe();
signals:
    void channelEditStateChange(int,QString,int,int);
public slots:
    void ok();
    void cancel();

    void deviceSelectChanged(const QString &);
};

#endif // DEVICEPIPEEDIT_H
