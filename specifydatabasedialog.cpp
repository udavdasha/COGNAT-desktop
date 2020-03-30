#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "specifydatabasedialog.h"

SpecifyDatabaseDialog::SpecifyDatabaseDialog(const QString &path, QWidget *parent)
    : QDialog(parent)
{
    browseLabel = new QLabel(tr("Specify a folder containing child folders named, "
                                "for example, Accession1 and Accession2.\n"
                                "Each of child folders has to contain 7 files named, "
                                "for example, respectively:\n\n"
                                "a_Accession1.gis\t\ta_Accession2.gis\n"
                                "a_Accession1.ids\t\ta_Accession2.ids\n"
                                "c_Accession1.txt\t\tc_Accession2.txt\n"
                                "d_Accession1.txt\t\td_Accession2.txt\n"
                                "g_Accession1.fasta\t\tg_Accession2.fasta\n"
                                "m_Accession1.txt\t\tm_Accession2.txt\n"
                                "p_Accession1.fasta\t\tp_Accession2.fasta"), this);

    browseLineEdit = new QLineEdit(this);
    browseLineEdit->setReadOnly(true);
    browseLineEdit->setText(path);
    connect(browseLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableSaveButton(const QString &)));

    browseButton = new QPushButton(tr("Browse..."), this);
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));

    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    saveButton = new QPushButton(tr("Save"), this);
    saveButton->setDefault(true);
    saveButton->setEnabled(!path.isEmpty());
    connect(saveButton, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *browseLayout = new QHBoxLayout;
    browseLayout->addWidget(browseLineEdit);
    browseLayout->addWidget(browseButton);

    QVBoxLayout *topLayout = new QVBoxLayout;
    topLayout->addWidget(browseLabel);
    topLayout->addLayout(browseLayout);

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
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    setFixedSize(sizeHint().width(), sizeHint().height());
    setFocus();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Specify Database"));
}

QString SpecifyDatabaseDialog::specifiedPath() const
{
    return browseLineEdit->text();
}

void SpecifyDatabaseDialog::enableSaveButton(const QString &text)
{
    saveButton->setEnabled(!text.isEmpty());
}

void SpecifyDatabaseDialog::browse()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Specify Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty()) {
        QDir dir(dirName);
        browseLineEdit->setText(dir.canonicalPath());
    }
    activateWindow();
}
