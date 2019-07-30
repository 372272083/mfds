#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QDialog>
#include <QVector>

class QTableView;
class SqliteDB;
class QSqlTableModel;
class QPushButton;
class QModelIndex;

class DatabaseManager: public QDialog
{
    Q_OBJECT
public:
    explicit DatabaseManager(SqliteDB *db,QWidget *parent = 0);
    ~DatabaseManager();

    void setmode(int m=0);
private:
    QTableView *listView;
    SqliteDB *m_db;
    QSqlTableModel *dataModel;

    QPushButton *btnDelete;
signals:
    void enterOfflineWithInterval(QString,QString,QString);
private slots:
    void enterWaves();
    void deleteWaves();

    void slotRowDoubleClicked(const QModelIndex &);
};

#endif // DATABASEMANAGER_H
