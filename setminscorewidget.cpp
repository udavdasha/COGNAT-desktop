#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "setminscorewidget.h"

SetMinimumScoreWidget::SetMinimumScoreWidget(const int &minscore, QWidget *parent)
    : QWidget(parent)
{
    scoreLabel = new QLabel(tr("A minimum domain/COG score (should be greater than 24 bits):"), this);

    hintLabel = new QLabel(tr("Domains/COGs with scores lower than a minimum score will not be shown."), this);

    scoreSpinBox = new QSpinBox(this);
    scoreSpinBox->setRange(25, 100500);
    scoreSpinBox->setValue(minscore);
    scoreSpinBox->setSuffix(tr(" bits"));

    defaultButton = new QPushButton(tr("Set a default score"), this);
    defaultButton->setDefault(false);
    connect(defaultButton, SIGNAL(clicked()), this, SLOT(setDefaultScore()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(scoreLabel);
    topLayout->addWidget(scoreSpinBox);
    topLayout->addWidget(defaultButton);
    topLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(hintLabel);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

int SetMinimumScoreWidget::tunedMinimumScore() const
{
    return scoreSpinBox->value();
}

void SetMinimumScoreWidget::setDefaultScore()
{
    scoreSpinBox->setValue(33);
}
