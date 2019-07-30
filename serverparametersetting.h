#ifndef SERVERPARAMETERSETTING_H
#define SERVERPARAMETERSETTING_H

#include <QDialog>

class QLineEdit;
class QIPLineEdit;
class QCheckBox;
class SqliteDB;

class ServerParameterSetting : public QDialog
{
    Q_OBJECT
public:
    ServerParameterSetting(SqliteDB *db,QDialog *parent = 0);

    QCheckBox *server_enable;
    QIPLineEdit *serverIpEdit;

private:
    SqliteDB *mdb;
signals:
    void serverStateChange(bool);
public slots:
    void ok();
    void cancel();
};

#endif // SERVERPARAMETERSETTING_H
