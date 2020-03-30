#ifndef LOADCOLORSCHEMEEDIT_H
#define LOADCOLORSCHEMEEDIT_H

#include <QFont>
#include <QTextEdit>

class QMouseEvent;

class LoadColorSchemeEdit : public QTextEdit
{
    Q_OBJECT

public:
    LoadColorSchemeEdit(QWidget *parent = NULL);

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
