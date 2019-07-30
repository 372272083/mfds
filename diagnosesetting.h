#ifndef DIAGNOSESETTING_H
#define DIAGNOSESETTING_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

class DiagnoseSetting : public QDialog
{
    Q_OBJECT
public:
    explicit DiagnoseSetting(QDialog *parent = 0);

private:
    QLineEdit *study_num_edit;
    QLineEdit *study_interval_edit;
    QCheckBox *study_CB;

private slots:
    void cancel();
    void ok();
    void onStudy();
};

#endif // DIAGNOSESETTING_H
