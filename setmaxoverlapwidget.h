#ifndef SETMAXOVERLAPWIDGET_H
#define SETMAXOVERLAPWIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;

class SetMaximumOverlappingWidget : public QWidget
{
    Q_OBJECT

public:
    SetMaximumOverlappingWidget(const int &maxoverlap = 0, QWidget *parent = NULL);

    int tunedMaximumOverlapping() const;

public slots:
    void setDefaultPercent();

private:
    QLabel *percentLabel;
    QLabel *hintLabel;

    QPushButton *defaultButton;

    QSpinBox *percentSpinBox;
};

#endif
