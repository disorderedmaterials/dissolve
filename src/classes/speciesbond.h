/*
    *** SpeciesBond Definition
    *** src/classes/speciesbond.h
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

#include "base/enumoptions.h"
#include "classes/speciesintra.h"
#include "templates/dynamicarrayobject.h"

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

// SpeciesBond Definition
class SpeciesBond : public SpeciesIntra, public DynamicArrayObject<SpeciesBond>
{
    public:
    SpeciesBond();
    ~SpeciesBond();

    /*
     * DynamicArrayObject Virtuals
     */
    public:
    // Clear object, ready for re-use
    void clear();

    /*
     * SpeciesAtom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_;
    // Second SpeciesAtom in interaction
    SpeciesAtom *j_;

    public:
    // Set SpeciesAtoms involved in interaction
    void setAtoms(SpeciesAtom *i, SpeciesAtom *j);
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
    // Return the 'other' SpeciesAtom
    SpeciesAtom *partner(const SpeciesAtom *i) const;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of nth SpeciesAtom
    int index(int n) const;
    // Return whether SpeciesAtoms match those specified
    bool matches(SpeciesAtom *i, SpeciesAtom *j) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Bond Type
     */
    public:
    // Bond Type enum
    enum BondType
    {
        SingleBond,
        DoubleBond,
        TripleBond,
        QuadrupleBond,
        AromaticBond,
        nBondTypes
    };
    // Convert bond type string to functional form
    static BondType bondType(const char *s);
    // Return bond type functional form text
    static const char *bondType(BondType bt);
    // Return bond order for specified bond type
    static double bondOrder(BondType bt);

    private:
    // Bond type
    BondType bondType_;

    public:
    // Set bond type
    void setBondType(BondType type);
    // Return bond type
    BondType bondType() const;
    // Return bond order for current bond type
    double bondOrder() const;

    /*
     * Interaction Parameters
     */
    public:
    // Bond functional forms
    enum BondFunction
    {
        NoForm,
        HarmonicForm,
        EPSRForm
    };
    // Return enum options for BondFunction
    static EnumOptions<BondFunction> bondFunctions();

    public:
    // Set up any necessary parameters
    void setUp();
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const;
    // Return energy for specified distance
    double energy(double distance) const;
    // Return force multiplier for specified distance
    double force(double distance) const;
};
