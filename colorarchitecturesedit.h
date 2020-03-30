#ifndef COLORARCHITECTURESEDIT_H
#define COLORARCHITECTURESEDIT_H

#include <QFont>
#include <QTextEdit>

class QMouseEvent;

class ColorArchitecturesEdit : public QTextEdit
{
    Q_OBJECT

public:
    ColorArchitecturesEdit(QWidget *parent = NULL);

public slots:
    void setExample();
    void setReadyToInput();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    QFont exampleFont;
    QFont inputFont;
};

#endif
