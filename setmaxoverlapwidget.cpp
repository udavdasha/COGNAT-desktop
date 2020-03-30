#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "setmaxoverlapwidget.h"

SetMaximumOverlappingWidget::SetMaximumOverlappingWidget(const int &maxoverlap, QWidget *parent)
    : QWidget(parent)
{
    percentLabel = new QLabel(tr("Lower-scored domains will be shown even if they overlap with higher-scored domains by:"), this);

    hintLabel = new QLabel(tr("To hide all lower-scored overlapping domains, tune 0 %.\n"
                              "To show all lower-scored overlapping domains, tune 100 %."), this);

    percentSpinBox = new QSpinBox(this);
    percentSpinBox->setRange(0, 100);
    percentSpinBox->setValue(maxoverlap);
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

int SetMaximumOverlappingWidget::tunedMaximumOverlapping() const
{
    return percentSpinBox->value();
}

void SetMaximumOverlappingWidget::setDefaultPercent()
{
    percentSpinBox->setValue(50);
}
