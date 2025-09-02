//
// Created by Ruhaan on 29/08/25.
//

#include "editorWindow.h"
#include "../../core/scene/nodescene.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QKeyEvent>
#include <QDebug>

EditorWindow::EditorWindow(QWidget* parent) : QWidget(parent) {
    setFixedWidth(420);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);

    splitter = new QSplitter(Qt::Vertical, this);
    splitter->setHandleWidth(1);

    QWidget* importsSection = createSectionWithTitle("Imports");
    QWidget* functionsSection = createSectionWithTitle("Functions");
    QWidget* mainSection = createSectionWithTitle("Main");

    splitter->addWidget(importsSection);
    splitter->addWidget(functionsSection);
    splitter->addWidget(mainSection);

    QList<int> sizes;
    sizes << 10 << 70 << 10;
    splitter->setSizes(sizes);

    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    // Install event filter on all text edits
    if (importsEdit) importsEdit->installEventFilter(this);
    if (functionsEdit) functionsEdit->installEventFilter(this);
    if (mainEdit) mainEdit->installEventFilter(this);
}

QWidget* EditorWindow::createSectionWithTitle(const QString& title) {
    QWidget* container = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);

    QLabel* titleLabel = new QLabel(title, container);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #e0e0e0;");

    QTextEdit* textEdit = new QTextEdit(container);

    textEdit->setReadOnly(true);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    textEdit->setStyleSheet(
        "QTextEdit {"
        "  border-radius: 6px;"
        "  background-color: #2a2a2a;"
        "  border: 1px solid #444444;"
        "  padding: 4px;"
        "  color: #e0e0e0;"
        "}"
        "QScrollBar {"
        "  background-color: #2a2a2a;"
        "}"
        "QScrollBar::handle {"
        "  background-color: #555555;"
        "  border-radius: 4px;"
        "}"
    );

    if (title == "Imports") {
        importsEdit = textEdit;
    } else if (title == "Functions") {
        functionsEdit = textEdit;
    } else if (title == "Main") {
        mainEdit = textEdit;
    }

    if (textEdit) textEdit->installEventFilter(this);

    layout->addWidget(titleLabel);
    layout->addWidget(textEdit);

    return container;
}

bool EditorWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if ((keyEvent->modifiers() & Qt::MetaModifier) && keyEvent->key() == Qt::Key_Return) {

            if (scene) {
                scene->updateEditorCode();
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void EditorWindow::setImportsText(const QString& text) {
    if (importsEdit) {
        importsEdit->setText(text);
    }
}

void EditorWindow::setFunctionsText(const QString& text) {
    if (functionsEdit) {
        functionsEdit->setText(text);
    }
}

void EditorWindow::setMainText(const QString& text) {
    if (mainEdit) {
        mainEdit->setText(text);
    }
}
