//
// Created by Ruhaan on 12/08/25.
//

#ifndef WIDGETS_H
#define WIDGETS_H

#include <QVBoxLayout>
#include <QDebug>
#include <qevent.h>
#include <QLabel>
#include <QTextEdit>

class Node;

class WidgetNode : public QWidget {
    Q_OBJECT
public:
    WidgetNode(Node* node, QWidget* parent = nullptr);

    void initUI();
    void setEditingFlag(bool flag);

    QVBoxLayout *layout{};
    QLabel *wdg_label{};
    Node* node;

};

class TextEdit: public QTextEdit {
public:
    TextEdit(const QString &text, QTextEdit* parent=nullptr) {
        setPlainText(text);
    }
    void keyPressEvent(QKeyEvent *event) override {
        QTextEdit::keyPressEvent(event);
    }
    void focusInEvent(QFocusEvent *event) override {
        auto a = dynamic_cast<WidgetNode*> (parentWidget());
        a->setEditingFlag(true);
        QTextEdit::focusInEvent(event);
    }
    void focusOutEvent(QFocusEvent *event) override {
        auto a = dynamic_cast<WidgetNode*> (parentWidget());
        a->setEditingFlag(false);
        QTextEdit::focusInEvent(event);
    }
};

#endif //WIDGETS_H
