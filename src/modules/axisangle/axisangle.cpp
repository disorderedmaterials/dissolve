// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/axisangle/axisangle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/speciessitevector.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/calculateaxisangle.h"
#include "procedure/nodes/calculatedistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/operatenormalise.h"
#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"
#include "procedure/nodes/select.h"

AxisAngleModule::AxisAngleModule() : Module("CalculateAxisAngle"), analyser_(ProcedureNode::AnalysisContext)
{
    try
    {
        // Select: Site 'A'
        selectA_ = analyser_.createRootNode<SelectProcedureNode>("A", std::vector<const SpeciesSite *>{},
                                                                 ProcedureNode::NodeContext::AnalysisContext, true);
        auto &forEachA = selectA_->branch()->get();

        // -- Select: Site 'B'
        selectB_ = forEachA.create<SelectProcedureNode>("B", std::vector<const SpeciesSite *>{},
                                                        ProcedureNode::NodeContext::AnalysisContext, true);
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
        auto &forEachB = selectB_->branch()->get();

        // -- -- Calculate: 'rAB'
        auto calcDistance = forEachB.create<CalculateDistanceProcedureNode>({}, selectA_, selectB_);

        // -- -- Calculate: 'axisAngle'
        calculateAxisAngle_ =
            forEachB.create<CalculateAxisAngleProcedureNode>({}, selectA_, OrientedSite::XAxis, selectB_, OrientedSite::XAxis);
        calculateAxisAngle_->keywords().set("Symmetric", symmetric_);

        // -- -- Collect2D:  'rAB vs axisAngle)'
        collectDAngle_ = forEachB.create<Collect2DProcedureNode>(
            {}, calcDistance, calculateAxisAngle_, ProcedureNode::AnalysisContext, distanceRange_.x, distanceRange_.y,
            distanceRange_.z, angleRange_.x, angleRange_.y, angleRange_.z);
        auto &subCollection = collectDAngle_->branch()->get();

        // -- -- -- Collect1D:  'RDF(AB)'
        collectDistance_ = subCollection.create<Collect1DProcedureNode>({}, calcDistance, ProcedureNode::AnalysisContext,
                                                                        distanceRange_.x, distanceRange_.y, distanceRange_.z);

        // -- -- -- Collect1D:  'ANGLE(axis-axis)'
        collectAngle_ = subCollection.create<Collect1DProcedureNode>({}, calculateAxisAngle_, ProcedureNode::AnalysisContext,
                                                                     angleRange_.x, angleRange_.y, angleRange_.z);

        // Process1D: 'RDF(AB)'
        processDistance_ =
            analyser_.createRootNode<Process1DProcedureNode>("RDF(AB)", collectDistance_, ProcedureNode::OperateContext);
        processDistance_->keywords().set("LabelValue", std::string("g(r)"));
        processDistance_->keywords().set("LabelX", std::string("r, \\symbol{Angstrom}"));

        auto &rdfNormalisation = processDistance_->branch()->get();
        rdfNormalisation.create<OperateSitePopulationNormaliseProcedureNode>({},
                                                                             ConstNodeVector<SelectProcedureNode>{selectA_});
        rdfNormalisation.create<OperateNumberDensityNormaliseProcedureNode>({}, ConstNodeVector<SelectProcedureNode>{selectB_});
        rdfNormalisation.create<OperateSphericalShellNormaliseProcedureNode>({});

        // Process1D: 'ANGLE(axis)'
        processAngle_ = analyser_.createRootNode<Process1DProcedureNode>("AxisAngle(AB)", collectAngle_);
        processAngle_->keywords().set("LabelValue", std::string("Normalised Frequency"));
        processAngle_->keywords().set("LabelX", std::string("\\symbol{theta}, \\symbol{degrees}"));
        auto &angleNormalisation = processAngle_->branch()->get();
        angleNormalisation.create<OperateExpressionProcedureNode>({}, "value/sin(x)");
        angleNormalisation.create<OperateNormaliseProcedureNode>({}, 1.0);

        // Process2D: 'DAngle'
        processDAngle_ = analyser_.createRootNode<Process2DProcedureNode>("DAxisAngle", collectDAngle_);
        processDAngle_->keywords().set("LabelX", std::string("\\it{r}, \\symbol{Angstrom}"));
        processDAngle_->keywords().set("LabelY", std::string("\\symbol{theta}, \\symbol{degrees}"));
        processDAngle_->keywords().set("LabelValue", std::string("g(r)"));
        auto &dAngleNormalisation = processDAngle_->branch()->get();
        dAngleNormalisationExpression_ = dAngleNormalisation.create<OperateExpressionProcedureNode>(
            {}, fmt::format("{} * value/sin(y)/sin(yDelta)", symmetric_ ? 1.0 : 2.0));
        dAngleNormalisation.create<OperateSitePopulationNormaliseProcedureNode>(
            {}, ConstNodeVector<SelectProcedureNode>({selectA_}));
        dAngleNormalisation.create<OperateNumberDensityNormaliseProcedureNode>({},
                                                                               ConstNodeVector<SelectProcedureNode>{selectB_});
        dAngleNormalisation.create<OperateSphericalShellNormaliseProcedureNode>({});
    }
    catch (...)
    {
        Messenger::error("Failed to create analysis procedure for module '{}'\n", name_);
    }

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Specify site(s) which represent 'A' in the interaction A-B...C",
                                            selectA_->speciesSites(), selectA_->axesRequired());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisA", "Axis to use from site A", calculateAxisAngle_->axis(0),
                                                              OrientedSite::siteAxis());
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Specify site(s) which represent 'B' in the interaction A-B...C",
                                            selectB_->speciesSites(), selectB_->axesRequired());
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisB", "Axis to use from site B", calculateAxisAngle_->axis(1),
                                                              OrientedSite::siteAxis());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<Vec3DoubleKeyword>("DistanceRange", "Range (min, max, binwidth) of distance binning", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMolecule_);
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportRDF", "File format and file name under which to save calculated B-C RDF",
                                        processDistance_->exportFileAndFormat(), "EndExportRDF");
    keywords_.add<FileAndFormatKeyword>(
        "ExportAngle", "File format and file name under which to save calculated A-B...C angle histogram to disk",
        processAngle_->exportFileAndFormat(), "EndExportAngle");
    keywords_.add<FileAndFormatKeyword>("ExportDAngle",
                                        "File format and file name under which to save calculated A-B...C angle map to disk",
                                        processDAngle_->exportFileAndFormat(), "EndExportDAngle");
}
