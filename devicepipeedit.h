#ifndef DEVICEPIPEEDIT_H
#define DEVICEPIPEEDIT_H

#include <QDialog>

class QSqlTableModel;
class QLineEdit;
class QCheckBox;
class QComboBox;
class SqliteDB;

class DevicePipeEdit : public QDialog
{
    Q_OBJECT
public:
    explicit DevicePipeEdit(QSqlTableModel *model,SqliteDB *db,QWidget *parent = 0);
    void setmode(int value);
    void setmodel(QList<QString> values);
private:
    int mode;

    QLineEdit *pipeNameEdit;
    QCheckBox *isActiveChBox;
    QComboBox *motorCombo;

    QSqlTableModel* pipemodel;
    QStringList motorList;
    SqliteDB* m_db;

    QString dcode;
    QString pipecode;

    bool editpipe();
signals:

public slots:
    void ok();
    void cancel();
};

#endif // DEVICEPIPEEDIT_H
