#ifndef SETMINSCOREWIDGET_H
#define SETMINSCOREWIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;

class SetMinimumScoreWidget : public QWidget
{
    Q_OBJECT

public:
    SetMinimumScoreWidget(const int &minscore = 0, QWidget *parent = NULL);

    int tunedMinimumScore() const;

public slots:
    void setDefaultScore();

private:
    QLabel *scoreLabel;
    QLabel *hintLabel;

    QPushButton *defaultButton;

    QSpinBox *scoreSpinBox;
};

#endif
