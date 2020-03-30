#ifndef COLORARCHITECTURESDIALOG_H
#define COLORARCHITECTURESDIALOG_H

#include <QDialog>
#include <QString>

class QFocusEvent;
class QPushButton;
class ColorArchitecturesEdit;

class ColorArchitecturesDialog : public QDialog
{
    Q_OBJECT

public:
    ColorArchitecturesDialog(QWidget *parent = NULL);

    QString loadedArchitectures() const; // Not const &!

protected:
    void focusInEvent(QFocusEvent *event);

private slots:
    void enableColorButton();

private:
    ColorArchitecturesEdit *colorArchitecturesEdit;

    QPushButton *cancelButton;
    QPushButton *colorButton;
};

#endif
