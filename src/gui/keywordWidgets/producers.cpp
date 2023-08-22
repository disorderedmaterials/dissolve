// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/producers.h"
#include "gui/keywordWidgets/atomTypeVector.h"
#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/bool.hui"
#include "gui/keywordWidgets/configuration.h"
#include "gui/keywordWidgets/configurationVector.h"
#include "gui/keywordWidgets/double.hui"
#include "gui/keywordWidgets/dropWidget.hui"
#include "gui/keywordWidgets/enumOptions.hui"
#include "gui/keywordWidgets/expressionVariableVector.h"
#include "gui/keywordWidgets/fileAndFormat.h"
#include "gui/keywordWidgets/function1D.h"
#include "gui/keywordWidgets/integer.hui"
#include "gui/keywordWidgets/interactionPotential.h"
#include "gui/keywordWidgets/isotopologueSet.h"
#include "gui/keywordWidgets/layer.h"
#include "gui/keywordWidgets/module.h"
#include "gui/keywordWidgets/moduleVector.h"
#include "gui/keywordWidgets/node.h"
#include "gui/keywordWidgets/nodeAndInteger.h"
#include "gui/keywordWidgets/nodeValue.h"
#include "gui/keywordWidgets/nodeValueEnumOptions.h"
#include "gui/keywordWidgets/nodeVector.h"
#include "gui/keywordWidgets/optionalDouble.hui"
#include "gui/keywordWidgets/optionalInt.hui"
#include "gui/keywordWidgets/range.h"
#include "gui/keywordWidgets/rangeVector.h"
#include "gui/keywordWidgets/species.h"
#include "gui/keywordWidgets/speciesSite.h"
#include "gui/keywordWidgets/speciesSiteVector.h"
#include "gui/keywordWidgets/speciesVector.h"
#include "gui/keywordWidgets/stdString.hui"
#include "gui/keywordWidgets/vec3Double.h"
#include "gui/keywordWidgets/vec3Integer.h"
#include "gui/keywordWidgets/vec3NodeValue.h"
#include "gui/keywordWidgets/weightedModuleVector.h"
#include "keywords/data1DStore.h"
#include "keywords/data2DStore.h"
#include "keywords/data3DStore.h"
#include "keywords/dataSource.h"
#include "keywords/elementVector.h"
#include "keywords/expression.h"
#include "keywords/nodeBranch.h"
#include "keywords/procedure.h"
#include <ios>

KeywordWidgetProducer::KeywordWidgetProducer()
{
    // Keywords with normal widgets
    registerProducer<AtomTypeVectorKeyword, AtomTypeVectorKeywordWidget>();
    registerProducer<BoolKeyword, BoolKeywordWidget>();
    registerProducer<ConfigurationKeyword, ConfigurationKeywordWidget>();
    registerProducer<ConfigurationVectorKeyword, ConfigurationVectorKeywordWidget>();
    registerProducer<DoubleKeyword, DoubleKeywordWidget>();
    registerProducer<EnumOptionsBaseKeyword, EnumOptionsKeywordWidget>();
    registerProducer<ExpressionVariableVectorKeyword, ExpressionVariableVectorKeywordWidget>();
    registerProducer<FileAndFormatKeyword, FileAndFormatKeywordWidget>();
    registerProducer<Function1DKeyword, Function1DKeywordWidget>();
    registerProducer<IntegerKeyword, IntegerKeywordWidget>();
    registerProducer<InteractionPotentialBaseKeyword, InteractionPotentialKeywordWidget>();
    registerProducer<IsotopologueSetKeyword, IsotopologueSetKeywordWidget>();
    registerProducer<LayerKeyword, LayerKeywordWidget>();
    registerProducer<ModuleKeywordBase, ModuleKeywordWidget>();
    registerProducer<ModuleVectorKeyword, ModuleVectorKeywordWidget>();
    registerProducer<NodeKeywordBase, NodeKeywordWidget>();
    registerProducer<NodeAndIntegerKeywordBase, NodeAndIntegerKeywordWidget>();
    registerProducer<NodeValueEnumOptionsBaseKeyword, NodeValueEnumOptionsKeywordWidget>();
    registerProducer<NodeValueKeyword, NodeValueKeywordWidget>();
    registerProducer<NodeVectorKeywordBase, NodeVectorKeywordWidget>();
    registerProducer<OptionalDoubleKeyword, OptionalDoubleKeywordWidget>();
    registerProducer<OptionalIntegerKeyword, OptionalIntegerKeywordWidget>();
    registerProducer<RangeKeyword, RangeKeywordWidget>();
    registerProducer<RangeVectorKeyword, RangeVectorKeywordWidget>();
    registerProducer<SpeciesKeyword, SpeciesKeywordWidget>();
    registerProducer<SpeciesSiteKeyword, SpeciesSiteKeywordWidget>();
    registerProducer<SpeciesSiteVectorKeyword, SpeciesSiteVectorKeywordWidget>();
    registerProducer<SpeciesVectorKeyword, SpeciesVectorKeywordWidget>();
    registerProducer<StringKeyword, StringKeywordWidget>();
    registerProducer<Vec3DoubleKeyword, Vec3DoubleKeywordWidget>();
    registerProducer<Vec3IntegerKeyword, Vec3IntegerKeywordWidget>();
    registerProducer<Vec3NodeValueKeyword, Vec3NodeValueKeywordWidget>();
    registerProducer<WeightedModuleVectorKeyword, WeightedModuleVectorKeywordWidget>();

    // Keywords with no widgets
    registerNullProducer<Data1DStoreKeyword>();
    registerNullProducer<Data2DStoreKeyword>();
    registerNullProducer<Data3DStoreKeyword>();
    registerNullProducer<DataSourceKeyword<Data1D, std::function<bool(std::vector<Data1D>)>>>();
    registerNullProducer<DataSourceKeyword<Data2D, std::function<bool(std::vector<Data2D>)>>>();
    registerNullProducer<DataSourceKeyword<Data3D, std::function<bool(std::vector<Data3D>)>>>();
    registerNullProducer<ElementVectorKeyword>();
    registerNullProducer<ExpressionKeyword>();
    registerNullProducer<NodeBranchKeyword>();
    registerNullProducer<ProcedureKeyword>();
}

/*
 * Producers
 */

// Produce object of specified type
std::pair<QWidget *, KeywordWidgetBase *> KeywordWidgetProducer::produce(KeywordBase *keyword, const CoreData &coreData) const
{
    auto it = producers_.find(keyword->typeIndex());
    if (it == producers_.end())
        throw(std::runtime_error(fmt::format("A producer has not been registered for type '{}' (keyword name is '{}'), so a "
                                             "new widget for this keyword cannot be created.\n",
                                             keyword->typeIndex().name(), keyword->name())));

    return (it->second)(keyword, coreData);
}

/*
 * Instance
 */

// Return the producer instance
const KeywordWidgetProducer &KeywordWidgetProducer::instance()
{
    static KeywordWidgetProducer instance;

    return instance;
}
