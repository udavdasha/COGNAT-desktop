#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QTextBrowser>

class QWheelEvent;
class RightWidget;

class TextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    TextBrowser(RightWidget *parent = NULL);

protected:
    void wheelEvent(QWheelEvent *event);

private:
    RightWidget *parent;
};

#endif
