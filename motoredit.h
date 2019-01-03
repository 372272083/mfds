#ifndef MOTOREDIT_H
#define MOTOREDIT_H

#include <QDialog>
class SqliteDB;
class QLineEdit;
class QComboBox;
class QSqlTableModel;

class MotorEdit : public QDialog
{
    Q_OBJECT
public:
    explicit MotorEdit(QSqlTableModel *model,SqliteDB *db,QWidget *parent = 0);
    ~MotorEdit();
    void setmode(int value);
    void setmodel(QList<QString> values);
private:
    SqliteDB *m_db;
    int mode;

    QSqlTableModel* motormodel;

    QLineEdit *pcodeEdit;
    QLineEdit *pnameEdit;
    QComboBox *typeCombo;
    QComboBox *beartypeCombo;

    QPushButton *papplybtn;

    QStringList motortypeclass;
    QStringList beartypeclass;

    bool editmotor();

signals:

public slots:
    void apply();
    void ok();
    void cancel();
};

#endif // MOTOREDIT_H
