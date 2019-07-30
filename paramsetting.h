#ifndef PARAMSETTING_H
#define PARAMSETTING_H

#include <QDialog>

class SqliteDB;
class QLineEdit;
class QIPLineEdit;
class QCheckBox;

class ParamSetting : public QDialog
{
    Q_OBJECT
public:
    explicit ParamSetting(SqliteDB *db,QWidget *parent = 0);
private:

    QLineEdit *nameEdit;

    QCheckBox *com_enable;
    QCheckBox *show_enable;
    QCheckBox *capture_desktop;
    QCheckBox *virtual_data;
    QCheckBox *modbus_server;
    QCheckBox *server_enable;

    QLineEdit *measureIntervalEdit;
    QLineEdit *freqIntervalEdit;
    QLineEdit *waveIntervalEdit;
    QLineEdit *sampleWaitingEdit;

    QIPLineEdit *serverIpEdit;
    QLineEdit *serverWaitingEdit;

    QLineEdit *savedaysEdit;

    SqliteDB* mdb;

signals:
    void syncProjectData();
public slots:
    void ok();
    void cancel();

    void syncSlot();
};

#endif // PARAMSETTING_H
