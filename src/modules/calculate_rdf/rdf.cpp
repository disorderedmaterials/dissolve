// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/calculate_rdf/rdf.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"

CalculateRDFModule::CalculateRDFModule() : Module("CalculateRDF"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
        auto forEachA = selectA_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- Select: Site 'B'
        selectB_ = forEachA->create<SelectProcedureNode>("B");
        selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
        auto forEachB = selectB_->addForEachBranch(ProcedureNode::AnalysisContext);

        // -- -- Calculate: 'rAB'
        auto calcDistance = forEachB->create<CalculateDistanceProcedureNode>({}, selectA_, selectB_);

        // -- -- Collect1D: 'RDF'
        collectDistance_ = forEachB->create<Collect1DProcedureNode>("Histo-AB", calcDistance);

        // Process1D: @dataName
        processDistance_ = analyser_.createRootNode<Process1DProcedureNode>("RDF", collectDistance_);
        processDistance_->keywords().set("LabelValue", std::string("g(r)"));
        processDistance_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));

        auto rdfNormalisation = processDistance_->addNormalisationBranch();
        rdfNormalisation->create<OperateSitePopulationNormaliseProcedureNode>({},
                                                                              ConstNodeVector<SelectProcedureNode>({selectA_}));
        rdfNormalisation->create<OperateNumberDensityNormaliseProcedureNode>({},
                                                                             ConstNodeVector<SelectProcedureNode>({selectB_}));
        rdfNormalisation->create<OperateSphericalShellNormaliseProcedureNode>({});
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", uniqueName_);
    }

    /*
     * Keywords
     */

    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<Vec3DoubleKeyword>("Control", "DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<SpeciesSiteVectorKeyword>("Control", "SiteA",
                                            "Set the site(s) 'A' which are to represent the origin of the RDF",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "Control", "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<BoolKeyword>("Control", "ExcludeSameMolecule",
                               "Whether to exclude correlations between sites on the same molecule", excludeSameMolecule_);

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "Export", "File format and file name under which to save calculated RDF data",
                                        processDistance_->exportFileAndFormat(), "EndExport");
}
