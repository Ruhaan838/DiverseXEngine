//
// Created by Ruhaan on 29/07/25.
//

#ifndef WIDGETNODE_H
#define WIDGETNODE_H
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>


class WidgetNode : public QWidget {
    Q_OBJECT
public:
    WidgetNode(QWidget* parent = nullptr);

    void initUI();

    QVBoxLayout *layout;
    QLabel *wdg_label;

};

#endif //WIDGETNODE_H
