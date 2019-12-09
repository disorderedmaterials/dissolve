/*
	*** CalculateSDF Module - Initialisation
	*** src/modules/calculate_sdf/init.cpp
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

#include "modules/calculate_sdf/sdf.h"
#include "keywords/types.h"
#include "procedure/nodes/calculatevector.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"

// Perform any necessary initialisation for the Module
void CalculateSDFModule::initialise()
{
	/*
	 * Assemble the following Procedure:
	 *
	 * Select  'A'
	 *   Site  ...
	 *   ForEach
	 *     Select  'B'
	 *       Site  ...
	 *       ExcludeSameSite  'A'
	 *       ExcludeSameMolecule  'A'
	 *       ForEach
	 *         Calculate  'vAB'
	 *           Vector  'A'  'B'
	 *           RotateIntoFrame  True
	 *         EndCalculate
	 *         Collect3D  RDF
	 *           QuantityX  'vAB'  1
	 *           QuantityY  'vAB'  2
	 *           QuantityZ  'vAB'  3
	 *           Extents  10.0  10.0  10.0
	 *           Deltas    0.5   0.5   0.5
	 *         EndCollect3D
	 *       EndForEach  'B'
	 *     EndSelect  'B'
	 *   EndForEach  'A'
	 * EndSelect  'A'
	 * Process3D  SDF
	 *   Normalisation
	 *     OperateSitePopulationNormalise
	 *       Site  'A'
	 *     EndOperateSitePopulationNormalise
	 *     OperateNumberDensityNormalise
	 *       Site  'B'
	 *     EndOperateNumberDensityNormalise
	 *     OperateSphericalShellNormalise
	 *     EndOperateSphericalShellNormalise
	 *   EndNormalisation
	 *   LabelValue  'g(r)'
	 *   LabelX  'r, Angstroms'
	 * EndProcess1D
	 */

	// Select: Site 'A'
	selectA_ = new SelectProcedureNode(NULL, true);
	selectA_->setName("A");
	SequenceProcedureNode* forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);
	analyser_.addRootSequenceNode(selectA_);

	// -- Select: Site 'B'
	selectB_ = new SelectProcedureNode();
	selectB_->setName("B");
	RefList<SelectProcedureNode> exclusions(selectA_);
	selectB_->setKeyword< RefList<SelectProcedureNode>& >("ExcludeSameSite", exclusions);
	selectB_->setKeyword< RefList<SelectProcedureNode>& >("ExcludeSameMolecule", exclusions);
	SequenceProcedureNode* forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);
	forEachA->addNode(selectB_);

	// -- -- Calculate: 'v(B->A)'
	CalculateVectorProcedureNode* calcVector = new CalculateVectorProcedureNode(selectA_, selectB_, true);
	forEachB->addNode(calcVector);

	// -- -- Collect3D: 'SDF'
	collectVector_ = new Collect3DProcedureNode(calcVector, -10.0, 10.0, 0.5, -10.0, 10.0, 0.5, -10.0, 10.0, 0.5);
	forEachB->addNode(collectVector_);

	// Process3D: @dataName
	processPosition_ = new Process3DProcedureNode(collectVector_);
	processPosition_->setName("SDF");
	processPosition_->setKeyword<CharString>("LabelValue", "\\symbol{rho}(x,y,z)");
	processPosition_->setKeyword<CharString>("LabelX", "x, \\symbol{Angstrom}");
	processPosition_->setKeyword<CharString>("LabelY", "y, \\symbol{Angstrom}");
	processPosition_->setKeyword<CharString>("LabelZ", "z, \\symbol{Angstrom}");
	analyser_.addRootSequenceNode(processPosition_);

// 	SequenceProcedureNode* rdfNormalisation = processPosition_->addNormalisationBranch();
// 	rdfNormalisation->addNode(new OperateSitePopulationNormaliseProcedureNode(selectA_));
// 	rdfNormalisation->addNode(new OperateNumberDensityNormaliseProcedureNode(selectB_));
// 	rdfNormalisation->addNode(new OperateSphericalShellNormaliseProcedureNode);
// 	analyser_.addRootSequenceNode(processPosition_);

	/*
	 * Keywords (including those exposed from the ProcedureNodes)
	 */

	// Calculation
	keywords_.add("Calculation", new Vec3DoubleKeyword(Vec3<double>(-10.0, 10.0, 0.5), Vec3<double>(-1.0e6, -1.0e6, 0.05), Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels), "RangeX", "Range along X axis", "<min> <max> <delta> (Angstroms)");
	keywords_.add("Calculation", new Vec3DoubleKeyword(Vec3<double>(-10.0, 10.0, 0.5), Vec3<double>(-1.0e6, -1.0e6, 0.05), Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels), "RangeY", "Range along Y axis", "<min> <max> <delta> (Angstroms)");
	keywords_.add("Calculation", new Vec3DoubleKeyword(Vec3<double>(-10.0, 10.0, 0.5), Vec3<double>(-1.0e6, -1.0e6, 0.05), Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels), "RangeZ", "Range along Z axis", "<min> <max> <delta> (Angstroms)");

	// Sites
	keywords_.link("Sites", selectA_->keywords().find("Site"), "SiteA", "Set the site(s) 'A' which are to represent the origin of the SDF", "<Species> <Site>");
	keywords_.link("Sites", selectB_->keywords().find("Site"), "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", "<Species> <Site>");
	keywords_.add("Sites", new BoolKeyword(false), "ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule", "<True|False>");

	// Export
	keywords_.link("Export", processPosition_->keywords().find("Save"), "Save", "Save the SDF to the specified file / format");
}

