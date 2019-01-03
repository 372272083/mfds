#ifndef MOTORTYPEEDIT_H
#define MOTORTYPEEDIT_H

#include <QDialog>

class QSqlTableModel;
class QLineEdit;
class QComboBox;

class MotortypeEdit : public QDialog
{
    Q_OBJECT
public:
    explicit MotortypeEdit(QSqlTableModel *model,QWidget *parent = 0);
    void setmode(int value);
    void setmodel(QList<QString> values);
private:
    int mode;

    QSqlTableModel* model;
    QLineEdit* pmodelEdit;
    QComboBox* workMCombo;
    QLineEdit* ppowerEdit;
    QLineEdit* pvoltageEdit;
    QLineEdit* pcurrentEdit;
    QLineEdit* ppolesEdit;
    QLineEdit* pcenterEdit;

    QLineEdit* pfactorEdit;
    QComboBox* pinsulateCombo;
    QLineEdit* protateEdit;

    QStringList insulateClass;
    QPushButton *papplybtn;

    bool editmotor();

signals:

public slots:
    void apply();
    void ok();
    void cancel();
};

#endif // MOTORTYPEEDIT_H
