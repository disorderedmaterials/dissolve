/*
    *** MasterIntra Definition
    *** src/classes/masterintra.h
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

#include "classes/speciesintra.h"
#include "templates/array2d.h"
#include "templates/listitem.h"

// Forward Declarations
/* none */

/*
 * MasterIntra Definition
 */
class MasterIntra : public SpeciesIntra, public ListItem<MasterIntra>
{
    public:
    MasterIntra();
    ~MasterIntra();

    /*
     * Basic Data
     */
    protected:
    // Name for interaction
    std::string name_;
    // Type of this interaction
    SpeciesIntra::InteractionType type_;

    public:
    // Set name for interaction (if relevant)
    void setName(std::string_view name);
    // Return name for interaction
    std::string_view name() const;
    // Set type of interaction
    void setType(SpeciesIntra::InteractionType type);

    /*
     * SpeciesIntra Virtuals
     */
    public:
    // Set up any necessary parameters
    void setUp();
    // Calculate and return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const;
};
