#include <QMouseEvent>
#include <QString>
#include "loadcolorschemeedit.h"

LoadColorSchemeEdit::LoadColorSchemeEdit(QWidget *parent)
    : QTextEdit(parent)
{
    exampleFont.setFamily("Courier");
    exampleFont.setStyleHint(QFont::Courier, QFont::PreferDevice);

    inputFont.setFamily("Courier New");
    inputFont.setStyleHint(QFont::Courier, QFont::PreferDevice);

    setUndoRedoEnabled(false);
}

void LoadColorSchemeEdit::setExample()
{
    setCurrentFont(exampleFont);
    setTextColor(Qt::gray);

    QString example1("#c83737\tPF04277");
    QString example2("\n#c83737\tCOG3630");
    QString example3("\n#008080\tPF00682|PF02436|PF00364");
    QString example4("\n#008080\tCOG5016\tSome comments...");
    QString example5("\nPF03977\t#ff9955\tSome comments...");
    QString example6("\nCOG1883\t#ff9955\tSome comments...");
    QString example = example1 + example2 + example3 + example4 + example5 + example6;
    append(example);

    setCurrentFont(exampleFont);
    setTextColor(Qt::gray);
}

void LoadColorSchemeEdit::setReadyToInput()
{
    setCurrentFont(inputFont);
    setTextColor(Qt::black);
}

void LoadColorSchemeEdit::mousePressEvent(QMouseEvent * /*event*/)
{
    if (textColor() == Qt::gray) {
        clear();
    }
}
