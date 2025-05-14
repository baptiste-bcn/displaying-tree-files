//######
//## TextEdit.cpp
//## ============
//## 02.06.2019: Creation
//######

#include  "TextEdit.h"

//
// <<<< TextEdit::TextEdit
//
TextEdit::TextEdit(QWidget *p_Parent): QTextEdit(p_Parent) {
}

// >>>> TextEdit::TextEdit


//
// <<<< TextEdit::~TextEdit
//
TextEdit::~TextEdit() {
}

// >>>> TextEdit::~TextEdit


//
// <<<< TextEdit::contextMenuEvent
//
void TextEdit::contextMenuEvent(QContextMenuEvent *p_Event) {
    QMenu *PopupM = this->createStandardContextMenu();

    QAction *X_Suivant = NULL;

    QString S_TEXT;

    bool b_HasSelection = this->textCursor().hasSelection();

    if (b_HasSelection) {
        S_TEXT = this->textCursor().selectedText();

        PopupM->addSeparator();

        X_Suivant = PopupM->addAction(QString("Suivant"));
    }

    QAction *X_Act = PopupM->exec(p_Event->globalPos());

    if (X_Act == NULL) {
        return;
    }

    this->setFocus();

    if (X_Act == X_Suivant) {
        this->find(S_TEXT);
    }
}

// >>>> TextEdit::contextMenuEvent
