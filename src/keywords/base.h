/*
    *** Keyword Base Class
    *** src/keywords/base.h
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

#pragma once

#include "templates/listitem.h"
#include "templates/reflist.h"
#include "templates/vector3.h"
#include <memory>

// Forward Declarations
class AtomType;
class Configuration;
class CoreData;
class Isotopologue;
class LineParser;
class Module;
class ProcedureNode;
class Species;
class SpeciesSite;
class ProcessPool;

// Keyword Base Class
class KeywordBase : public ListItem<KeywordBase>
{
    public:
    // Keyword Data Type
    enum KeywordDataType
    {
        AtomTypeRefListData,
        AtomTypeSelectionData,
        BoolData,
        BroadeningFunctionData,
        Data1DStoreData,
        Data2DStoreData,
        Data3DStoreData,
        DoubleData,
        DynamicSiteNodesData,
        ElementRefListData,
        EnumOptionsData,
        ExpressionData,
        ExpressionVariableListData,
        FileAndFormatData,
        GeometryListData,
        IntegerData,
        IsotopologueCollectionData,
        IsotopologueListData,
        LinkToKeywordData,
        ModuleData,
        ModuleGroupsData,
        ModuleRefListData,
        NodeData,
        NodeAndIntegerData,
        NodeArrayData,
        NodeBranchData,
        NodeRefListData,
        NodeValueData,
        NodeValueEnumOptionsData,
        PairBroadeningFunctionData,
        ProcedureData,
        RangeData,
        SpeciesData,
        SpeciesRefListData,
        SpeciesSiteData,
        SpeciesSiteRefListData,
        StringData,
        Vec3DoubleData,
        Vec3IntegerData,
        Vec3NodeValueData,
        VectorIntegerDoubleData,
        VectorIntegerStringData,
        WindowFunctionData
    };
    KeywordBase(KeywordDataType type);
    virtual ~KeywordBase();
    // Return DataType name
    static std::string_view keywordDataType(KeywordDataType kdt);

    /*
     * Base Pointer Return
     */
    public:
    // Return base pointer for this (may be overloaded to provide access to other KeywordBase instance)
    virtual KeywordBase *base();

    /*
     * Keyword Description
     */
    public:
    // Keyword Options
    enum KeywordOption
    {
        NoOptions = 0,           /* Keyword has no options set */
        InRestartFileOption = 1, /* Keyword should have its data written to the restart file */
        ModificationRequiresSetUpOption =
            2 /* Modifying the keyword's data requires that the owning object requires setting up */
    };

    private:
    // Data type stored by keyword
    KeywordDataType type_;
    // Keyword name
    std::string name_;
    // Arguments string (for information)
    std::string arguments_;
    // Description of keyword, if any
    std::string description_;
    // Keyword option mask
    int optionMask_;

    protected:
    // Whether the current data value has ever been set
    bool set_;

    public:
    // Set name, description, arguments, and option mask
    void set(std::string_view name, std::string_view description, std::string_view arguments, int optionMask = NoOptions);
    // Return whether data has been set
    bool isSet() const;
    // Flag that data has been set by some other means
    void hasBeenSet();
    // Return data type stored by keyword
    KeywordDataType type() const;
    // Return name of data type stored by keyword
    std::string_view typeName() const;
    // Return keyword name
    std::string_view name() const;
    // Return arguments string
    std::string_view arguments() const;
    // Return keyword description
    std::string_view description() const;
    // Return keyword option mask
    int optionMask() const;
    // Return whether specified option is set
    bool isOptionSet(KeywordOption opt) const;
    // Return whether the data has ever been set
    virtual bool isDataEmpty() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    virtual int minArguments() const = 0;
    // Return maximum number of arguments accepted
    virtual int maxArguments() const = 0;
    // Check number of arguments provided to keyword
    bool validNArgs(int nArgsProvided) const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    virtual bool read(LineParser &parser, int startArg, CoreData &coreData) = 0;
    // Write keyword data to specified LineParser
    virtual bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix = "") = 0;

    /*
     * Parse Result
     */
    public:
    // Keyword Parse Result
    enum ParseResult
    {
        Unrecognised = -1,
        Failed = 0,
        Success = 1
    };

    /*
     * Conversion
     */
    public:
    // Return value (as bool)
    virtual bool asBool();
    // Return value (as int)
    virtual int asInt();
    // Return value (as double)
    virtual double asDouble();
    // Return value (as string)
    virtual std::string asString();
    // Return value as Vec3<int>
    virtual Vec3<int> asVec3Int();
    // Return value as Vec3<double>
    virtual Vec3<double> asVec3Double();

    /*
     * Object Management
     */
    private:
    // References to all keyword objects
    static RefList<KeywordBase> allKeywords_;

    protected:
    // Prune any references to the supplied AtomType in the contained data
    virtual void removeReferencesTo(std::shared_ptr<AtomType> at);
    // Prune any references to the supplied Configuration in the contained data
    virtual void removeReferencesTo(Configuration *cfg);
    // Prune any references to the supplied Isotopologue in the contained data
    virtual void removeReferencesTo(Isotopologue *iso);
    // Prune any references to the supplied Module in the contained data
    virtual void removeReferencesTo(Module *module);
    // Prune any references to the supplied Species in the contained data
    virtual void removeReferencesTo(Species *sp);
    // Prune any references to the supplied SpeciesSite in the contained data
    virtual void removeReferencesTo(SpeciesSite *spSite);
    // Prune any references to the supplied ProcedureNode in the contained data
    virtual void removeReferencesTo(ProcedureNode *node);

    public:
    // Gracefully deal with the specified object no longer being valid
    template <class O> static void objectNoLongerValid(O *object)
    {
        // Loop over all keyword objects and call their local functions
        for (auto kwd : allKeywords_)
            kwd->removeReferencesTo(object);
    }
};
