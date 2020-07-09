/*
    *** Forcefield Atom Type
    *** src/data/ffatomtype.h
    Copyright T. Youngs 2019-2020

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

#include "base/charstring.h"
#include "base/parameters.h"
#include "data/elements.h"
#include "neta/neta.h"

// Forward Declarations
class Forcefield;
class ForcefieldParameters;

// Forcefield AtomType Base Class
class ForcefieldAtomType : public ElementReference
{
    public:
    ForcefieldAtomType(const Forcefield *parent = NULL, int Z = 0, int index = -1, const char *name = NULL,
                       const char *netaDefinition = NULL, const char *description = NULL, double q = 0.0, double data0 = 0.0,
                       double data1 = 0.0, double data2 = 0.0, double data3 = 0.0);
    ForcefieldAtomType(const Forcefield *parent = NULL, int Z = 0, int index = -1, const char *name = NULL,
                       const char *netaDefinition = NULL, const char *description = NULL, double q = 0.0,
                       const char *parameterReference = NULL);
    ForcefieldAtomType(const Forcefield *parent, const ForcefieldAtomType &sourceType, const char *newTypeName,
                       const char *netaDefinition = NULL, const char *equivalentName = NULL);
    virtual ~ForcefieldAtomType();

    /*
     * Identity
     */
    private:
    // Index of atom type
    int index_;
    // Name of atom type
    CharString name_;
    // Equivalent name, if defined
    CharString equivalentName_;
    // Brief description of tyoe
    CharString description_;
    // NETA definition for the atom type
    NETADefinition neta_;

    public:
    // Return index of type
    int index() const;
    // Return name of type
    const char *name() const;
    // Return equivalent name of type
    const char *equivalentName() const;
    // Return description for type
    const char *description() const;
    // Return NETA definition for the atom type
    const NETADefinition &neta() const;

    /*
     * Parameters
     */
    private:
    // Parameters that this atom type references (if any)
    const ForcefieldParameters *parameterReference_;
    // Interatomic interaction parameters for this atom type
    InteractionParameters parameters_;

    public:
    // Return interatomic interaction parameters (referenced or otherwise)
    const InteractionParameters &parameters() const;
    // Return charge (from local parameters)
    double charge() const;
};
