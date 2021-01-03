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
    SequenceProcedureNode *rdfNormalisation = process1D_->addNormalisationBranch();
    siteNormaliser_ = new OperateSitePopulationNormaliseProcedureNode;
    rdfNormalisation->addNode(siteNormaliser_);
    analyser_.addRootSequenceNode(process1D_);

    // Sum1D
    sum1D_ = new Sum1DProcedureNode(process1D_);
    sum1D_->setName("CN");
    analyser_.addRootSequenceNode(sum1D_);

    // Target / Ranges
    keywords_.add("Target / Ranges", new ModuleKeyword<const CalculateRDFModule>("CalculateRDF"), "SourceRDF",
                  "Source CalculateRDFModule containing the data to process");
    keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeA"), "RangeA",
                   "Distance range for first coordination number");
    keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeBEnabled"), "RangeBEnabled",
                   "Whether calculation of the second coordination number is enabled");
    keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeB"), "RangeB",
                   "Distance range for second coordination number");
    keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeCEnabled"), "RangeCEnabled",
                   "Whether calculation of the third coordination number is enabled");
    keywords_.link("Target / Ranges", sum1D_->keywords().find("RangeC"), "RangeC",
                   "Distance range for third coordination number");

    // Test
    keywords_.add("Test", new DoubleKeyword(0.0), "TestRangeA",
                  "Reference coordination number for range A against which calculated value should be tested");
    keywords_.add("Test", new DoubleKeyword(0.0), "TestRangeB",
                  "Reference coordination number for range B against which calculated value should be tested");
    keywords_.add("Test", new DoubleKeyword(0.0), "TestRangeC",
                  "Reference coordination number for range C against which calculated value should be tested");
    keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold difference at which test comparisons will fail");
}
