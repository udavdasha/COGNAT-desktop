#ifndef SEARCHGENESDIALOG_H
#define SEARCHGENESDIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QPushButton;

class SearchGenesDialog : public QDialog
{
    Q_OBJECT

public:
    SearchGenesDialog(QWidget *parent = NULL);

    QString specifiedDomain() const; // Not const &!

private slots:
    void enableSearchButton(const QString &text);

private:
    QLabel *domainLabel;

    QLineEdit *domainLineEdit;

    QPushButton *cancelButton;
    QPushButton *searchButton;
};

#endif
