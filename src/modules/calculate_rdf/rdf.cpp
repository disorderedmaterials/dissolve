// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/calculate_rdf/rdf.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/fileandformat.h"
#include "keywords/optionaldouble.h"
#include "keywords/range.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sum1d.h"

CalculateRDFModule::CalculateRDFModule() : Module("CalculateRDF"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A");
        auto &forEachA = selectA_->branch()->get();

        // -- Select: Site 'B'
        selectB_ = forEachA.create<SelectProcedureNode>("B");
        selectB_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
        auto &forEachB = selectB_->branch()->get();

        // -- -- Calculate: 'rAB'
        auto calcDistance = forEachB.create<CalculateDistanceProcedureNode>({}, selectA_, selectB_);

        // -- -- Collect1D: 'RDF'
        collectDistance_ = forEachB.create<Collect1DProcedureNode>("Histo-AB", calcDistance);

        // Process1D: RDF
        processDistance_ = analyser_.createRootNode<Process1DProcedureNode>("RDF", collectDistance_);
        processDistance_->keywords().set("LabelValue", std::string("g(r)"));
        processDistance_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));
        // -- Normalisation Branch
        auto &rdfNormalisation = processDistance_->branch()->get();
        rdfNormalisation.create<OperateSitePopulationNormaliseProcedureNode>({},
                                                                             ConstNodeVector<SelectProcedureNode>({selectA_}));
        rdfNormalisation.create<OperateNumberDensityNormaliseProcedureNode>({},
                                                                            ConstNodeVector<SelectProcedureNode>({selectB_}));
        rdfNormalisation.create<OperateSphericalShellNormaliseProcedureNode>({});

        // Process1D: CN
        processCN_ = analyser_.createRootNode<Process1DProcedureNode>("HistogramNorm", collectDistance_);
        processCN_->keywords().set("Instantaneous", true);
        auto &cnNormalisation = processCN_->branch()->get();
        cnNormaliser_ = cnNormalisation.create<OperateSitePopulationNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectA_}));

        // Sum1D
        sumCN_ = analyser_.createRootNode<Sum1DProcedureNode>("CN", processCN_);
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", name_);
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
    keywords_.add<BoolKeyword>("Control", "RangeAEnabled", "Whether calculation of the second coordination number is enabled",
                               sumCN_->rangeEnabled(0));
    keywords_.add<RangeKeyword>("Control", "RangeA", "Distance range for first coordination number", sumCN_->range(0));
    keywords_.add<BoolKeyword>("Control", "RangeBEnabled", "Whether calculation of the second coordination number is enabled",
                               sumCN_->rangeEnabled(1));
    keywords_.add<RangeKeyword>("Control", "RangeB", "Distance range for second coordination number", sumCN_->range(1));
    keywords_.add<BoolKeyword>("Control", "RangeCEnabled", "Whether calculation of the third coordination number is enabled",
                               sumCN_->rangeEnabled(2));
    keywords_.add<RangeKeyword>("Control", "RangeC", "Distance range for third coordination number", sumCN_->range(2));

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "Export", "File format and file name under which to save calculated RDF data",
                                        processDistance_->exportFileAndFormat(), "EndExport");
    keywords_.add<BoolKeyword>(
        "Export", "ExportInstantaneousCN",
        "Export instantaneous coordination numbers to disk (only if 'Instantaneous' option is enabled)\n",
        exportInstantaneous_);

    // Test
    keywords_.add<OptionalDoubleKeyword>(
        "Test", "TestRangeA", "Reference coordination number for range A against which calculated value should be tested",
        testRangeA_, 0.0, std::nullopt, 0.1, "Off");
    keywords_.add<OptionalDoubleKeyword>(
        "Test", "TestRangeB", "Reference coordination number for range B against which calculated value should be tested",
        testRangeB_, 0.0, std::nullopt, 0.1, "Off");
    keywords_.add<OptionalDoubleKeyword>(
        "Test", "TestRangeC", "Reference coordination number for range C against which calculated value should be tested",
        testRangeC_, 0.0, std::nullopt, 0.1, "Off");
    keywords_.add<DoubleKeyword>("Test", "TestThreshold", "Threshold difference at which test comparisons will fail",
                                 testThreshold_, 1.0e-5);
}
