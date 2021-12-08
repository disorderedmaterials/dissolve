// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/aliases.h"
#include "templates/anyptr.h"
#include "templates/reflist.h"
#include "templates/vector3.h"
#include <memory>
#include <optional>
#include <typeindex>

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

// Keyword Base Class
class KeywordBase
{
    public:
    KeywordBase(const std::type_index typeIndex);
    virtual ~KeywordBase() = default;

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
    // Type index of derived class
    const std::type_index typeIndex_;
    // Keyword name
    std::string name_;
    // Description of keyword, if any
    std::string description_;
    // Keyword option mask
    int optionMask_{NoOptions};

    protected:
    // Whether the current data value has ever been set
    bool set_{false};

    public:
    // Set base keyword information
    void setBaseInfo(std::string_view name, std::string_view description);
    // Return typeindex for the keyword
    const std::type_index typeIndex() const;
    // Set option mask
    void setOptionMask(int opttionMask);
    // Flag that data has been set by some other means
    void setAsModified();
    // Return keyword name
    std::string_view name() const;
    // Return keyword description
    std::string_view description() const;
    // Return keyword option mask
    int optionMask() const;
    // Return whether specified option is set
    bool isOptionSet(KeywordOption opt) const;
    // Return if the current data object is empty
    virtual bool isDataEmpty() const;
    // Return whether the keyword has been set, and is not currently empty (if relevant)
    bool hasBeenSet() const;
    // Convert to string for display in GUI
    virtual dissolve::any_ptr data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    virtual int minArguments() const;
    // Return maximum number of arguments accepted
    virtual std::optional<int> maxArguments() const;
    // Check number of arguments provided to keyword
    bool validNArgs(int nArgsProvided) const;
    // Deserialise from supplied LineParser, starting at given argument offset
    virtual bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) = 0;
    // Serialise data to specified LineParser
    virtual bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix = "") const = 0;

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
     * Object Management
     */
    private:
    // References to all keyword objects
    static std::vector<KeywordBase *> allKeywords_;

    public:
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
    virtual void removeReferencesTo(NodeRef node);

    public:
    // Gracefully deal with the specified object no longer being valid
    template <class O> static void objectNoLongerValid(O *object)
    {
        // Loop over all keyword objects and call their local functions
        for (auto &kwd : allKeywords_)
            kwd->removeReferencesTo(object);
    }
};
