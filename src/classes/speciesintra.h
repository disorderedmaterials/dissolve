// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/serialiser.h"
#include "classes/interactionpotential.h"
#include "templates/algorithms.h"
#include <string>
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// Base class for intramolecular interactions within Species
template <class Intra, class Functions> class SpeciesIntra : public Serialisable
{
    public:
    explicit SpeciesIntra(typename Functions::Form form) : interactionPotential_(form){};
    virtual ~SpeciesIntra() = default;
    SpeciesIntra(const SpeciesIntra &source) : interactionPotential_(source.interactionPotential_.form()) { (*this) = source; }
    SpeciesIntra(SpeciesIntra &&source) = delete;
    SpeciesIntra &operator=(const SpeciesIntra &source)
    {
        interactionPotential_ = source.interactionPotential_;
        masterTerm_ = source.masterTerm_;
        attached_[0] = source.attached_[0];
        attached_[1] = source.attached_[1];
        inCycle_ = source.inCycle_;

        return *this;
    }
    SpeciesIntra &operator=(SpeciesIntra &&source) = delete;

    /*
     * SpeciesAtom Information
     */
    public:
    // Return vector of involved atoms
    virtual std::vector<const SpeciesAtom *> atoms() const = 0;

    /*
     * Interaction Parameters
     */
    protected:
    // Interaction potential describing interaction
    InteractionPotential<Functions> interactionPotential_;
    // Linked master term from which parameters should be taken (if relevant)
    const Intra *masterTerm_{nullptr};

    public:
    // Set functional form of interaction
    void setInteractionForm(typename Functions::Form form)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (!masterTerm_)
            interactionPotential_.setForm(form);
        else
            Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
    }
    // Return functional form of interaction
    typename Functions::Form interactionForm() const
    {
        return masterTerm_ ? masterTerm_->interactionForm() : interactionPotential_.form();
    }
    // Set interaction parameters
    bool setInteractionParameters(std::string params)
    {
        if (masterTerm_)
            return Messenger::error("Refused to set intramolecular parameters since master parameters are referenced.\n");

        std::vector<std::string> terms{DissolveSys::splitString(params)};
        return interactionPotential_.parseParameters(terms);
    }
    bool setInteractionParameters(LineParser &parser, int startArg)
    {
        if (masterTerm_)
            return Messenger::error("Refused to set intramolecular parameters since master parameters are referenced.\n");

        return interactionPotential_.parseParameters(parser, startArg);
    }
    // Set form and parameters
    void setInteractionFormAndParameters(typename Functions::Form form, const std::vector<double> &params)
    {
        if (masterTerm_)
            Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");

        return interactionPotential_.setFormAndParameters(form, params);
    }
    bool setInteractionFormAndParameters(typename Functions::Form form, std::string params)
    {
        if (masterTerm_)
            return Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");

        return interactionPotential_.setFormAndParameters(form, params);
    }
    // Return array of parameters
    const std::vector<double> &interactionParameters() const
    {
        return masterTerm_ ? masterTerm_->interactionPotential_.parameters() : interactionPotential_.parameters();
    }
    // Return interaction potential
    const InteractionPotential<Functions> &interactionPotential() const { return interactionPotential_; }
    // Set linked master from which parameters should be taken
    void setMasterTerm(const Intra *master) { masterTerm_ = master; }
    // Return linked master term from which parameters should be taken
    const Intra *masterTerm() const { return masterTerm_; }
    // Detach from master term, if we are currently referencing one
    void detachFromMasterTerm()
    {
        if (!masterTerm_)
            return;

        // Copy master term parameters over our own
        interactionPotential_.setFormAndParameters(masterTerm_->interactionForm(), masterTerm_->interactionParameters());

        masterTerm_ = nullptr;
    }
    // Calculate and return fundamental frequency for the interaction
    virtual double fundamentalFrequency(double reducedMass) const = 0;

    /*
     * Connections
     */
    private:
    // Number of SpeciesAtoms attached to termini (number of items stored in attached_ arrays)
    std::vector<int> attached_[2];
    // Whether the term is contained within a cycle
    bool inCycle_{false};

    public:
    // Set attached SpeciesAtoms for terminus specified
    void setAttachedAtoms(int terminus, const std::vector<int> &atoms) { attached_[terminus] = atoms; }
    // Set attached SpeciesAtoms for terminus specified (single index)
    void setAttachedAtoms(int terminus, int index)
    {
        attached_[terminus].clear();
        attached_[terminus].push_back(index);
    }
    // Return vector of attached indices for terminus specified
    const std::vector<int> &attachedAtoms(int terminus) const { return attached_[terminus]; }
    // Set whether the term is contained within a cycle
    void setInCycle(bool b) { inCycle_ = b; }
    // Return whether the term is contained within a cycle
    bool inCycle() const { return inCycle_; }

    /*
     * Identifying Name
     */
    public:
    // Set identifying name (if a master term)
    virtual void setName(std::string_view name) { throw(std::runtime_error("Can't set the name of a base SpeciesIntra.\n")); }
    // Return identifying name (if a master term)
    virtual std::string_view name() const { return ""; };
    // Load form from tree node
    template <typename Lambda> void deserialiseForm(SerialisedValue &node, Lambda lambda)
    {
        if (node.contains("form"))
        {
            std::string form = node["form"].as_string();
            if (form.find("@") != std::string::npos)
            {
                auto master = lambda(form);
                if (!master)
                    throw std::runtime_error("Master Torsion not found.");
                setMasterTerm(&master->get());
            }
            else
                setInteractionForm(Functions::forms().enumeration(form));
        }
        if (node.contains("parameters"))
        {
            std::vector<std::string> parameters = Functions::parameters(interactionForm());
            std::vector<double> values;
            for (auto parameter : parameters)
                values.push_back(node["parameters"][parameter].as_floating());
            setInteractionFormAndParameters(interactionForm(), values);
        }
    }
    void deserialise(SerialisedValue &node) override
    {
        if (node.contains("form"))
        {
            std::string form = node["form"].as_string();
            setInteractionForm(Functions::forms().enumeration(form));
        }
        if (node.contains("parameters"))
        {
            std::vector<std::string> parameters = Functions::parameters(interactionForm());
            std::vector<double> values;
            std::transform(parameters.begin(), parameters.end(), std::back_inserter(values),
                           [&node](const auto param) { return node["parameters"][param].as_floating(); });
            setInteractionFormAndParameters(interactionForm(), values);
        }
    }
    SerialisedValue serialise() const override
    {
        SerialisedValue result;

        if (masterTerm_ != nullptr)
            result["form"] = "@" + std::string(masterTerm_->name());
        else
            result["form"] = Functions::forms().keyword(interactionForm());

        std::vector<double> values = interactionPotential().parameters();
        if (!values.empty())
        {
            SerialisedValue parametersNode;
            std::vector<std::string> parameters = Functions::parameters(interactionForm());
            for (int parameterIndex = 0; parameterIndex < values.size(); parameterIndex++)
                parametersNode[parameters[parameterIndex]] = values[parameterIndex];
            result["parameters"] = parametersNode;
        }

        return result;
    }
};
