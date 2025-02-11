// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/nodeGraph/keywords.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"
#include "keywords/species.h"
#include "keywords/vec3NodeValue.h"

// This returns a pointer because the C++ committee still cnn't get their act together around optional references
template <typename T>
auto asKeyword(const KeywordStore &keywords, std::string_view name)
    -> decltype(&((T *)keywords.find(name).value().first)->data())
{
    auto temp = keywords.find(name);
    // Check that we found a keyword
    if (!temp)
        return nullptr;
    auto precise = dynamic_cast<T *>(temp.value().first);
    if (!precise)
        return nullptr;
    return &precise->data();
}
