// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/siteRDF/siteRDF.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/fileAndFormat.h"
#include "keywords/optionalDouble.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateDistance.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateNumberDensityNormalise.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/operateSphericalShellNormalise.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sum1D.h"

SiteRDFModule::SiteRDFModule() : Module(ModuleTypes::SiteRDF), analyser_(ProcedureNode::AnalysisContext)
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
    rdfNormalisation.create<OperateSitePopulationNormaliseProcedureNode>({}, ConstNodeVector<SelectProcedureNode>({selectA_}));
    rdfNormalisation.create<OperateNumberDensityNormaliseProcedureNode>({}, ConstNodeVector<SelectProcedureNode>({selectB_}));
    rdfNormalisation.create<OperateSphericalShellNormaliseProcedureNode>({});

    // Process1D: CN
    processCN_ = analyser_.createRootNode<Process1DProcedureNode>("HistogramNorm", collectDistance_);
    processCN_->keywords().set("Instantaneous", true);
    auto &cnNormalisation = processCN_->branch()->get();
    cnNormaliser_ = cnNormalisation.create<OperateSitePopulationNormaliseProcedureNode>(
        {}, ConstNodeVector<SelectProcedureNode>({selectA_}));

    // Sum1D
    sumCN_ = analyser_.createRootNode<Sum1DProcedureNode>("CN", processCN_);

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated",
        selectB_->speciesSites(), selectB_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<Vec3DoubleKeyword>("DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
                               excludeSameMolecule_);

    keywords_.setOrganisation("Options", "Coordination Numbers");
    keywords_.add<BoolKeyword>("RangeAEnabled", "Whether calculation of the second coordination number is enabled",
                               sumCN_->rangeEnabled(0));
    keywords_.add<RangeKeyword>("RangeA", "Distance range for first coordination number", sumCN_->range(0));
    keywords_.add<BoolKeyword>("RangeBEnabled", "Whether calculation of the second coordination number is enabled",
                               sumCN_->rangeEnabled(1));
    keywords_.add<RangeKeyword>("RangeB", "Distance range for second coordination number", sumCN_->range(1));
    keywords_.add<BoolKeyword>("RangeCEnabled", "Whether calculation of the third coordination number is enabled",
                               sumCN_->rangeEnabled(2));
    keywords_.add<RangeKeyword>("RangeC", "Distance range for third coordination number", sumCN_->range(2));

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated RDF data",
                                        processDistance_->exportFileAndFormat(), "EndExport");
    keywords_.add<BoolKeyword>("ExportInstantaneousCN", "Export instantaneous coordination numbers to disk\n",
                               exportInstantaneous_);
}
