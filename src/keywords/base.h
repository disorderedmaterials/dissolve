// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "math/range.h"
#include "procedure/nodes/aliases.h"
#include "templates/flags.h"
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

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
class KeywordBase : public Serialisable<CoreData const &>
{
    public:
    KeywordBase(const std::type_index typeIndex);
    virtual ~KeywordBase() = default;

    /*
     * Keyword Description
     */
    private:
    // Type index of derived class
    const std::type_index typeIndex_;
    // Keyword name
    std::string name_;
    // Description of keyword, if any
    std::string description_;

    public:
    // Set base keyword information
    void setBaseInfo(std::string_view name, std::string_view description);
    // Return typeindex for the keyword
    const std::type_index typeIndex() const;
    // Set option mask
    void setOptionMask(int optionMask);
    // Return keyword name
    std::string_view name() const;
    // Return keyword description
    std::string_view description() const;

    /*
     * Arguments
     */
    public:
    // Has not changed from initial value
    virtual bool isDefault() const { return false; }
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
    // Express as a serialisable value
    virtual SerialisedValue serialise() const override = 0;
    // Read values from a serialisable value
    virtual void deserialise(const SerialisedValue &node, const CoreData &coreData) override{};

    /*
     * Parse Result
     */
    public:
    // Keyword Parse Result
    enum class ParseResult
    {
        Unrecognised,
        Deprecated,
        Failed,
        Success
    };

    /*
     * GUI Signalling
     */
    public:
    // Keyword Signals
    enum KeywordSignal
    {
        RecreateRenderables,
        ReloadExternalData,
        ClearModuleData,
        RunSetUp
    };

    private:
    // Signals to be emitted (via Qt) when editing this keyword in the GUI
    Flags<KeywordSignal> signals_;

    public:
    // Set signals to be emitted (via Qt) when editing this keyword in the GUI
    void setEditSignals(Flags<KeywordSignal> editSignals);
    // Return signals to be emitted (via Qt) when editing this keyword in the GUI
    Flags<KeywordSignal> editSignals() const;

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
    // Prune any references to the supplied Range in the contained data
    virtual void removeReferencesTo(Range range);

    public:
    // Gracefully deal with the specified object no longer being valid
    template <class O> static void objectNoLongerValid(O *object)
    {
        // Loop over all keyword objects and call their local functions
        for (auto &kwd : allKeywords_)
            kwd->removeReferencesTo(object);
    }
};
