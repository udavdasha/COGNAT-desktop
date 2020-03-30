#include <QWheelEvent>
#include "rightwidget.h"
#include "textbrowser.h"

TextBrowser::TextBrowser(RightWidget *parent)
    : QTextBrowser(parent)
{
    this->parent = parent;
}

void TextBrowser::wheelEvent(QWheelEvent *event)
{
    QTextBrowser::wheelEvent(event);
    parent->saveState();
}
