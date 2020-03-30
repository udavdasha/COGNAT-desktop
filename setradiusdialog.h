#ifndef SETRADIUSDIALOG_H
#define SETRADIUSDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;
class QComboBox;

class SetRadiusDialog : public QDialog
{
    Q_OBJECT

public:
    SetRadiusDialog(const int &radius = 0, const int &maxradius = 0, QWidget *parent = NULL);

    int tunedRadius() const;

private slots:
    void setDefaultRadius();

private:
    QLabel *radiusLabel;

    QPushButton *defaultButton;
    QPushButton *cancelButton;
    QPushButton *saveButton;

    QComboBox *radiusComboBox;
};

#endif
