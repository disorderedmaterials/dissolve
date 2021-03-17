// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
/* none */

// Value and Enumeration
template <class V, class E> class Venum
{
    public:
    Venum() = default;
    Venum(V value, EnumOptions<E> enumOptions) : enumOptions_(enumOptions) { value_ = value; }
    ~Venum() = default;
    Venum(const Venum &source) : enumOptions_(source.enumOptions_) { value_ = source.value_; }
    void operator=(const Venum &source)
    {
        value_ = source.value_;
        enumOptions_ = source.enumOptions_;
    }
    void operator=(const V &value) { value_ = value; }
    void operator=(const E enumeration) { enumOptions_ = enumeration; }

    /*
     * Value Data
     */
    private:
    // Value object
    V value_;

    public:
    // Return value
    V &value() { return value_; }
    const V &value() const { return value_; }
    // Set value
    void setValue(V value) { value_ = value; }

    /*
     * Enumeration
     */
    private:
    // Enumeration object
    EnumOptions<E> enumOptions_;

    public:
    // Return EnumOptionsBase
    EnumOptionsBase &baseOptions() { return enumOptions_; }
    // Return enumeration
    E enumeration() const { return enumOptions_.enumeration(); }
    // Return enumeration as string
    std::string_view enumerationAsString() const { return enumOptions_.keyword(); }
    // Set enumeration by index
    void setEnumerationByIndex(int index) { enumOptions_.setIndex(index); }
    // Set enumeration from string
    bool setEnumeration(std::string_view option) { return enumOptions_.set(option); }
};
