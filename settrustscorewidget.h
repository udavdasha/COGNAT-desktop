#ifndef SETTRUSTSCOREWIDGET_H
#define SETTRUSTSCOREWIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;
class QSpinBox;

class SetTrustworthyScoreWidget : public QWidget
{
    Q_OBJECT

public:
    SetTrustworthyScoreWidget(const int &trustscore = 0, QWidget *parent = NULL);

    int tunedTrustworthyScore() const;

public slots:
    void setDefaultScore();

private:
    QLabel *scoreLabel;
    QLabel *hintLabel;

    QPushButton *defaultButton;

    QSpinBox *scoreSpinBox;
};

#endif
