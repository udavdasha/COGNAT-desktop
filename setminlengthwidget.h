#ifndef SETMINLENGTHWIDGET_H
#define SETMINLENGTHWIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;

class SetMinimumLengthWidget : public QWidget
{
    Q_OBJECT

public:
    SetMinimumLengthWidget(const int &minlength = 0, QWidget *parent = NULL);

    int tunedMinimumLength() const;

public slots:
    void setDefaultPercent();

private:
    QLabel *percentLabel;
    QLabel *hintLabel;

    QPushButton *defaultButton;

    QSpinBox *percentSpinBox;
};

#endif
