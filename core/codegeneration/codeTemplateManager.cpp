//
// Created by Copilot on 01/09/25.
//

#include "codeTemplateManager.h"
#include <QStringList>
#include <QRegularExpression>
#include "../nodes/node.h"
#include "../nodes/socket.h"

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

    for (const QString& key : templatesJson.keys()) {
        if (templatesJson[key].isObject()) {
            QJsonObject obj = templatesJson[key].toObject();
            for (const QString& subKey : obj.keys()) {}
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

QString CodeTemplateManager::makeSafeIdentifier(const QString& base, int idx) const {
    QString s = base;
    s = s.trimmed();
    if (s.isEmpty()) s = QString("in");
    s = s.toLower();
    s.replace(QRegularExpression("[^a-z0-9_]+"), "_");
    if (!s.isEmpty() && s[0].isDigit()) s.prepend("_");
    s = s + "_" + QString::number(idx);
    return s;
}

QString CodeTemplateManager::defaultValueForSocketType(const QString& socketType) const {
    QString t = socketType.toLower();
    if (t.contains("number") || t.contains("int") || t.contains("float") || t.contains("double")) return QString("0");
    if (t.contains("str") || t.contains("text") || t.contains("string")) return QString("''");
    if (t.contains("bool")) return QString("False");
    // fallback
    return QString("None");
}

QString CodeTemplateManager::generateNodeWrapper(Node* node, const QString& implFunctionName) const {
    if (!node) return QString();

    QStringList decls;
    QStringList pops;
    QStringList params;

    int seq = 1;
    for (size_t i = 0; i < node->inputs.size(); ++i) {
        auto *s = node->inputs[i];
        if (!s) continue;
        QString sockType = s->socket_type;
        if (sockType == "addsocket") continue;

        QString ident = QString("in%1").arg(seq);
        seq++;

        QString defVal = defaultValueForSocketType(sockType);
        decls << QString("%1 = %2").arg(ident, defVal);

        QString pop = QString("v = get_connection_value(node_id, %1)")
                      .arg(static_cast<int>(i));
        pop += "\n";
        pop += QString("if v is not None:\n    %1 = v").arg(ident);
        pops << pop;

        params << ident;
    }

    QString out;
    out += "# --- auto-generated wrapper start ---\n";
    out += "# Declarations\n";
    for (const QString &d : decls) out += d + "\n";
    out += "\n# Populate from connections (runtime helper: get_connection_value)\n";
    out += "# node_id must be provided by the caller\n";
    for (const QString &p : pops) out += p + "\n";
    out += "\n# Call implementation\n";

    if (params.empty()) {
        out += QString("result = %1()\n").arg(implFunctionName);
    } else if (params.size() == 1) {
        out += QString("result = %1(%2)\n").arg(implFunctionName, params[0]);
    } else if (params.size() == 2) {
        out += QString("result = %1(%2, %3)\n").arg(implFunctionName, params[0], params[1]);
    } else {

        out += QString("acc = %1(%2, %3)\n").arg(implFunctionName, params[0], params[1]);
        for (int i = 2; i < params.size(); ++i) {
            out += QString("acc = %1(acc, %2)\n").arg(implFunctionName, params[i]);
        }
        out += "result = acc\n";
    }
    out += "# --- auto-generated wrapper end ---\n";

    return out;
}
