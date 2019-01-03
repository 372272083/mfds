#ifndef DEVICETYPEEDIT_H
#define DEVICETYPEEDIT_H

#include <QDialog>

class QSqlTableModel;
class QLineEdit;
class QTextEdit;
class QComboBox;

class DevicetypeEdit : public QDialog
{
    Q_OBJECT
public:
    explicit DevicetypeEdit(QSqlTableModel *model,QWidget *parent = 0);
    void setmode(int value);
    void setmodel(QList<QString> values);
private:
    int mode;

    QLineEdit *pmodelEdit;
    QComboBox *typeCombo;
    QLineEdit *ppipesEdit;
    QTextEdit *pdescriptionEdit;

    QSqlTableModel* model;

    QPushButton *papplybtn;

    QStringList typeclass;

    bool editdevicetype();
signals:

public slots:
    void apply();
    void ok();
    void cancel();
};

#endif // DEVICETYPEEDIT_H
