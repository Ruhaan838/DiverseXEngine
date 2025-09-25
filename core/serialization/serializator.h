//
// Created by Ruhaan on 14/08/25.
//

#ifndef SERIALIZATOR_H
#define SERIALIZATOR_H

#include <QJsonArray>
#include <QJsonObject>
#include <string>
#include <unordered_map>

using namespace std;

class Serializable {
public:

    Serializable();
    virtual ~Serializable();
    uintptr_t id;

    virtual QJsonObject serialize() = 0;
    virtual bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) = 0;

};

#endif //SERIALIZATOR_H
