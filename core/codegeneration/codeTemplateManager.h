//
// Created by Copilot on 01/09/25.
//

#ifndef CODETEMPLATEMANAGER_H
#define CODETEMPLATEMANAGER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QString>
#include <QHash>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

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

    QString getTemplateType(const QString& templatePath) const;

private:
    CodeTemplateManager() = default;
    ~CodeTemplateManager() = default;

    CodeTemplateManager(const CodeTemplateManager&) = delete;
    CodeTemplateManager& operator=(const CodeTemplateManager&) = delete;

    QJsonObject templatesJson;
};

#endif // CODETEMPLATEMANAGER_H
