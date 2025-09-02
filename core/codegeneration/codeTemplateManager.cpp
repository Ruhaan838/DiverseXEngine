//
// Created by Copilot on 01/09/25.
//

#include "codeTemplateManager.h"

bool CodeTemplateManager::loadTemplatesFromFile(const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QString path = QDir::currentPath() + "/../codebase/python.json";
        file.setFileName(path);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open JSON template file:" << filePath;
            qWarning() << "Tried fallback path:" << path;
            return false;
        }
        qDebug() << "Loaded JSON template file from:" << path;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format in template file:" << file.fileName();
        return false;
    }

    templatesJson = doc.object();
    return true;
}

QString CodeTemplateManager::getInputTemplate() const {
    if (templatesJson.contains("python.Input")) {
        QJsonObject inputObj = templatesJson["python.Input"].toObject();
        if (inputObj.contains("python.input.int")) {
            return inputObj["python.input.int"].toString();
        }
    }
    return "";
}

QString CodeTemplateManager::getOutputTemplate() const {
    if (templatesJson.contains("python.Output")) {
        QJsonObject outputObj = templatesJson["python.Output"].toObject();
        if (outputObj.contains("python.output.print")) {
            return outputObj["python.output.print"].toString();
        }
    }

    qDebug() << "Output template not found. Available keys:";
    for (const QString& key : templatesJson.keys()) {
        qDebug() << " - " << key;
        if (templatesJson[key].isObject()) {
            QJsonObject obj = templatesJson[key].toObject();
            for (const QString& subKey : obj.keys()) {
                qDebug() << "   - " << subKey << ": " << obj[subKey].toString();
            }
        }
    }
    return "";
}

QString CodeTemplateManager::getFunctionTemplate(const QString& functionName) const {
    QString functionKey = "python." + functionName.toLower();
    if (templatesJson.contains(functionKey)) {
        QJsonObject funcObj = templatesJson[functionKey].toObject();

        if (funcObj.contains("def")) {
            return funcObj["def"].toString();
        }

        QString templateKey = functionKey + ".sum";
        if (funcObj.contains(templateKey)) {
            return funcObj[templateKey].toString();
        }
    }
    return "";
}

QString CodeTemplateManager::getFunctionCallTemplate(const QString& functionName) const {
    QString functionKey = "python." + functionName.toLower();
    if (templatesJson.contains(functionKey)) {
        QJsonObject funcObj = templatesJson[functionKey].toObject();

        if (funcObj.contains("call")) {
            return funcObj["call"].toString();
        }

        QString callTemplateKey = functionKey + ".sum.call";
        if (funcObj.contains(callTemplateKey)) {
            return funcObj[callTemplateKey].toString();
        }
    }
    qDebug() << "Function call template not found for:" << functionName;
    return "";
}

QString CodeTemplateManager::getTemplateType(const QString& templatePath) const {
    QStringList parts = templatePath.split('.');
    QString category = parts.size() > 1 ? parts[1] : "";

    if (templatesJson.contains("python." + category)) {
        QJsonObject categoryObj = templatesJson["python." + category].toObject();
        if (categoryObj.contains("type")) {
            return categoryObj["type"].toString();
        }
    }
    return "";
}
