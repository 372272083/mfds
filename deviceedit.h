#ifndef DEVICEEDIT_H
#define DEVICEEDIT_H

#include <QDialog>
#include <QMap>

class QSqlTableModel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class SqliteDB;

class DeviceEdit : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceEdit(QSqlTableModel *model,SqliteDB *db,QWidget *parent = 0);
    void setmode(int value);
    void setmodel(QList<QString> values);
private:
    SqliteDB *m_db;
    int mode;

    QLineEdit *codeEdit;
    QLineEdit *nameEdit;
    QComboBox *dmodelCombo;
    QLineEdit *typeText;
    QLineEdit *ipaddressText;
    QLineEdit *portEdit;
    QComboBox *motorCombo;
    QCheckBox *isactiveCheckBox;

    QSqlTableModel* devicemodel;

    QPushButton *papplybtn;

    //QStringList motorList;
    QStringList deviceModelList;
    QMap<QString,QString> m_map;
    QMap<QString,int> m_pipes;

    bool editdevice();
signals:

public slots:
    void modelValueChanged(const QString &text);
    void apply();
    void ok();
    void cancel();
};

#endif // DEVICEEDIT_H
