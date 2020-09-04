/*
    *** Procedure Node - Calculate Axis Angle
    *** src/procedure/nodes/calculateaxisangle.h
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

#include "classes/site.h"
#include "procedure/nodes/calculatebase.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate AxisAngle Node
class CalculateAxisAngleProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateAxisAngleProcedureNode(SelectProcedureNode *site0 = nullptr, OrientedSite::SiteAxis axis0 = OrientedSite::XAxis,
                                    SelectProcedureNode *site1 = nullptr, OrientedSite::SiteAxis axis1 = OrientedSite::XAxis);
    ~CalculateAxisAngleProcedureNode();

    /*
     * Data
     */
    private:
    // Axis to use from site 'I' (retrieved from keyword)
    int axisI_;
    // Axis to use from site 'J' (retrieved from keyword)
    int axisJ_;

    /*
     * Observable Target (implements virtuals in CalculateProcedureNodeBase)
     */
    public:
    // Return number of sites required to calculate observable
    int nSitesRequired() const;
    // Return dimensionality of calculated observable
    int dimensionality() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
};
