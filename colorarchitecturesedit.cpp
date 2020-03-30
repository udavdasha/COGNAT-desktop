#include <QMouseEvent>
#include <QString>
#include "colorarchitecturesedit.h"

ColorArchitecturesEdit::ColorArchitecturesEdit(QWidget *parent)
    : QTextEdit(parent)
{
    exampleFont.setFamily("Courier");
    exampleFont.setStyleHint(QFont::Courier, QFont::PreferDevice);

    inputFont.setFamily("Courier New");
    inputFont.setStyleHint(QFont::Courier, QFont::PreferDevice);

    setUndoRedoEnabled(false);
}

void ColorArchitecturesEdit::setExample()
{
    setCurrentFont(exampleFont);
    setTextColor(Qt::gray);

    QString example1("PF04277");
    QString example2("\nPF00682|PF02436|PF00364\tSome comments...");
    QString example3("\nPF03977\tSome comments...");
    QString example = example1 + example2 + example3;
    append(example);

    setCurrentFont(exampleFont);
    setTextColor(Qt::gray);
}

void ColorArchitecturesEdit::setReadyToInput()
{
    setCurrentFont(inputFont);
    setTextColor(Qt::black);
}

void ColorArchitecturesEdit::mousePressEvent(QMouseEvent * /*event*/)
{
    if (textColor() == Qt::gray) {
        clear();
    }
}
