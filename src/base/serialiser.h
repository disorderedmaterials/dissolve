// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "templates/keyedVector.h"
#include "templates/orderedMap.h"
#include "templates/resolvableKeyedVector.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <toml11/toml.hpp>
#include <vector>

// The type we use for the nodes of our serialisation tree
using SerialisedValue = toml::basic_value<toml::discard_comments, dissolve::OrderedMap, std::vector>;

namespace Serialisable
{

// We need a way at compile time to detect all the types of smart
// pointers for things that can be serialised
template <typename T>
concept SerialisiblePointer = requires(T a, std::string tag, SerialisedValue target) { a->serialise(tag, target); };

template <typename T>
concept SerialisibleClass = requires(T a, std::string tag, SerialisedValue &target) { a.serialise(tag, target); };

template <typename T>
concept SerialisableCast = requires(T a) { toml::into<T>::into_toml(a); };

// template <typename T>
// concept SerialisableFromInto = requires(T a,
template <SerialisiblePointer T> void serialiseOnto(T a, std::string tag, SerialisedValue &target)
{
    a->serialise(tag, target);
}

template <SerialisibleClass T> void serialiseOnto(T a, std::string tag, SerialisedValue &target) { a.serialise(tag, target); }

template <SerialisableCast T> void serialiseOnto(T a, std::string tag, SerialisedValue &target) { target[tag] = a; }

void serialiseOnto(int a, std::string tag, SerialisedValue &target);
void serialiseOnto(double a, std::string tag, SerialisedValue &target);
void serialiseOnto(std::string a, std::string tag, SerialisedValue &target);

template <typename T>
concept Serialisible = requires(T a, std::string tag, SerialisedValue &target) { serialiseOnto(a, tag, target); };

template <Serialisible T> SerialisedValue ser(T a)
{
    SerialisedValue temp;
    serialiseOnto(a, "inner", temp);
    return temp["inner"];
}
// A helper function to add the elements of a map to a node under a name
template <typename K, typename V> void fromMap(const std::map<K, V> &map, std::string name, SerialisedValue &node)
{
    SerialisedValue result;
    for (auto &[key, value] : map)
        serialiseOnto(value, std::format("{}", key), result);
    if (!map.empty())
        node[name] = result;
}

// A helper function to add elements of a vector to a node under the named heading
template <SerialisiblePointer T> void fromVectorToTable(const std::vector<T> &vector, std::string name, SerialisedValue &node)
{
    fromVectorToTable(vector, name, node, [](const auto &item) { return item->name().data(); });
}
// A helper function to add elements of a vector to a node
template <typename T, typename Lambda> SerialisedValue fromVectorToTable(const std::vector<T> &vector, Lambda getName)
{
    SerialisedValue group;
    for (const auto &value : vector)
        value->serialise(getName(value), group);
    return group;
};
// A helper function to add elements of a KeyedVector to a node
template <typename KeyClass, typename ValueClass, typename Lambda>
SerialisedValue fromVectorToTable(const KeyedVector<KeyClass, ValueClass> &keyedVector, Lambda getName)
{
    SerialisedValue group;
    for (const auto &[key, value] : keyedVector)
        group[std::string(getName(key))] = value;
    return group;
};
// A helper function to add elements of a ResolvableKeyedVector to a node
template <typename KeyClass, typename ValueClass>
SerialisedValue fromVectorToTable(const ResolvableKeyedVector<KeyClass, ValueClass> &keyedVector)
{
    SerialisedValue group;
    for (const auto &[resolvable, value] : keyedVector)
        group[std::string(resolvable.name())] = value;
    return group;
}
template <typename KeyClass, typename ValueClass, typename Lambda>
SerialisedValue fromVectorToTable(const ResolvableKeyedVector<KeyClass, ValueClass> &keyedVector, Lambda getInner)
{
    SerialisedValue group;
    for (const auto &[resolvable, value] : keyedVector)
        group[std::string(resolvable.name())] = getInner(value);
    return group;
}
// A helper function to add elements of a vector to a node under the named heading
template <typename T, typename Lambda>
void fromVectorToTable(const std::vector<T> &vector, std::string name, SerialisedValue &node, Lambda getName)
{
    if (vector.empty())
        return;
    node[name] = fromVectorToTable(vector, getName);
};
// A helper function to add elements of a vector to a node.  This
// is more generic than fromVectorToTable and the later could be
// be implemented in terms of this function, but the two template
// types conflict with the resolution of other overloads.  While
// this could be solved with C++20 Concepts, it's probably better
// to just remove the other overloads.  That should be another
// issue before TOML is merged.
template <typename T, typename Lambda, typename Lambda2>
SerialisedValue fromVectorToMap(const std::vector<T> &vector, Lambda getName, Lambda2 getValue)
{
    SerialisedValue group;
    for (auto &value : vector)
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
    vector(vector, name, node, [](const auto &item) { return item->serialise(); });
}
// A helper function to add the elements of a vector to a node under a name
template <typename T> void vector(const std::vector<T> &vec, std::string name, SerialisedValue &node)
{
    vector(vec, name, node,
           [](const auto &item)
           {
               SerialisedValue outer;
               item.serialise("inner", outer);
               return outer["inner"];
           });
}
// A helper function to add the elements of a vector to a node under a name
template <typename T, typename Lambda> SerialisedValue vector(const std::vector<T> &vec, Lambda toSerial)
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
        if constexpr (SerialisiblePointer<V>)
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

template <typename T>
concept DeserialisableCast = requires(T a, SerialisedValue &node) { a = toml::from<T>::from_toml(node); };

// template <typename T>
// concept DeserialisableFromInto = requires(T a,

template <DeserialisibleClass T> void deserialiseOnto(T &a, const SerialisedValue &target) { a.deserialise(target); }

template <DeserialisableCast T> void deserialiseOnto(T &a, const SerialisedValue &target)
{
    a = toml::from<T>::from_toml(target);
}

void deserialiseOnto(int &a, const SerialisedValue &target);
void deserialiseOnto(long &a, const SerialisedValue &target);
void deserialiseOnto(float &a, const SerialisedValue &target);
void deserialiseOnto(double &a, const SerialisedValue &target);
void deserialiseOnto(std::string &a, const SerialisedValue &target);

template <typename T>
concept Deserialisible = requires(T &a, const SerialisedValue &target) { deserialiseOnto(a, target); };

template <Deserialisible T> T de(const SerialisedValue &target)
{
    T a;
    deserialiseOnto(a, target);
    return a;
}

template <Deserialisible T> T de_or(const SerialisedValue &target, std::string tag, T def)
{
    T a;
    if (target.contains(tag))
    {
        deserialiseOnto(a, target);
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
        if (!node.is_uninitialized())
            action(child);
        return true;
    }

    return false;
}
// Act over each value in a node table, if the key exists
template <typename Lambda> void toMap(const SerialisedValue &node, Lambda action)
{
    for (auto &[key, value] : node.as_table())
        action(key, value);
}

// Act over each value in a node table, if the key exists
template <typename Lambda> void toMap(const SerialisedValue &node, std::string key, Lambda action)
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
        result.push_back(de<T>(x));
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

// // An interface for classes that can be serialised into an input file
// class Serialisable
// {
//     public:
//     Serialisable() = default;
//     virtual ~Serialisable() = default;
//     // Express as a serialisable value
//     virtual void serialise(std::string tag, SerialisedValue &target) const = 0;
//     // Read values from a serialisable value
//     virtual void deserialise(const SerialisedValue &node) {}

//     /* Functions that hook into the toml11 library */
//     // Wrapper for deserialise that toml11 will check for
//     void from_toml(const toml::value &node) { deserialise(node); }
//     // Wrapper for serialise that toml11 will check for
//     SerialisedValue into_toml() const
//     {
//         SerialisedValue result;
//         serialise("inner", result);
//         return result["inner"];
//     }
// };
