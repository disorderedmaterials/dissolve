/*
	*** CalculateCN Module - Initialisation
	*** src/modules/calculate_cn/init.cpp
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

#include "keywords/types.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/sum1d.h"

// Perform any necessary initialisation for the Module
void CalculateCNModule::initialise()
{
	/*
	 * Assemble the following Procedure:
	 *
	 * Process1D  '@RDF-target-data'
	 *   NSites  A
	 * EndProcess1D
	 * Sum1D  'CN'
	 *   RangeA  minA  maxA
	 *   RangeB  minB  maxB
	 *   RangeC  minC  maxC
	 * EndSum1D
	 */

	// Process1D - targets Collect1D in source RDF module
	process1D_ = new Process1DProcedureNode;
	process1D_->setName("HistogramNorm");
	SequenceProcedureNode *rdfNormalisation = process1D_->addNormalisationBranch();
	siteNormaliser_ = new OperateSitePopulationNormaliseProcedureNode;
	rdfNormalisation->addNode(siteNormaliser_);
	analyser_.addRootSequenceNode(process1D_);

	// Sum1D
	sum1D_ = new Sum1DProcedureNode(process1D_);
	sum1D_->setName("CN");
	analyser_.addRootSequenceNode(sum1D_);

	// Target / Ranges
	keywords_.add("Target / Ranges", new ModuleKeyword<const CalculateRDFModule>("CalculateRDF"), "SourceRDF", "Source CalculateRDFModule containing the data to process");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeA"), "RangeA", "Distance range for first coordination number");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeBEnabled"), "RangeBEnabled", "Whether calculation of the second coordination number is enabled");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeB"), "RangeB", "Distance range for second coordination number");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeCEnabled"), "RangeCEnabled", "Whether calculation of the third coordination number is enabled");
	keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeC"), "RangeC", "Distance range for third coordination number");

	// Test
	keywords_.add("Test", new DoubleKeyword(0.0), "TestRangeA", "Reference coordination number for range A against which calculated value should be tested");
	keywords_.add("Test", new DoubleKeyword(0.0), "TestRangeB", "Reference coordination number for range B against which calculated value should be tested");
	keywords_.add("Test", new DoubleKeyword(0.0), "TestRangeC", "Reference coordination number for range C against which calculated value should be tested");
	keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold difference at which test comparisons will fail");
}
