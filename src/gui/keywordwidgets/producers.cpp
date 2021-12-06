// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/producers.h"
#include "gui/keywordwidgets/atomtypevector.h"
#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/bool.hui"
#include "gui/keywordwidgets/configuration.hui"
#include "gui/keywordwidgets/configurationvector.h"
#include "gui/keywordwidgets/double.hui"
#include "gui/keywordwidgets/dropwidget.hui"
#include "gui/keywordwidgets/enumoptions.hui"
#include "gui/keywordwidgets/expressionvariablevector.h"
#include "gui/keywordwidgets/fileandformat.h"
#include "gui/keywordwidgets/function1d.h"
#include "gui/keywordwidgets/integer.hui"
#include "gui/keywordwidgets/isotopologueset.h"
#include "gui/keywordwidgets/module.h"
#include "gui/keywordwidgets/modulevector.h"
#include "gui/keywordwidgets/node.h"
#include "gui/keywordwidgets/nodeandinteger.h"
#include "gui/keywordwidgets/nodevalue.h"
#include "gui/keywordwidgets/nodevalueenumoptions.h"
#include "gui/keywordwidgets/nodevector.h"
#include "gui/keywordwidgets/optionaldouble.hui"
#include "gui/keywordwidgets/range.h"
#include "gui/keywordwidgets/species.hui"
#include "gui/keywordwidgets/speciessite.h"
#include "gui/keywordwidgets/speciessitevector.h"
#include "gui/keywordwidgets/speciesvector.h"
#include "gui/keywordwidgets/stdstring.hui"
#include "gui/keywordwidgets/vec3double.h"
#include "gui/keywordwidgets/vec3integer.h"
#include "gui/keywordwidgets/vec3nodevalue.h"
#include "keywords/data1dstore.h"
#include "keywords/data2dstore.h"
#include "keywords/data3dstore.h"
#include "keywords/dynamicsitenodes.h"
#include "keywords/elementvector.h"
#include "keywords/expression.h"
#include "keywords/geometrylist.h"
#include "keywords/nodebranch.h"
#include "keywords/procedure.h"
#include "keywords/valuestore.h"
#include "keywords/vector_intdouble.h"
#include "keywords/vector_intstring.h"
#include "keywords/vector_stringdouble.h"
#include "keywords/vector_stringpair.h"
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
