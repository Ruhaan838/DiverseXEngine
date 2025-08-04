//
// Created by Ruhaan on 29/07/25.
//

#include "Widget.h"

#include <QTextEdit>

WidgetNode::WidgetNode(QWidget *parent) {
    QWidget::setParent(parent);

    initUI();

}

void WidgetNode::initUI() {
    //set layout
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    wdg_label = new QLabel("Some Title");
    layout->addWidget(wdg_label);
    layout->addWidget(new QTextEdit("foo"));
}
