#ifndef MOTORMANAGE_H
#define MOTORMANAGE_H

#include <QDialog>

class SqliteDB;
class QSqlTableModel;
class QTableView;

class MotorManage : public QDialog
{
    Q_OBJECT
public:
    explicit MotorManage(SqliteDB *db,QWidget *parent = 0);
    ~MotorManage();

private:
    SqliteDB *m_db;
    QTableView* view;

    QSqlTableModel *motormodel;

signals:

public slots:
    void addmotor();
    void editmotor();
    void deletemotor();
};

#endif // MOTORMANAGE_H
