// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "templates/resolvableKeyedVector.h"
#include <algorithm>
#include <iterator>
#include <map>

namespace Serialisable
{

// We need a way at compile time to detect all the types of smart
// pointers for things that can be serialised
template <typename T>
concept SerialisablePointer = requires(T a, std::string tag, SerialisedValue target) { a->serialise(tag, target); };

template <typename T>
concept SerialisableClass = requires(T a, std::string tag, SerialisedValue &target) { a.serialise(tag, target); };

template <SerialisablePointer T> void serialiseOnto(T &a, std::string tag, SerialisedValue &target)
{
    a->serialise(tag, target);
}

template <SerialisableClass T> void serialiseOnto(T &a, std::string tag, SerialisedValue &target) { a.serialise(tag, target); }

template <typename T>
concept Serialisable = requires(const T a, std::string tag, SerialisedValue &target) { serialiseOnto(a, tag, target); };

template <Serialisable T> SerialisedValue ser(const T &a)
{
    SerialisedValue temp;
    serialiseOnto(a, "inner", temp);
    return temp["inner"];
}
// A helper function to add the elements of a map to a node under a name
template <typename K, typename V> void map(const std::map<K, V> &map, std::string name, SerialisedValue &node)
{
    if (map.empty())
        return;

    SerialisedValue result;
    for (auto &[key, value] : map)
        serialiseOnto(value, std::format("{}", key), result);
    node[name] = result;
}

// A helper function to add elements of a vector to a node under the named heading
template <SerialisablePointer T> void fromVectorToTable(const std::vector<T> &vec, std::string name, SerialisedValue &node)
{
    fromVectorToTable(vec, name, node, [](const auto &item) { return item->name().data(); });
}
// A helper function to add elements of a ResolvableKeyedVector to a node
template <typename KeyClass, typename ValueClass>
SerialisedValue vector(const ResolvableKeyedVector<KeyClass, ValueClass> &keyedVector)
{
    SerialisedValue group;
    for (const auto &[resolvable, value] : keyedVector)
        group[std::string(resolvable.name())] = value;
    return group;
}
template <typename KeyClass, typename ValueClass, typename Lambda>
SerialisedValue vector(const ResolvableKeyedVector<KeyClass, ValueClass> &keyedVector, Lambda getInner)
{
    SerialisedValue group;
    for (const auto &[resolvable, value] : keyedVector)
        group[std::string(resolvable.name())] = getInner(value);
    return group;
}
// A helper function to add elements of a vector to a node under the named heading
template <typename T, typename Lambda>
void fromVectorToTable(const std::vector<T> &vec, std::string name, SerialisedValue &node, Lambda getName)
{
    if (vec.empty())
        return;
    SerialisedValue group;
    for (const auto &value : vec)
        serialiseOnto(value, getName(value), group);
    node[name] = group;
};
// A helper function to add elements of a vector to a node.  This
// is more generic than fromVectorToTable and the later could be
// be implemented in terms of this function, but the two template
// types conflict with the resolution of other overloads.  While
// this could be solved with C++20 Concepts, it's probably better
// to just remove the other overloads.  That should be another
// issue before TOML is merged.
template <typename T, typename Lambda, typename Lambda2>
SerialisedValue fromVectorToMap(const std::vector<T> &vec, Lambda getName, Lambda2 getValue)
    requires requires(T x) { std::is_same<std::string, decltype(getName(x))>::value; }
{
    SerialisedValue group;
    for (auto &value : vec)
        group[getName(value)] = getValue(value);
    return group;
};
// A helper function to add the elements of a vector to a node under a name
template <typename T> void vector(const std::vector<std::unique_ptr<T>> &vec, std::string name, SerialisedValue &node)
{
    vector(vec, name, node,
           [](const auto &item)
           {
               SerialisedValue outer;
               item->serialise("inner", outer);
               return outer["inner"];
           });
}
// A helper function to add the elements of a vector to a node under a name
template <typename T> void vector(const std::vector<std::shared_ptr<T>> &vec, std::string name, SerialisedValue &node)
{
    vector(vec, name, node, [](const auto &item) { return item->serialise(); });
}
// A helper function to add the elements of a vector to a node under a name
template <typename T> void vector(const std::vector<T> &vec, std::string name, SerialisedValue &node)
{
    vector(vec, name, node, [](const auto &item) { return ser(item); });
}
// A helper function to add the elements of a vector to a node under a name
template <typename T, typename Lambda>
SerialisedValue vector(const std::vector<T> &vec, Lambda toSerial)
    requires(requires(T a) { std::is_same_v<decltype(toSerial(a)), SerialisedValue>; })
{
    SerialisedValue result = SerialisedValue::array_type{};
    std::transform(vec.begin(), vec.end(), std::back_inserter(result), toSerial);
    return result;
}
// A helper function to add the elements of a vector to a node under a name
template <typename T, typename Lambda>
void vector(const std::vector<T> &vec, std::string name, SerialisedValue &node, Lambda toSerial)
{
    if (vec.empty())
        return;
    node[name] = vector(vec, toSerial);
}
// A helper function to add the elements of a ranged object to a node under a name
template <std::ranges::input_range Range, typename Lambda> SerialisedValue fromRange(const Range &range, Lambda toSerial)
{
    SerialisedValue result = SerialisedValue::array_type{};
    std::ranges::transform(range, std::back_inserter(result), toSerial);
    return result;
}
// A helper function to add the elements of a map to a node under a name
// Only add values that pass the test lambda
template <typename K, typename V, typename Lambda>
void fromMap(const std::map<K, V> &map, std::string name, SerialisedValue &node, Lambda filter)
{
    SerialisedValue result;
    bool changed = false;
    for (auto &[key, value] : map)
    {
        if (!filter(key, value))
            continue;
        changed = true;
        if constexpr (SerialisablePointer<V>)
            value->serialise(std::string(key), result);
        else
            // We use the direct value (with casting) instead of
            // value.serialise() to handle the case where the value
            // is a raw type (e.g. int)
            result[std::string(key)] = value;
    }
    if (changed)
        node[name] = result;
}

} // namespace Serialisable

