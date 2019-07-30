#ifndef MOTOREDIT_H
#define MOTOREDIT_H

#include <QDialog>

class SqliteDB;
class QLineEdit;
class QComboBox;
class QCheckBox;

class MotorEdit : public QDialog
{
    Q_OBJECT
public:
    explicit MotorEdit(SqliteDB *db,QWidget *parent = 0);
    ~MotorEdit();
    void setmode(int value);
private:
    SqliteDB *m_db;
    int mode;

    QLineEdit *pcodeEdit;
    QLineEdit *pnameEdit;
    QComboBox *typeCombo;
    QComboBox *beartypeCombo;

    QComboBox *rotateDeviceCombo;
    QCheckBox *isManualCB;
    QLineEdit *manualRotateEdit;

    QStringList motortypeclass;
    QStringList beartypeclass;

    bool editmotor();

signals:
    void motorEditStateChange(int,QString);
public slots:
    void ok();
    void cancel();

    void stateChangedSlot(int state);
};

#endif // MOTOREDIT_H
