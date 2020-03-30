#ifndef LOADCOLORSCHEMEDIALOG_H
#define LOADCOLORSCHEMEDIALOG_H

#include <QDialog>
#include <QString>

class QFocusEvent;
class QPushButton;
class LoadColorSchemeEdit;

class LoadColorSchemeDialog : public QDialog
{
    Q_OBJECT

public:
    LoadColorSchemeDialog(QWidget *parent = NULL);

    QString loadedScheme() const; // Not const &!

protected:
    void focusInEvent(QFocusEvent *event);

private slots:
    void enableSaveButton();

private:
    LoadColorSchemeEdit *loadColorSchemeEdit;

    QPushButton *cancelButton;
    QPushButton *saveButton;
};

#endif