namespace Deserialisable
{
template <typename T>
concept DeserialisibleClass = requires(T a, SerialisedValue &node) { a.deserialise(node); };

template <DeserialisibleClass T> void deserialiseOnto(T &a, const SerialisedValue &target) { a.deserialise(target); }

template <typename T>
concept Deserialisible = requires(T &a, const SerialisedValue &target) { deserialiseOnto(a, target); };

template <Deserialisible T> T deser(const SerialisedValue &target)
{
    T a;
    deserialiseOnto(a, target);
    return a;
}

template <Deserialisible T> T deser_or(const SerialisedValue &target, std::string tag, T def)
{
    T a;
    if (target.contains(tag))
    {
        deserialiseOnto(a, target.at(tag));
        return a;
    }
    else
        return def;
}

// Perform an action on a child node in a table if the node exists.
// This cuts out quite a bit of boilerplate.
template <typename Lambda> bool optionalOn(const SerialisedValue &node, std::string name, Lambda action)
{
    if (node.contains(name))
    {
        auto child = toml::find(node, name);
        if (!child.is_empty())
            action(child);
        return true;
    }

    return false;
}
// Act over each value in a node table, if the key exists
template <typename Lambda> void map(const SerialisedValue &node, Lambda action)
{
    for (auto &[key, value] : node.as_table())
        action(key, value);
}

// Act over each value in a node table, if the key exists
template <typename Lambda> void map(const SerialisedValue &node, std::string key, Lambda action)
{
    if (!node.contains(key))
        return;

    for (auto &[subKey, value] : toml::find<SerialisedValue::table_type>(node, key))
        action(subKey, value);
}

// Act over each value in a node array
template <typename Lambda> void vector(const SerialisedValue &node, Lambda action)
{
    for (auto &item : node.as_array())
        action(item);
}

template <Deserialisible T> std::vector<T> vector(const SerialisedValue &node)
{
    std::vector<T> result;
    for (auto &x : node.as_array())
        result.push_back(deser<T>(x));
    return result;
}

// Act over each value in a node table, if the key exists
template <typename Lambda> void vector(const SerialisedValue &node, std::string key, Lambda action)
{
    if (!node.contains(key))
        return;

    vector(node.at(key), action);
}

// Place the named value into the supplied object, but only if it exists
template <typename T, typename U> bool getIfPresent(const SerialisedValue &node, std::string name, U &destination)
{
    if (!node.contains(name))
        return false;
    destination = toml::find<T>(node, name);
    return true;
}
} // namespace Deserialisable
