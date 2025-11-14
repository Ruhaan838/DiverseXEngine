//
// Created by Ruhaan on 29/08/25.
//

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QSplitter>
#include <QTextEdit>

class Scene;

class EditorWindow : public QWidget {
    Q_OBJECT
public:
    EditorWindow(QWidget* parent);

    void setImportsText(const QString& text);
    void setFunctionsText(const QString& text);
    void setMainText(const QString& text);

    QString getImportsText() const { return importsEdit ? importsEdit->toPlainText() : QString(); }
    QString getFunctionsText() const { return functionsEdit ? functionsEdit->toPlainText() : QString(); }
    QString getMainText() const { return mainEdit ? mainEdit->toPlainText() : QString(); }

    void setScene(Scene* scene) { this->scene = scene; }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QWidget* createSectionWithTitle(const QString& title);

    QSplitter* splitter;
    QTextEdit* importsEdit;
    QTextEdit* functionsEdit;
    QTextEdit* mainEdit;

    Scene* scene = nullptr;
};

#endif //EDITORWINDOW_H
