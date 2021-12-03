// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/producers.h"
#include "gui/keywordwidgets/widgets.h"
#include "keywords/types.h"
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
    registerProducer<IsotopologueSetKeyword, IsotopologueSetKeywordWidget>();
    registerProducer<ModuleKeywordBase, ModuleKeywordWidget>();
    registerProducer<ModuleVectorKeyword, ModuleVectorKeywordWidget>();
    registerProducer<NodeKeywordBase, NodeKeywordWidget>();
    registerProducer<NodeValueEnumOptionsBaseKeyword, NodeValueEnumOptionsKeywordWidget>();
    registerProducer<NodeValueKeyword, NodeValueKeywordWidget>();
    registerProducer<OptionalDoubleKeyword, OptionalDoubleKeywordWidget>();
    registerProducer<RangeKeyword, RangeKeywordWidget>();
    registerProducer<SpeciesKeyword, SpeciesKeywordWidget>();
    registerProducer<SpeciesSiteKeyword, SpeciesSiteKeywordWidget>();
    registerProducer<SpeciesSiteVectorKeyword, SpeciesSiteVectorKeywordWidget>();
    registerProducer<SpeciesVectorKeyword, SpeciesVectorKeywordWidget>();
    registerProducer<StringKeyword, StringKeywordWidget>();
    registerProducer<Vec3DoubleKeyword, Vec3DoubleKeywordWidget>();
    registerProducer<Vec3IntegerKeyword, Vec3IntegerKeywordWidget>();
    registerProducer<Vec3NodeValueKeyword, Vec3NodeValueKeywordWidget>();

    // Keywords with no widgets
    registerNullProducer<Data1DStoreKeyword>();
    registerNullProducer<Data2DStoreKeyword>();
    registerNullProducer<Data3DStoreKeyword>();
    registerNullProducer<DynamicSiteNodesKeyword>();
    registerNullProducer<ElementVectorKeyword>();
    registerNullProducer<ExpressionKeyword>();
    registerNullProducer<GeometryListKeyword>();
    registerNullProducer<IntegerDoubleVectorKeyword>();
    registerNullProducer<IntegerStringVectorKeyword>();
    registerNullProducer<NodeBranchKeyword>();
    registerNullProducer<ProcedureKeyword>();
    registerNullProducer<StringDoubleVectorKeyword>();
    registerNullProducer<StringPairVectorKeyword>();
    registerNullProducer<ValueStoreKeyword>();
}

/*
 * Producers
 */

// Produce object of specified type
std::pair<QWidget *, KeywordWidgetBase *> KeywordWidgetProducer::produce(KeywordBase *keyword, const CoreData &coreData) const
{
    auto it = producers_.find(keyword->typeIndex());
    if (it == producers_.end())
        throw(std::runtime_error(fmt::format(
            "A producer has not been registered for type '{}', so a new widget for this keyword cannot be created.\n",
            keyword->typeIndex().name())));

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
