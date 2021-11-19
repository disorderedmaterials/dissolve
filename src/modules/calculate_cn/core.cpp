// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/module.h"
#include "keywords/range.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/sum1d.h"

CalculateCNModule::CalculateCNModule() : Module(), analyser_(ProcedureNode::AnalysisContext)
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
    process1D_ = std::make_shared<Process1DProcedureNode>();
    process1D_->setName("HistogramNorm");
    process1D_->keywords().set("CurrentDataOnly", true);
    std::shared_ptr<SequenceProcedureNode> rdfNormalisation = process1D_->addNormalisationBranch();
    siteNormaliser_ = std::make_shared<OperateSitePopulationNormaliseProcedureNode>(
        std::vector<std::shared_ptr<const SelectProcedureNode>>());
    rdfNormalisation->addNode(siteNormaliser_);
    analyser_.addRootSequenceNode(process1D_);

    // Sum1D
    sum1D_ = std::make_shared<Sum1DProcedureNode>(process1D_);
    sum1D_->setName("CN");
    analyser_.addRootSequenceNode(sum1D_);

    // Target
    keywords_.addTarget<ModuleKeyword<CalculateRDFModule>>(
        "SourceRDF", "Source CalculateRDFModule containing the data to process", sourceRDF_, "CalculateRDF");

    // Ranges
    keywords_.add<RangeKeyword>("Ranges", "RangeA", "Distance range for first coordination number", sum1D_->range(0));
    keywords_.add<BoolKeyword>("anges", "RangeBEnabled", "Whether calculation of the second coordination number is enabled",
                               sum1D_->rangeEnabled(1));
    keywords_.add<RangeKeyword>("Ranges", "RangeB", "Distance range for second coordination number", sum1D_->range(1));
    keywords_.add<BoolKeyword>("Ranges", "RangeCEnabled", "Whether calculation of the third coordination number is enabled",
                               sum1D_->rangeEnabled(2));
    keywords_.add<RangeKeyword>("Ranges", "RangeC", "Distance range for third coordination number", sum1D_->range(2));

    // Test
    keywords_.add<DoubleKeyword>("Test", "TestRangeA",
                                 "Reference coordination number for range A against which calculated value should be tested",
                                 testRangeA_);
    keywords_.add<DoubleKeyword>("Test", "TestRangeB",
                                 "Reference coordination number for range B against which calculated value should be tested",
                                 testRangeB_);
    keywords_.add<DoubleKeyword>("Test", "TestRangeC",
                                 "Reference coordination number for range C against which calculated value should be tested",
                                 testRangeC_);
    keywords_.add<DoubleKeyword>("Test", "TestThreshold", "Threshold difference at which test comparisons will fail",
                                 testThreshold_, 1.0e-5);
}
