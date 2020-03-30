#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include "setmaxoverlapwidget.h"
#include "setminlengthwidget.h"
#include "setminscorewidget.h"
#include "settrustscorewidget.h"
#include "setvaluesdialog.h"

SetValuesDialog::SetValuesDialog(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, QWidget *parent)
    : QDialog(parent)
{
    setMinimumScoreWidget = new SetMinimumScoreWidget(minscore, this);

    setMinimumLengthWidget = new SetMinimumLengthWidget(minlength, this);

    setMaximumOverlappingWidget = new SetMaximumOverlappingWidget(maxoverlap, this);

    setTrustworthyScoreWidget = new SetTrustworthyScoreWidget(trustscore, this);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(setMinimumScoreWidget, tr("Minimum Score"));
    tabWidget->addTab(setMinimumLengthWidget, tr("Minimum Length"));
    tabWidget->addTab(setMaximumOverlappingWidget, tr("Maximum Overlapping"));
    tabWidget->addTab(setTrustworthyScoreWidget, tr("Trustworthy Score"));

    defaultButton = new QPushButton(tr("Restore default settings"), this);
    connect(defaultButton, SIGNAL(clicked()), this, SLOT(setDefaultValues()));

    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    saveButton = new QPushButton(tr("Save"), this);
    saveButton->setDefault(true);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(defaultButton);
#ifdef Q_OS_MAC
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(saveButton);
#else
    bottomLayout->addWidget(saveButton);
    bottomLayout->addWidget(cancelButton);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    setFixedSize(sizeHint().width(), sizeHint().height());
    setFocus();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Advanced Settings"));
}

int SetValuesDialog::tunedMinimumScore() const
{
    return setMinimumScoreWidget->tunedMinimumScore();
}

int SetValuesDialog::tunedMinimumLength() const
{
    return setMinimumLengthWidget->tunedMinimumLength();
}

int SetValuesDialog::tunedMaximumOverlapping() const
{
    return setMaximumOverlappingWidget->tunedMaximumOverlapping();
}

int SetValuesDialog::tunedTrustworthyScore() const
{
    return setTrustworthyScoreWidget->tunedTrustworthyScore();
}

void SetValuesDialog::setDefaultValues()
{
    setMinimumScoreWidget->setDefaultScore();
    setMinimumLengthWidget->setDefaultPercent();
    setMaximumOverlappingWidget->setDefaultPercent();
    setTrustworthyScoreWidget->setDefaultScore();
}
