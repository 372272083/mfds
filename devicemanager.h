#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QDialog>

class SqliteDB;
class QSqlTableModel;
class QTableView;

class DeviceManager : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceManager(SqliteDB *db,QWidget *parent = 0);
    ~DeviceManager();

private:
    SqliteDB *m_db;
    QTableView* view;

    QSqlTableModel *devicemodel;

signals:

public slots:
    void adddevice();
    void editdevice();
    void deletedevice();
};

#endif // DEVICEMANAGER_H
