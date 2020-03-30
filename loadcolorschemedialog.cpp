#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFocusEvent>
#include <QPushButton>
#include "loadcolorschemeedit.h"
#include "loadcolorschemedialog.h"

LoadColorSchemeDialog::LoadColorSchemeDialog(QWidget *parent)
    : QDialog(parent)
{
    loadColorSchemeEdit = new LoadColorSchemeEdit(this);
    loadColorSchemeEdit->setExample();
    connect(loadColorSchemeEdit, SIGNAL(textChanged()), this, SLOT(enableSaveButton()));

    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    saveButton = new QPushButton(tr("Save"), this);
    saveButton->setDefault(true);
    saveButton->setEnabled(false);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
#ifdef Q_OS_MAC
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(saveButton);
#else
    bottomLayout->addWidget(saveButton);
    bottomLayout->addWidget(cancelButton);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(loadColorSchemeEdit);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    resize(sizeHint().width() * 2, parent->height() * 3 / 4);
    setMinimumSize(sizeHint().width(), sizeHint().height());
    setFocus();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Load Color Scheme"));
}

QString LoadColorSchemeDialog::loadedScheme() const
{
    return loadColorSchemeEdit->toPlainText().trimmed().toUpper();
}

void LoadColorSchemeDialog::focusInEvent(QFocusEvent * /*event*/)
{
    if (loadColorSchemeEdit->toPlainText().isEmpty()) {
        loadColorSchemeEdit->setExample();
        saveButton->setEnabled(false);
    }
}

void LoadColorSchemeDialog::enableSaveButton()
{
    loadColorSchemeEdit->setReadyToInput();
    saveButton->setEnabled(!loadColorSchemeEdit->toPlainText().isEmpty());
}
