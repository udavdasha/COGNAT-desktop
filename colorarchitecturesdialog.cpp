#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFocusEvent>
#include <QPushButton>
#include "colorarchitecturesedit.h"
#include "colorarchitecturesdialog.h"

ColorArchitecturesDialog::ColorArchitecturesDialog(QWidget *parent)
    : QDialog(parent)
{
    colorArchitecturesEdit = new ColorArchitecturesEdit(this);
    colorArchitecturesEdit->setExample();
    connect(colorArchitecturesEdit, SIGNAL(textChanged()), this, SLOT(enableColorButton()));

    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    colorButton = new QPushButton(tr("Color"), this);
    colorButton->setDefault(true);
    colorButton->setEnabled(false);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
#ifdef Q_OS_MAC
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(colorButton);
#else
    bottomLayout->addWidget(colorButton);
    bottomLayout->addWidget(cancelButton);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(colorArchitecturesEdit);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    resize(sizeHint().width() * 2, parent->height() * 3 / 4);
    setMinimumSize(sizeHint().width(), sizeHint().height());
    setFocus();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Color Architectures"));
}

QString ColorArchitecturesDialog::loadedArchitectures() const
{
    return colorArchitecturesEdit->toPlainText().trimmed().toUpper();
}

void ColorArchitecturesDialog::focusInEvent(QFocusEvent * /*event*/)
{
    if (colorArchitecturesEdit->toPlainText().isEmpty()) {
        colorArchitecturesEdit->setExample();
        colorButton->setEnabled(false);
    }
}

void ColorArchitecturesDialog::enableColorButton()
{
    colorArchitecturesEdit->setReadyToInput();
    colorButton->setEnabled(!colorArchitecturesEdit->toPlainText().isEmpty());
}
