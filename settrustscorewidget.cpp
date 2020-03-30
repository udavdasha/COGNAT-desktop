#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "settrustscorewidget.h"

SetTrustworthyScoreWidget::SetTrustworthyScoreWidget(const int &trustscore, QWidget *parent)
    : QWidget(parent)
{
    scoreLabel = new QLabel(tr("A 'trustworthy' domain/COG score (should be not less than a minimum score):"), this);

    hintLabel = new QLabel(tr("Regardless of an overlapping region size, domains/COGs with 'trustworthy' scores will be shown."), this);

    scoreSpinBox = new QSpinBox(this);
    scoreSpinBox->setRange(25, 100500);
    scoreSpinBox->setValue(trustscore);
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

int SetTrustworthyScoreWidget::tunedTrustworthyScore() const
{
    return scoreSpinBox->value();
}

void SetTrustworthyScoreWidget::setDefaultScore()
{
    scoreSpinBox->setValue(250);
}
