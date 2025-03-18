// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/nodeNumber.h"
#include <string>

NodeNumber::NodeNumber(const std::variant<int, double> &value) : value_(value) {}

NodeNumber::NodeNumber(int value) : value_(value) {}

NodeNumber::NodeNumber(double value) : value_(value) {}

NodeNumber &NodeNumber::operator=(const NodeNumber &other)
{
    value_ = other.value_;
    return *this;
}

struct AddNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a + b}; }
};
NodeNumber NodeNumber::operator+(const NodeNumber &other) const
{
    return std::visit(AddNodeNumberVisitor{}, value_, other.value_);
}

struct AddAssignmentNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a += b}; }
};
NodeNumber &NodeNumber::operator+=(const NodeNumber &other)
{
    set(std::visit(AddAssignmentNodeNumberVisitor{}, value_, other.value_));
    return *this;
}

struct SubtractNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a - b}; }
};
NodeNumber NodeNumber::operator-(const NodeNumber &other) const
{
    return std::visit(SubtractNodeNumberVisitor{}, value_, other.value_);
}

struct SubtractAssignmentNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a -= b}; }
};
NodeNumber &NodeNumber::operator-=(const NodeNumber &other)
{
    set(std::visit(SubtractAssignmentNodeNumberVisitor{}, value_, other.value_));
    return *this;
}

struct MultiplyNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a * b}; }
};
NodeNumber NodeNumber::operator*(const NodeNumber &other) const
{
    return std::visit(MultiplyNodeNumberVisitor{}, value_, other.value_);
}

struct MultiplyAssignmentNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a *= b}; }
};
NodeNumber &NodeNumber::operator*=(const NodeNumber &other)
{
    set(std::visit(MultiplyAssignmentNodeNumberVisitor{}, value_, other.value_));
    return *this;
}

struct DivisionNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a / b}; }
};
NodeNumber NodeNumber::operator/(const NodeNumber &other) const
{
    return std::visit(DivisionNodeNumberVisitor{}, value_, other.value_);
}

struct DivisionAssignmentNodeNumberVisitor
{
    template <class T1, class T2> auto operator()(T1 a, T2 b) const -> NodeNumber { return {a /= b}; }
};
NodeNumber &NodeNumber::operator/=(const NodeNumber &other)
{
    set(std::visit(DivisionAssignmentNodeNumberVisitor{}, value_, other.value_));
    return *this;
}

// NodeNumber &NodeNumber::operator++();
//
// NodeNumber &NodeNumber::operator--();

bool NodeNumber::operator==(const NodeNumber &other) const { return value_ == other.value_; }

bool NodeNumber::operator!=(const NodeNumber &other) const { return value_ != other.value_; }

/*
 * Data
 */

// Set from other NodeNumber
void NodeNumber::set(const NodeNumber &other) { value_ = other.value_; }

// Return whether the contained value is an integer
bool NodeNumber::isInteger() const { return std::holds_alternative<int>(value_); }

// Return whether the contained value is a double
bool NodeNumber::isDouble() const { return std::holds_alternative<double>(value_); }

// Return contained value as integer
int NodeNumber::asInteger() const
{
    if (std::holds_alternative<int>(value_))
        return std::get<int>(value_);
    else
        return int(std::get<double>(value_));
}

// Return contained value as double
double NodeNumber::asDouble() const
{
    if (std::holds_alternative<int>(value_))
        return double(std::get<int>(value_));
    else
        return std::get<double>(value_);
}

// Return value represented as a string
std::string NodeNumber::asString(bool addQuotesIfRequired) const
{
    return std::visit([](auto &&arg) -> std::string { return std::format("{}", arg); }, value_);
}

// Express as a serialisable value
SerialisedValue NodeNumber::serialise() const
{
    if (std::holds_alternative<int>(value_))
        return std::get<int>(value_);
    else
        return std::get<double>(value_);
}

// Read values from a serialisable value
void NodeNumber::deserialise(const SerialisedValue &node)
{
    toml::visit(
        [this](auto &arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, toml::integer>)
            {
                set((int)arg);
            }
            else if constexpr (std::is_same_v<T, toml::floating>)
            {
                set((double)arg);
            }
        },
        node);
}
