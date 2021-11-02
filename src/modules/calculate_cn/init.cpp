// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    process1D_->setKeyword("CurrentDataOnly", true);
    SequenceProcedureNode *rdfNormalisation = process1D_->addNormalisationBranch();
    siteNormaliser_ = new OperateSitePopulationNormaliseProcedureNode;
    rdfNormalisation->addNode(siteNormaliser_);
    analyser_.addRootSequenceNode(process1D_);

    // Sum1D
    sum1D_ = new Sum1DProcedureNode(process1D_);
    sum1D_->setName("CN");
    analyser_.addRootSequenceNode(sum1D_);

    // Target / Ranges
    keywords_.add<ModuleKeyword<CalculateRDFModule>>(
        "Target / Ranges", "SourceRDF", "Source CalculateRDFModule containing the data to process", sourceRDF_, "CalculateRDF");
    keywords_.add<RangeKeyword>("Target / Ranges", "RangeA", "Distance range for first coordination number", sum1D_->range(0));
    keywords_.add<BoolKeyword>("Target / Ranges", "RangeBEnabled",
                               "Whether calculation of the second coordination number is enabled", sum1D_->rangeEnabled(1));
    keywords_.add<RangeKeyword>("Target / Ranges", "RangeB", "Distance range for second coordination number", sum1D_->range(1));
    keywords_.add<BoolKeyword>("Target / Ranges", "RangeCEnabled",
                               "Whether calculation of the third coordination number is enabled", sum1D_->rangeEnabled(2));
    keywords_.add<RangeKeyword>("Target / Ranges", "RangeC", "Distance range for third coordination number", sum1D_->range(2));

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
