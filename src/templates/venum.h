/*
    *** Value and Enum (venum)
    *** src/templates/venum.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
