#ifndef PARAMSETTINGDIALOG_H
#define PARAMSETTINGDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;

class ParamSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParamSettingDialog(QDialog *parent = 0);

private:
    QLineEdit *freqshowlimitEdit;
    QComboBox *waterfalldepthlimitCB;

private slots:
    void cancel();
    void ok();
};

#endif // PARAMSETTINGDIALOG_H
