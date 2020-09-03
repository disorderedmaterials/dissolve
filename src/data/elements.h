/*
    *** Element Data
    *** src/data/elements.h
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

#include "templates/array.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Basic Element Definition (Z, name, symbol)
class Element
{
    public:
    Element(int Z, std::string_view name, std::string_view symbol, int group);

    private:
    // Atomic number (Z)
    int Z_;
    // Element name
    std::string_view name_;
    // Element symbol
    std::string_view symbol_;
    // Group in periodic table
    int group_;

    public:
    // Return atomic number (Z)
    int Z() const;
    // Return whether the element is unknown
    bool isUnknown() const;
    // Return name of element
    std::string_view name() const;
    // Return symbol of element
    std::string_view symbol() const;
    // Return group for element
    int group() const;
};

// Basic Elements Base Class
class Elements
{
    /*
     * Element Data
     */
    private:
    // Instantiate / return array of element data
    static Element *elements();

    public:
    // Return Element with corresponding Z
    static Element &element(int Z);
    // Return Element with corresponding symbol
    static Element &element(std::string_view symbol);
    // Return pointer to Element with corresponding Z
    static Element *elementPointer(int Z);
    // Return pointer to Element with corresponding symbol
    static Element *elementPointer(std::string_view symbol);
    // Return total number of defined elements
    static constexpr int nElements() { return 119; }
    // Return name of element with specified Z
    static std::string_view name(int Z);
    // Return symbol of element with specified Z
    static std::string_view symbol(int Z);
    // Return group for element with specified Z
    static int group(int Z);

    /*
     * Helper Functions
     */
    public:
    // Create array of Lists, with array size equal to number of elements defined
    template <class T> static void createElementListArray(Array<List<T>> &listArray)
    {
        /*
         * Create the array, and set all Lists to only disown their items on destruction, rather than deleting them.
         * Need to do this otherwise each datum will be destructed twice - once from the List<T> destructor, and once
         * again from the destruction of the static array.
         */
        listArray.initialise(Elements::nElements());
        for (int n = 0; n < nElements(); ++n)
            listArray[n].setDisownOnDestruction(true);
    }
    // Create array of RefLists, with array size equal to number of elements defined
    template <class T> static void createElementRefListArray(Array<RefList<T>> &listArray)
    {
        listArray.initialise(Elements::nElements());
    }
};

// Reference to Element, for use in constructing derived/associated data classes
class ElementReference
{
    public:
    ElementReference(int Z);
    virtual ~ElementReference();

    private:
    // Referenced Element
    const Element &element_;

    public:
    // Return referenced element
    const Element &element() const;
    // Return atomic number (Z)
    int Z() const;
    // Return name of element
    std::string_view name() const;
    // Return symbol of element
    std::string_view symbol() const;
};

// Element defines
#ifndef ELEMENTS_DEFINED
#define ELEMENT_XX 0
#define ELEMENT_H 1
#define ELEMENT_HE 2
#define ELEMENT_LI 3
#define ELEMENT_BE 4
#define ELEMENT_B 5
#define ELEMENT_C 6
#define ELEMENT_N 7
#define ELEMENT_O 8
#define ELEMENT_F 9
#define ELEMENT_NE 10
#define ELEMENT_NA 11
#define ELEMENT_MG 12
#define ELEMENT_AL 13
#define ELEMENT_SI 14
#define ELEMENT_P 15
#define ELEMENT_S 16
#define ELEMENT_CL 17
#define ELEMENT_AR 18
#define ELEMENT_K 19
#define ELEMENT_CA 20
#define ELEMENT_SC 21
#define ELEMENT_TI 22
#define ELEMENT_V 23
#define ELEMENT_CR 24
#define ELEMENT_MN 25
#define ELEMENT_FE 26
#define ELEMENT_CO 27
#define ELEMENT_NI 28
#define ELEMENT_CU 29
#define ELEMENT_ZN 30
#define ELEMENT_GA 31
#define ELEMENT_GE 32
#define ELEMENT_AS 33
#define ELEMENT_SE 34
#define ELEMENT_BR 35
#define ELEMENT_KR 36
#define ELEMENT_RB 37
#define ELEMENT_SR 38
#define ELEMENT_Y 39
#define ELEMENT_ZR 40
#define ELEMENT_NB 41
#define ELEMENT_MO 42
#define ELEMENT_TC 43
#define ELEMENT_RU 44
#define ELEMENT_RH 45
#define ELEMENT_PD 46
#define ELEMENT_AG 47
#define ELEMENT_CD 48
#define ELEMENT_IN 49
#define ELEMENT_SN 50
#define ELEMENT_SB 51
#define ELEMENT_TE 52
#define ELEMENT_I 53
#define ELEMENT_XE 54
#define ELEMENT_CS 55
#define ELEMENT_BA 56
#define ELEMENT_LA 57
#define ELEMENT_CE 58
#define ELEMENT_PR 59
#define ELEMENT_ND 60
#define ELEMENT_PM 61
#define ELEMENT_SM 62
#define ELEMENT_EU 63
#define ELEMENT_GD 64
#define ELEMENT_TB 65
#define ELEMENT_DY 66
#define ELEMENT_HO 67
#define ELEMENT_ER 68
#define ELEMENT_TM 69
#define ELEMENT_YB 70
#define ELEMENT_LU 71
#define ELEMENT_HF 72
#define ELEMENT_TA 73
#define ELEMENT_W 74
#define ELEMENT_RE 75
#define ELEMENT_OS 76
#define ELEMENT_IR 77
#define ELEMENT_PT 78
#define ELEMENT_AU 79
#define ELEMENT_HG 80
#define ELEMENT_TL 81
#define ELEMENT_PB 82
#define ELEMENT_BI 83
#define ELEMENT_PO 84
#define ELEMENT_AT 85
#define ELEMENT_RN 86
#define ELEMENT_FR 87
#define ELEMENT_RA 88
#define ELEMENT_AC 89
#define ELEMENT_TH 90
#define ELEMENT_PA 91
#define ELEMENT_U 92
#define ELEMENT_NP 93
#define ELEMENT_PU 94
#define ELEMENT_AM 95
#define ELEMENT_CM 96
#define ELEMENT_BK 97
#define ELEMENT_CF 98
#define ELEMENT_ES 99
#define ELEMENT_FM 100
#define ELEMENT_MD 101
#define ELEMENT_NO 102
#define ELEMENT_LR 103
#define ELEMENT_RF 104
#define ELEMENT_DB 105
#define ELEMENT_SG 106
#define ELEMENT_BH 107
#define ELEMENT_HS 108
#define ELEMENT_MT 109
#define ELEMENT_DS 110
#define ELEMENT_RG 111
#define ELEMENT_CN 112
#define ELEMENT_NH 113
#define ELEMENT_FL 114
#define ELEMENT_MC 115
#define ELEMENT_LV 116
#define ELEMENT_TS 117
#define ELEMENT_OG 118
#endif
