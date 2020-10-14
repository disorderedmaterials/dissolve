// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

// Forward Declarations
/* none */

// Value and Enumeration
template <class V, class E> class Venum
{
    public:
    Venum(V value = V(), EnumOptions<E> enumOptions = EnumOptions<E>()) : enumOptions_(enumOptions) { value_ = value; }
    ~Venum() {}
    Venum(const Venum &source) : enumOptions_(source.enumOptions_) { value_ = source.value_; }
    // Assignment Operators
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
    // Return value (const)
    const V &constValue() const { return value_; }
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
    // Return EnumOptionsBase (const)
    const EnumOptionsBase &constBaseOptions() const { return enumOptions_; }
    // Return enumeration
    E enumeration() const { return enumOptions_.enumeration(); }
    // Return enumeration as string
    std::string_view enumerationAsString() const { return enumOptions_.currentOption().keyword(); }
    // Set enumeration by index
    bool setEnumerationByIndex(int index) { return enumOptions_.setCurrentOptionIndex(index); }
    // Set enumeration from string
    bool setEnumeration(std::string_view option)
    {
        if (enumOptions_.isValid(option))
        {
            enumOptions_.setCurrentOption(option);
            return true;
        }

        return false;
    }
};
