// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/calculate_cn/cn.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/module.h"
#include "keywords/optionaldouble.h"
#include "keywords/range.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/sum1d.h"

CalculateCNModule::CalculateCNModule() : Module("CalculateCN"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Process1D - targets Collect1D in source RDF module
        process1D_ = analyser_.createRootNode<Process1DProcedureNode>("HistogramNorm");
        process1D_->keywords().set("CurrentDataOnly", true);
        auto rdfNormalisation = process1D_->addNormalisationBranch();
        siteNormaliser_ = rdfNormalisation->create<OperateSitePopulationNormaliseProcedureNode>(
            {}, std::vector<std::shared_ptr<const SelectProcedureNode>>());

        // Sum1D
        sum1D_ = analyser_.createRootNode<Sum1DProcedureNode>("CN", process1D_);
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", uniqueName_);
    }

    /*
     * Keywords
     */

    // Target
    keywords_.addTarget<ModuleKeyword<CalculateRDFModule>>(
        "SourceRDF", "Source CalculateRDFModule containing the data to process", sourceRDF_, "CalculateRDF");

    // Ranges
    keywords_.add<RangeKeyword>("Ranges", "RangeA", "Distance range for first coordination number", sum1D_->range(0));
    keywords_.add<BoolKeyword>("Ranges", "RangeBEnabled", "Whether calculation of the second coordination number is enabled",
                               sum1D_->rangeEnabled(1));
    keywords_.add<RangeKeyword>("Ranges", "RangeB", "Distance range for second coordination number", sum1D_->range(1));
    keywords_.add<BoolKeyword>("Ranges", "RangeCEnabled", "Whether calculation of the third coordination number is enabled",
                               sum1D_->rangeEnabled(2));
    keywords_.add<RangeKeyword>("Ranges", "RangeC", "Distance range for third coordination number", sum1D_->range(2));

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
