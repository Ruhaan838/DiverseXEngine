//
// Created by Copilot on 01/09/25.
//

#ifndef CODETEMPLATEMANAGER_H
#define CODETEMPLATEMANAGER_H

#include <QJsonObject>
#include <QString>

class Node;

class CodeTemplateManager {
public:
    static CodeTemplateManager& getInstance() {
        static CodeTemplateManager instance;
        return instance;
    }

    bool loadTemplatesFromFile(const QString& filePath);

    QString getInputTemplate() const;
    QString getOutputTemplate() const;
    QString getFunctionTemplate(const QString& functionName) const;
    QString getFunctionCallTemplate(const QString& functionName) const;

    QString getFunctionImport(const QString& functionName) const;

    QString getTemplateType(const QString& templatePath) const;
    QString generateNodeWrapper(Node* node, const QString& implFunctionName) const;

private:
    CodeTemplateManager() = default;
    ~CodeTemplateManager() = default;

    CodeTemplateManager(const CodeTemplateManager&) = delete;
    CodeTemplateManager& operator=(const CodeTemplateManager&) = delete;

    QJsonObject templatesJson;

    QString makeSafeIdentifier(const QString& base, int idx) const;
    QString defaultValueForSocketType(const QString& socketType) const;
};

#endif // CODETEMPLATEMANAGER_H
