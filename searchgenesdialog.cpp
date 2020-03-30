#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "searchgenesdialog.h"

SearchGenesDialog::SearchGenesDialog(QWidget *parent)
    : QDialog(parent)
{
    domainLabel = new QLabel(tr("An accession of a domain or COG:"), this);

    domainLineEdit = new QLineEdit(this);
    connect(domainLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableSearchButton(const QString &)));

    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    searchButton = new QPushButton(tr("Search"), this);
    searchButton->setDefault(true);
    searchButton->setEnabled(!domainLineEdit->text().isEmpty());
    connect(searchButton, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(domainLabel);
    topLayout->addWidget(domainLineEdit);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
#ifdef Q_OS_MAC
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(searchButton);
#else
    bottomLayout->addWidget(searchButton);
    bottomLayout->addWidget(cancelButton);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    setFixedSize(sizeHint().width(), sizeHint().height());
    //setFocus();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Search Genes"));
}

QString SearchGenesDialog::specifiedDomain() const
{
    return domainLineEdit->text().trimmed().toUpper();
}

void SearchGenesDialog::enableSearchButton(const QString & /*text*/)
{
    searchButton->setEnabled(!domainLineEdit->text().isEmpty());
}
