/*
    *** Calculate RDF Module - Functions
    *** src/modules/calculate_rdf/functions.cpp
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

#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/process1d.h"

// Return Collect1DNode for A-B RDF
const Collect1DProcedureNode *CalculateRDFModule::collectDistanceNode() const { return collectDistance_; }

// Return SelectNode for site A
const SelectProcedureNode *CalculateRDFModule::selectANode() const { return selectA_; }

// Return Process1DNode result (i.e. RDF)
const Process1DProcedureNode *CalculateRDFModule::rdfResult() const
{
    if ((!processDistance_) || (!processDistance_->hasProcessedData()))
        return nullptr;

    return processDistance_;
}
