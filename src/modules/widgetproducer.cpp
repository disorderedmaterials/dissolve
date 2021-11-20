// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/widgetproducer.h"
#include "modules/accumulate/accumulate.h"
#include "modules/accumulate/gui/accumulatewidget.h"
#include "modules/analyse/analyse.h"
#include "modules/analyse/gui/analysewidget.h"
#include "modules/atomshake/atomshake.h"
#include "modules/atomshake/gui/atomshakewidget.h"
#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/benchmarkwidget.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/braggwidget.h"
#include "modules/calculate_angle/angle.h"
#include "modules/calculate_angle/gui/calculateanglewidget.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_avgmol/gui/calculateavgmolwidget.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "modules/calculate_axisangle/gui/calculateaxisanglewidget.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_cn/gui/calculatecnwidget.h"
#include "modules/calculate_dangle/dangle.h"
#include "modules/calculate_dangle/gui/calculatedanglewidget.h"
#include "modules/calculate_rdf/gui/calculaterdfwidget.h"
#include "modules/calculate_rdf/rdf.h"
#include "modules/calculate_sdf/gui/calculatesdfwidget.h"
#include "modules/calculate_sdf/sdf.h"
#include "modules/datatest/datatest.h"
#include "modules/datatest/gui/datatestwidget.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/energywidget.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/epsrwidget.h"
#include "modules/geomopt/geomopt.h"
#include "modules/geomopt/gui/geomoptwidget.h"
#include "modules/neutronsq/gui/neutronsqwidget.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/gui/rdfwidget.h"
#include "modules/rdf/rdf.h"
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
    registerProducer<AtomShakeModule, AtomShakeModuleWidget>();
    registerProducer<BenchmarkModule, BenchmarkModuleWidget>();
    registerProducer<BraggModule, BraggModuleWidget>();
    registerProducer<CalculateAngleModule, CalculateAngleModuleWidget>();
    registerProducer<CalculateAvgMolModule, CalculateAvgMolModuleWidget>();
    registerProducer<CalculateAxisAngleModule, CalculateAxisAngleModuleWidget>();
    registerProducer<CalculateCNModule, CalculateCNModuleWidget>();
    registerProducer<CalculateDAngleModule, CalculateDAngleModuleWidget>();
    registerProducer<CalculateRDFModule, CalculateRDFModuleWidget>();
    registerProducer<CalculateSDFModule, CalculateSDFModuleWidget>();
    registerProducer<DataTestModule, DataTestModuleWidget>();
    registerProducer<EnergyModule, EnergyModuleWidget>();
    registerProducer<EPSRModule, EPSRModuleWidget>();
    registerProducer<GeometryOptimisationModule, GeometryOptimisationModuleWidget>();
    registerProducer<NeutronSQModule, NeutronSQModuleWidget>();
    registerProducer<RDFModule, RDFModuleWidget>();
    registerProducer<SkeletonModule, SkeletonModuleWidget>();
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
            module->type(), typeid(*module).name());
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
