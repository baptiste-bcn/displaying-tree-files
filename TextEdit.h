//######
//## TextEdit.h
//## ===========
//## 02.06.2019: Création
//######

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QtCore>
#include <QtWidgets>

class TextEdit : public QTextEdit {
    Q_OBJECT

public:
    TextEdit(QWidget *p_Parent);

    ~TextEdit();

    void contextMenuEvent(QContextMenuEvent *p_Event);
};
#endif
