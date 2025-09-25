//
// Created by Ruhaan on 14/08/25.
//

#include <ios>

#include "serializator.h"

Serializable::Serializable() {
    id = reinterpret_cast<uintptr_t>(this);
}

Serializable::~Serializable() = default;
