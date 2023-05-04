// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/widgetproducer.h"
#include "modules/accumulate/accumulate.h"
#include "modules/accumulate/gui/accumulatewidget.h"
#include "modules/analyse/analyse.h"
#include "modules/analyse/gui/analysewidget.h"
#include "modules/angle/angle.h"
#include "modules/angle/gui/anglewidget.h"
#include "modules/atomshake/atomshake.h"
#include "modules/atomshake/gui/atomshakewidget.h"
#include "modules/avgmol/avgmol.h"
#include "modules/avgmol/gui/avgmolwidget.h"
#include "modules/axisangle/axisangle.h"
#include "modules/axisangle/gui/axisanglewidget.h"
#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/benchmarkwidget.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/braggwidget.h"
#include "modules/dangle/dangle.h"
#include "modules/dangle/gui/danglewidget.h"
#include "modules/datatest/datatest.h"
#include "modules/datatest/gui/datatestwidget.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/energywidget.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/epsrwidget.h"
#include "modules/geomopt/geomopt.h"
#include "modules/geomopt/gui/geomoptwidget.h"
#include "modules/gr/gr.h"
#include "modules/gr/gui/grwidget.h"
#include "modules/histogramcn/gui/histogramcnwidget.h"
#include "modules/histogramcn/histogramcn.h"
#include "modules/intraangle/gui/intraanglewidget.h"
#include "modules/intraangle/intraangle.h"
#include "modules/intradistance/gui/intradistancewidget.h"
#include "modules/intradistance/intradistance.h"
#include "modules/neutronsq/gui/neutronsqwidget.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/sdf/gui/sdfwidget.h"
#include "modules/sdf/sdf.h"
#include "modules/siterdf/gui/siterdfwidget.h"
#include "modules/siterdf/siterdf.h"
#include "modules/skeleton/gui/skeletonwidget.h"
#include "modules/skeleton/skeleton.h"
#include "modules/sq/gui/sqwidget.h"
#include "modules/sq/sq.h"
#include "modules/test/gui/testwidget.h"
#include "modules/test/test.h"
#include "modules/xraysq/gui/xraysqwidget.h"
#include "modules/xraysq/xraysq.h"
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
    registerProducer<DataTestModule, DataTestModuleWidget>();
    registerProducer<EnergyModule, EnergyModuleWidget>();
    registerProducer<EPSRModule, EPSRModuleWidget>();
    registerProducer<GeometryOptimisationModule, GeometryOptimisationModuleWidget>();
    registerProducer<GRModule, GRModuleWidget>();
    registerProducer<HistogramCNModule, HistogramCNModuleWidget>();
    registerProducer<IntraAngleModule, IntraAngleModuleWidget>();
    registerProducer<IntraDistanceModule, IntraDistanceModuleWidget>();
    registerProducer<NeutronSQModule, NeutronSQModuleWidget>();
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
