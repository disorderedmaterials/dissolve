/*
	*** CalculateCoordinationNumber Module - Initialisation
	*** src/modules/calculate_cn/init.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/sum1d.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "keywords/types.h"

// Perform any necessary initialisation for the Module
void CalculateCoordinationNumberModule::initialise()
{
	/*
	 * Assemble the following Procedure:
	 *
	 * Process1D  '@RDF-target-data'
	 *   NSites  A
	 * EndProcess1D
	 * Integrate1D  'CN'
	 *   RangeA  minA  maxA
	 *   RangeB  minB  maxB
	 *   RangeC  minC  maxC
	 * EndIntegrate1D
	 */

	// Process1D - targets Collect1D in source RDF module
	process1D_ = new Process1DProcedureNode;
	process1D_->setName("HistogramNorm");
	SequenceProcedureNode* rdfNormalisation = process1D_->addNormalisationBranch();
	siteNormaliser_ = new OperateSitePopulationNormaliseProcedureNode;
	rdfNormalisation->addNode(siteNormaliser_);
	analyser_.addRootSequenceNode(process1D_);

	// Integrate1D
	sum1D_ = new Sum1DProcedureNode(process1D_);
	sum1D_->setName("CN");
	analyser_.addRootSequenceNode(sum1D_);

	keywords_.add("Target / Ranges", new ModuleKeyword<const CalculateRDFModule>("CalculateRDF"), "SourceRDF", "Source CalculateRDFModule containing the data to process");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeA"), "RangeA", "Distance range for first coordination number");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeB"), "RangeB", "Distance range for second coordination number");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeC"), "RangeC", "Distance range for third coordination number");
}
