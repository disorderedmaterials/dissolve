// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/widgetProducer.h"
#include "modules/accumulate/accumulate.h"
#include "modules/accumulate/gui/accumulateWidget.h"
#include "modules/analyse/analyse.h"
#include "modules/analyse/gui/analyseWidget.h"
#include "modules/angle/angle.h"
#include "modules/angle/gui/angleWidget.h"
#include "modules/atomShake/atomShake.h"
#include "modules/atomShake/gui/atomShakeWidget.h"
#include "modules/avgMol/avgMol.h"
#include "modules/avgMol/gui/avgMolWidget.h"
#include "modules/axisAngle/axisAngle.h"
#include "modules/axisAngle/gui/axisAngleWidget.h"
#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/benchmarkWidget.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/braggWidget.h"
#include "modules/dAngle/dAngle.h"
#include "modules/dAngle/gui/dAngleWidget.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/energyWidget.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/epsrWidget.h"
#include "modules/geomOpt/geomOpt.h"
#include "modules/geomOpt/gui/geomOptWidget.h"
#include "modules/gr/gr.h"
#include "modules/gr/gui/grWidget.h"
#include "modules/histogramCN/gui/histogramCNWidget.h"
#include "modules/histogramCN/histogramCN.h"
#include "modules/intraAngle/gui/intraAngleWidget.h"
#include "modules/intraAngle/intraAngle.h"
#include "modules/intraDistance/gui/intraDistanceWidget.h"
#include "modules/intraDistance/intraDistance.h"
#include "modules/modifierOSites/gui/modifierOSitesWidget.h"
#include "modules/modifierOSites/modifierOSites.h"
#include "modules/neutronSQ/gui/neutronSQWidget.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/orientedSDF/gui/orientedSDFWidget.h"
#include "modules/orientedSDF/orientedSDF.h"
#include "modules/qSpecies/gui/qSpeciesWidget.h"
#include "modules/qSpecies/qSpecies.h"
#include "modules/sdf/gui/sdfWidget.h"
#include "modules/sdf/sdf.h"
#include "modules/siteRDF/gui/siteRDFWidget.h"
#include "modules/siteRDF/siteRDF.h"
#include "modules/skeleton/gui/skeletonWidget.h"
#include "modules/skeleton/skeleton.h"
#include "modules/sq/gui/sqWidget.h"
#include "modules/sq/sq.h"
#include "modules/test/gui/testWidget.h"
#include "modules/test/test.h"
#include "modules/xRaySQ/gui/xRaySQWidget.h"
#include "modules/xRaySQ/xRaySQ.h"
#include <ios>

ModuleWidgetProducer::ModuleWidgetProducer()
{
    registerProducer<AccumulateModule, AccumulateModuleWidget>();
    registerProducer<AnalyseModule, AnalyseModuleWidget>();
    registerProducer<AngleModule, AngleModuleWidget>();
    registerProducer<AtomShakeModule, AtomShakeModuleWidget>();
    registerProducer<AvgMolModule, AvgMolModuleWidget>();
    registerProducer<AxisAngleModule, AxisAngleModuleWidget>();
    registerProducer<BenchmarkModule, BenchmarkModuleWidget>();
    registerProducer<BraggModule, BraggModuleWidget>();
    registerProducer<DAngleModule, DAngleModuleWidget>();
    registerProducer<EnergyModule, EnergyModuleWidget>();
    registerProducer<EPSRModule, EPSRModuleWidget>();
    registerProducer<GeometryOptimisationModule, GeometryOptimisationModuleWidget>();
    registerProducer<GRModule, GRModuleWidget>();
    registerProducer<HistogramCNModule, HistogramCNModuleWidget>();
    registerProducer<IntraAngleModule, IntraAngleModuleWidget>();
    registerProducer<IntraDistanceModule, IntraDistanceModuleWidget>();
    registerProducer<ModifierOSitesModule,ModifierOSitesModuleWidget>();
    registerProducer<NeutronSQModule, NeutronSQModuleWidget>();
    registerProducer<OrientedSDFModule, OrientedSDFModuleWidget>();
    registerProducer<QSpeciesModule, QSpeciesModuleWidget>();
    registerProducer<SDFModule, SDFModuleWidget>();
    registerProducer<SkeletonModule, SkeletonModuleWidget>();
    registerProducer<SiteRDFModule, SiteRDFModuleWidget>();
    registerProducer<SQModule, SQModuleWidget>();
    registerProducer<TestModule, TestModuleWidget>();
    registerProducer<XRaySQModule, XRaySQModuleWidget>();
}

/*
 * Producers
 */

// Produce object of specified type
ModuleWidget *ModuleWidgetProducer::produce(Module *module, Dissolve &dissolve) const
{
    auto it = producers_.find(typeid(*module));
    if (it == producers_.end())
    {
        Messenger::printVerbose(
            "A producer has not been registered for module type '{}' ('{}'), so a new widget for it cannot be created.\n",
            ModuleTypes::moduleType(module->type()), typeid(*module).name());
        return nullptr;
    }

    return (it->second)(module, dissolve);
}

/*
 * Instance
 */

// Return the producer instance
const ModuleWidgetProducer &ModuleWidgetProducer::instance()
{
    static ModuleWidgetProducer instance;

    return instance;
}
