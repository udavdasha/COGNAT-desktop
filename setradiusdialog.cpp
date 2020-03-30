#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include "setradiusdialog.h"

SetRadiusDialog::SetRadiusDialog(const int &radius, const int &maxradius, QWidget *parent)
    : QDialog(parent)
{
    radiusLabel = new QLabel(tr("A number of genes in a neighborhood:"), this);

    radiusComboBox = new QComboBox(this);
    for (int i = 1; i <= maxradius; ++i) {
        if (i * 2 + 1 < 10) {
            radiusComboBox->addItem(tr("%1 genes").arg(i * 2 + 1, 3));
        } else {
            radiusComboBox->addItem(tr("%1 genes").arg(i * 2 + 1));
        }
    }
    radiusComboBox->setCurrentIndex(radius - 1);
    radiusComboBox->setFocusPolicy(Qt::NoFocus);

    defaultButton = new QPushButton(tr("Set a default number"), this);
    connect(defaultButton, SIGNAL(clicked()), this, SLOT(setDefaultRadius()));

    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    saveButton = new QPushButton(tr("Save"), this);
    saveButton->setDefault(true);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(radiusLabel);
    topLayout->addWidget(radiusComboBox);

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
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    setFixedSize(sizeHint().width(), sizeHint().height());
    setFocus();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Set Neighborhood"));
}

int SetRadiusDialog::tunedRadius() const
{
    return radiusComboBox->currentIndex() + 1;
}

void SetRadiusDialog::setDefaultRadius()
{
    radiusComboBox->setCurrentIndex(3);
}
