#ifndef SPECIFYDATABASEDIALOG_H
#define SPECIFYDATABASEDIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QPushButton;

class SpecifyDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    SpecifyDatabaseDialog(const QString &path = QString(), QWidget *parent = NULL);

    QString specifiedPath() const; // Not const &!

private slots:
    void enableSaveButton(const QString &text);
    void browse();

private:
    QLabel *browseLabel;

    QLineEdit *browseLineEdit;

    QPushButton *browseButton;
    QPushButton *cancelButton;
    QPushButton *saveButton;
};

#endif
