/*
    *** SQ Module - Processing
    *** src/modules/sq/process.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/box.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"

// Run main processing
bool SQModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate S(Q) from Configuration's g(r).
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    const auto &qBroadening = keywords_.retrieve<BroadeningFunction>("QBroadening", BroadeningFunction());
    const auto qDelta = keywords_.asDouble("QDelta");
    const auto qMin = keywords_.asDouble("QMin");
    double qMax = keywords_.asDouble("QMax");
    if (qMax < 0.0)
        qMax = 30.0;
    const bool saveData = keywords_.asBool("Save");
    const auto &windowFunction = keywords_.retrieve<WindowFunction>("WindowFunction", WindowFunction());

    // Print argument/parameter summary
    Messenger::print("SQ: Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin,
                     qMax, qDelta);
    if (windowFunction.function() == WindowFunction::NoWindow)
        Messenger::print("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        Messenger::print("SQ: Window function to be applied in Fourier transforms is {} ({}).",
                         WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary());
    if (qBroadening.function() == BroadeningFunction::NoFunction)
        Messenger::print("SQ: No broadening will be applied to calculated S(Q).");
    else
        Messenger::print("SQ: Broadening to be applied in calculated S(Q) is {} ({}).",
                         BroadeningFunction::functionType(qBroadening.function()), qBroadening.parameterSummary());
    Messenger::print("SQ: Save data is {}.\n", DissolveSys::onOff(saveData));
    Messenger::print("\n");

    /*
     * Loop over target Configurations and Fourier transform their UnweightedGR into the UnweightedSQ.
     */
    for (Configuration *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Get unweighted g(r) for this Configuration - we don't supply a specific Module prefix, since the unweighted
        // g(r) may come from one of many RDF-type modules
        if (!cfg->moduleData().contains("UnweightedGR"))
            return Messenger::error("Couldn't locate UnweightedGR for Configuration '{}'.\n", cfg->name());
        const auto &unweightedgr = GenericListHelper<PartialSet>::value(cfg->moduleData(), "UnweightedGR");

        // Does a PartialSet already exist for this Configuration?
        bool wasCreated;
        auto &unweightedsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedSQ", "SQ",
                                                                    GenericItem::InRestartFileFlag, &wasCreated);
        if (wasCreated)
            unweightedsq.setUpPartials(unweightedgr.atomTypes(), fmt::format("{}-{}", cfg->niceName(), uniqueName()),
                                       "unweighted", "sq", "Q, 1/Angstroms");

        // Is the PartialSet already up-to-date?
        if (DissolveSys::sameString(unweightedsq.fingerprint(), fmt::format("{}", cfg->moduleData().version("UnweightedGR"))))
        {
            Messenger::print("SQ: Unweighted partial S(Q) are up-to-date for Configuration '{}'.\n", cfg->name());
            continue;
        }

        // Transform g(r) into S(Q)
        if (!calculateUnweightedSQ(procPool, cfg, unweightedgr, unweightedsq, qMin, qDelta, qMax, cfg->atomicDensity(),
                                   windowFunction, qBroadening))
            return false;

        // Set names of resources (Data1D) within the PartialSet
        unweightedsq.setObjectTags(fmt::format("{}//{}//{}", cfg->niceName(), "SQ", "UnweightedSQ"));
        unweightedsq.setFingerprint(fmt::format("{}", cfg->moduleData().version("UnweightedGR")));

        // Save data if requested
        if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, unweightedsq.save())))
            return false;
    }

    // Create/retrieve PartialSet for summed partial S(Q)
    auto &summedUnweightedSQ = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedSQ",
                                                                      uniqueName_, GenericItem::InRestartFileFlag);

    // Sum the partials from the associated Configurations
    if (!sumUnweightedSQ(procPool, this, dissolve.processingModuleData(), summedUnweightedSQ))
        return false;

    // Create/retrieve PartialSet for summed unweighted g(r)
    auto &summedUnweightedGR = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedGR",
                                                                      uniqueName_, GenericItem::InRestartFileFlag);

    // Sum the partials from the associated Configurations
    if (!RDFModule::sumUnweightedGR(procPool, this, dissolve.processingModuleData(), summedUnweightedGR))
        return false;

    return true;
}
