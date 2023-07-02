// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/widgetProducer.h"
#include "modules/accumulate/accumulate.h"
#include "modules/accumulate/gui/widget.h"
#include "modules/analyse/analyse.h"
#include "modules/analyse/gui/widget.h"
#include "modules/angle/angle.h"
#include "modules/angle/gui/widget.h"
#include "modules/atomShake/atomShake.h"
#include "modules/atomShake/gui/widget.h"
#include "modules/avgMol/avgMol.h"
#include "modules/avgMol/gui/widget.h"
#include "modules/axisAngle/axisAngle.h"
#include "modules/axisAngle/gui/widget.h"
#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/widget.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/widget.h"
#include "modules/dAngle/dAngle.h"
#include "modules/dAngle/gui/widget.h"
#include "modules/dataTest/dataTest.h"
#include "modules/dataTest/gui/widget.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/widget.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/widget.h"
#include "modules/geomOpt/geomOpt.h"
#include "modules/geomOpt/gui/widget.h"
#include "modules/gr/gr.h"
#include "modules/gr/gui/widget.h"
#include "modules/histogramCN/gui/widget.h"
#include "modules/histogramCN/histogramCN.h"
#include "modules/intraAngle/gui/widget.h"
#include "modules/intraAngle/intraAngle.h"
#include "modules/intraDistance/gui/widget.h"
#include "modules/intraDistance/intraDistance.h"
#include "modules/neutronSQ/gui/widget.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sdf/gui/widget.h"
#include "modules/sdf/sdf.h"
#include "modules/siteRDF/gui/widget.h"
#include "modules/siteRDF/siteRDF.h"
#include "modules/skeleton/gui/widget.h"
#include "modules/skeleton/skeleton.h"
#include "modules/sq/gui/widget.h"
#include "modules/sq/sq.h"
#include "modules/test/gui/widget.h"
#include "modules/test/test.h"
#include "modules/xRaySQ/gui/widget.h"
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
