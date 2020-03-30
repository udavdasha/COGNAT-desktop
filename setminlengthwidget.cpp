#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "setminlengthwidget.h"

SetMinimumLengthWidget::SetMinimumLengthWidget(const int &minlength, QWidget *parent)
    : QWidget(parent)
{
    percentLabel = new QLabel(tr("A minimum domain/COG length relative to a profile-HMM length:"), this);

    hintLabel = new QLabel(tr("Domains/COGs shorter than a minimum length will not be shown."), this);

    percentSpinBox = new QSpinBox(this);
    percentSpinBox->setRange(1, 100);
    percentSpinBox->setValue(minlength);
    percentSpinBox->setSuffix(QString(" %"));

    defaultButton = new QPushButton(tr("Set a default percent"), this);
    defaultButton->setDefault(false);
    connect(defaultButton, SIGNAL(clicked()), this, SLOT(setDefaultPercent()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(percentLabel);
    topLayout->addWidget(percentSpinBox);
    topLayout->addWidget(defaultButton);
    topLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(hintLabel);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

int SetMinimumLengthWidget::tunedMinimumLength() const
{
    return percentSpinBox->value();
}

void SetMinimumLengthWidget::setDefaultPercent()
{
    percentSpinBox->setValue(33);
}
