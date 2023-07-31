// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/qSpecies/qSpecies.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateExpression.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/select.h"

QSpeciesModule::QSpeciesModule() : Module(ModuleTypes::QSpecies), analyser_(ProcedureNode::AnalysisContext)
{
    // Select: Site 'A'
    selectBO_ = analyser_.createRootNode<SelectProcedureNode>("BO");
    auto &forEachBO = selectBO_->branch()->get();

    // -- Select: Site 'B'
    selectNF_ = forEachBO.create<SelectProcedureNode>("NF");
    selectNF_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectBO_});
    selectNF_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectBO_});
    selectNF_->keywords().set("ReferenceSite", selectBO_);
    selectNF_->keywords().set("InclusiveRange", distanceRange_);

    /*Loop over each NF site - check for exactly 2 sites
    Need to do this inside ForEach with IfValueRange*/

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF",
                                            selectBO_->speciesSites(), selectBO_->axesRequired());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'NF' for which the distribution around the origin sites 'A' should be calculated",
        selectNF_->speciesSites(), selectNF_->axesRequired());

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate coordination number from central site",
                                distanceRange_, 0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
}
