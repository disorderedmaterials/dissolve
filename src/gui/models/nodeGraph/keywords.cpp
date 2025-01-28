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

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::Temperature>(const KeywordStore &keywords)
{
    auto result = asKeyword<NodeValueKeyword>(keywords, "Temperature");
    if (!result)
        return {};
    return QVariant::fromValue(result->asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::Temperature>(KeywordStore &keywords, QVariant value)
{
    return keywords.set("Temperature", value.toDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::LengthsA>(const KeywordStore &keywords)
{
    auto result = asKeyword<Vec3NodeValueKeyword>(keywords, "Lengths");
    if (!result)
        return {};
    return QVariant::fromValue(result->get(0).asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::LengthsA>(KeywordStore &keywords, QVariant value)
{
    return false;
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::LengthsB>(const KeywordStore &keywords)
{
    auto result = asKeyword<Vec3NodeValueKeyword>(keywords, "Lengths");
    if (!result)
        return {};
    return QVariant::fromValue(result->get(1).asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::LengthsB>(KeywordStore &keywords, QVariant value)
{
    return false;
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::LengthsC>(const KeywordStore &keywords)
{
    auto result = asKeyword<Vec3NodeValueKeyword>(keywords, "Lengths");
    if (!result)
        return {};
    return QVariant::fromValue(result->get(2).asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::LengthsC>(KeywordStore &keywords, QVariant value)
{
    return false;
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::AnglesA>(const KeywordStore &keywords)
{
    auto result = asKeyword<Vec3NodeValueKeyword>(keywords, "Angles");
    if (!result)
        return {};
    return QVariant::fromValue(result->get(0).asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::AnglesA>(KeywordStore &keywords, QVariant value)
{
    return false;
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::AnglesB>(const KeywordStore &keywords)
{
    auto result = asKeyword<Vec3NodeValueKeyword>(keywords, "Angles");
    if (!result)
        return {};
    return QVariant::fromValue(result->get(1).asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::AnglesB>(KeywordStore &keywords, QVariant value)
{
    return false;
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::AnglesC>(const KeywordStore &keywords)
{
    auto result = asKeyword<Vec3NodeValueKeyword>(keywords, "Angles");
    if (!result)
        return {};
    return QVariant::fromValue(result->get(2).asDouble());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::AnglesC>(KeywordStore &keywords, QVariant value)
{
    return false;
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::NonPeriodic>(const KeywordStore &keywords)
{
    auto result = asKeyword<BoolKeyword>(keywords, "NonPeriodic");
    if (!result)
        return {};
    return QVariant::fromValue(result);
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::NonPeriodic>(KeywordStore &keywords, QVariant value)
{
    return keywords.set("NonPeriodic", value.toBool());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::Species>(const KeywordStore &keywords)
{
    auto result = asKeyword<SpeciesKeyword>(keywords, "Species");
    if (!result)
        return {};
    if (!*result) // null pointer check
        return {};
    return QString::fromUtf8((*result)->name());
}

template <> bool updateKeyword<GeneratorGraphModel::PropertyIndex::Species>(KeywordStore &keywords, QVariant value)
{
    return false;
}
