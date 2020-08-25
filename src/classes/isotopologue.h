/*
    *** Isotopologue Definition
    *** src/classes/isotopologue.h
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

#include "data/elements.h"
#include "templates/list.h"
#include "templates/refdatalist.h"
#include <memory>
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class Isotope;
class Species;

/*
 * Isotopologue Definition
 */
class Isotopologue : public ListItem<Isotopologue>
{
    public:
    Isotopologue();
    ~Isotopologue() = default;

    /*
     * Basic Information
     */
    private:
    // Parent Species
    Species *parent_;
    // Descriptive name
    std::string name_;

    public:
    // Set parent Species
    void setParent(Species *parent);
    // Return parent Species
    Species *parent() const;
    // Set name of Isotopologue
    void setName(std::string_view name);
    // Return name of Isotopologue
    std::string_view name() const;

    /*
     * Isotope Definition
     */
    private:
    // List of AtomType references and their assigned Isotopes
    std::vector<std::tuple<std::shared_ptr<AtomType>, Isotope *>> isotopes_;

    public:
    // Update AtomType/Isotope RefList
    void update();
    // Set AtomType/Isotope pair in list
    bool setAtomTypeIsotope(std::shared_ptr<AtomType> at, Isotope *isotope);
    // Return Isotope for specified AtomType
    Isotope *atomTypeIsotope(std::shared_ptr<AtomType> at) const;
    // Return AtomType/Isotope pairs list
    const std::vector<std::tuple<std::shared_ptr<AtomType>, Isotope *>> &isotopes() const;
    // Return nth Atom/Isotope pair
    std::tuple<std::shared_ptr<AtomType>, Isotope *> &isotope(int n);
};
