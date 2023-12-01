// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/intraDistance/intraDistance.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateDistance.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/select.h"

IntraDistanceModule::IntraDistanceModule() : Module(ModuleTypes::IntraDistance), analyser_(ProcedureNode::AnalysisContext)
{
    // Select: Site 'A'
    selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
    auto &forEachA = selectA_->branch()->get();

    // -- Select: Site 'B'
    selectB_ = forEachA.create<SelectProcedureNode>("B");
    selectB_->keywords().set("SameMoleculeAsSite", selectA_);
    selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
    auto &forEachB = selectB_->branch()->get();

    // -- -- Calculate: 'rAB'
    auto calcDistance = forEachB.create<CalculateDistanceProcedureNode>({}, selectA_, selectB_);

    // -- -- Collect1D: 'Distance'
    collectDistance_ = forEachB.create<Collect1DProcedureNode>("Histo-AB", calcDistance);

    // Process1D: Distance
    processDistance_ = analyser_.createRootNode<Process1DProcedureNode>("NormalisedHistogram", collectDistance_);
    processDistance_->keywords().set("LabelValue", std::string("Normalised Frequency"));
    processDistance_->keywords().set("LabelX", std::string("\\it{r}, \\symbol{Angstrom}"));
    // -- Normalisation Branch
    auto &normalisation = processDistance_->branch()->get();
    normalisation.create<OperateNormaliseProcedureNode>({});

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the distance interaction A-B.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges", "Range over which to bin quantities from the calculation.");
    keywords_.add<Vec3DoubleKeyword>("DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated RDF data",
                                        processDistance_->exportFileAndFormat(), "EndExport");
}
