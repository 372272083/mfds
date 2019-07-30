#ifndef BEARTYPEEDIT_H
#define BEARTYPEEDIT_H

#include <QDialog>

class QSqlTableModel;
class QLineEdit;
class QComboBox;

class BeartypeEdit : public QDialog
{
    Q_OBJECT
public:
    explicit BeartypeEdit(QSqlTableModel *model,QWidget *parent = 0);
    void setmode(int value);
    void setmodel(QList<QString> values);
private:
    int mode;

    QLineEdit *pmodelEdit;
    QComboBox *typeCombo;
    QLineEdit *prinEdit;
    QLineEdit *proutEdit;
    QLineEdit *ppitchEdit;
    QLineEdit *protatedEdit;
    QLineEdit *protatenEdit;
    QLineEdit *pangleEdit;

    QSqlTableModel* model;

    QPushButton *papplybtn;

    bool editbear();

signals:

public slots:
    void apply();
    void ok();
    void cancel();
};

#endif // BEARTYPEEDIT_H
