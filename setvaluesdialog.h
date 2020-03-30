#ifndef SETVALUESDIALOG_H
#define SETVALUESDIALOG_H

#include <QDialog>

class QPushButton;
class QTabWidget;
class SetMaximumOverlappingWidget;
class SetMinimumLengthWidget;
class SetMinimumScoreWidget;
class SetTrustworthyScoreWidget;

class SetValuesDialog : public QDialog
{
    Q_OBJECT

public:
    SetValuesDialog(const int &minscore = 0, const int &minlength = 0, const int &maxoverlap = 0, const int &trustscore = 0, QWidget *parent = NULL);

    int tunedMinimumScore() const;
    int tunedMinimumLength() const;
    int tunedMaximumOverlapping() const;
    int tunedTrustworthyScore() const;

private slots:
    void setDefaultValues();

private:
    SetMinimumScoreWidget *setMinimumScoreWidget;

    SetMinimumLengthWidget *setMinimumLengthWidget;

    SetMaximumOverlappingWidget *setMaximumOverlappingWidget;

    SetTrustworthyScoreWidget *setTrustworthyScoreWidget;

    QTabWidget *tabWidget;

    QPushButton *defaultButton;
    QPushButton *cancelButton;
    QPushButton *saveButton;
};

#endif
