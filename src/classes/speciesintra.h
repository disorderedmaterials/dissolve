// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "base/sysfunc.h"
#include "templates/algorithms.h"
#include <string>
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// Base class for intramolecular interactions within Species
template <class Intra, class Functions> class SpeciesIntra
{
    public:
    explicit SpeciesIntra(typename Functions::Form form) : form_(form){};
    virtual ~SpeciesIntra() = default;
    SpeciesIntra(const SpeciesIntra &source) { (*this) = source; }
    SpeciesIntra(SpeciesIntra &&source) = delete;
    SpeciesIntra &operator=(const SpeciesIntra &source)
    {
        form_ = source.form_;
        parameters_.clear();
        parameters_.resize(source.parameters_.size());
        std::copy(source.parameters_.begin(), source.parameters_.end(), parameters_.begin());
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
    // Functional form of interaction
    typename Functions::Form form_;
    // Linked master term from which parameters should be taken (if relevant)
    const Intra *masterTerm_{nullptr};
    // Parameters for interaction
    std::vector<double> parameters_;

    protected:
    // Parse named parameters from string assuming current functional form
    bool parseParameters(std::string params)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
            return Messenger::error("Refused to set intramolecular parameters since master parameters are referenced.\n");

        // First, split the string into individual parameters (either simple values or name=value assignments)
        std::vector<std::string> terms{DissolveSys::splitString(params)};

        // Do we have a suitable number of parameters
        if (!Functions::forms().validNArgs(form(), terms.size()))
            return false;

        // We allow either a set of plain values or a set of name=value assignments - we don't allow mixing of the two
        auto nAssigned =
            std::count_if(terms.begin(), terms.end(), [](const auto &s) { return s.find('=') != std::string::npos; });
        if (nAssigned == 0)
        {
            // Plain values
            parameters_.resize(terms.size(), 0.0);
            std::transform(terms.begin(), terms.end(), parameters_.begin(), [](const auto &term) { return std::stod(term); });
        }
        else if (nAssigned == terms.size())
        {
            // Name = value assignments
            // The parameters may not have been given in the expected order, so maintain/resize a value vector
            parameters_.clear();
            for (const auto &term : terms)
            {
                // Split the string into name and value parts
                auto name = DissolveSys::beforeChar(term, '=');
                if (name.empty())
                    return Messenger::error("Bad assignment found in parameters - no name present in '{}'.\n", term);
                auto value = DissolveSys::afterChar(term, '=');
                if (value.empty())
                    return Messenger::error("Bad assignment found in parameters - no value present in '{}'.\n", term);

                // Get the index of the named parameter
                auto index = Functions::parameterIndex(form(), name);
                if (!index)
                    return Messenger::error(
                        "Bad assignment found in parameters - '{}' is not a valid parameter for this interaction.\n", name);

                // Resize vector if necessary
                if (index.value() >= parameters_.size())
                    parameters_.resize(index.value() + 1, 0.0);
                parameters_[index.value()] = std::stod(std::string(value));
            }
        }
        else
            return Messenger::error(
                "Failed to parse parameters string - provide either plain values or name=value, but don't mix both.\n");

        return true;
    }

    public:
    // Set functional form of interaction
    void setForm(typename Functions::Form form)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
        {
            Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
            return;
        }

        form_ = form;
    }
    // Return functional form of interaction
    typename Functions::Form form() const { return masterTerm_ ? masterTerm_->form_ : form_; }
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
        form_ = masterTerm_->form();
        parameters_ = masterTerm_->parameters_;

        masterTerm_ = nullptr;
    }
    // Add parameter to interaction
    void addParameter(double param)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
        {
            Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
            return;
        }

        parameters_.push_back(param);
    }
    // Set existing parameter
    void setParameter(int id, double value)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
        {
            Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
            return;
        }

        parameters_[id] = value;
    }
    // Set all parameters
    void setParameters(const std::vector<double> &params)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
        {
            Messenger::error("Refused to set intramolecular parameters since master parameters are referenced.\n");
            return;
        }

        parameters_ = params;
    }
    bool setParameters(std::string params)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
            return Messenger::error("Refused to set intramolecular parameters since master parameters are referenced.\n");

        return parseParameters(params);
    }
    // Set form and parameters
    void setFormAndParameters(typename Functions::Form form, const std::vector<double> &params)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
        {
            Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
            return;
        }

        form_ = form;
        parameters_ = params;
    }
    bool setFormAndParameters(typename Functions::Form form, std::string params)
    {
        // Does this intramolecular interaction reference a set of master parameters?
        if (masterTerm_)
            return Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");

        form_ = form;
        return parseParameters(params);
    }
    // Return number of parameters defined
    int nParameters() const { return parameters_.size(); }
    // Return nth parameter
    double parameter(int id) const { return masterTerm_ ? masterTerm_->parameters_[id] : parameters_[id]; }
    // Return array of parameters
    const std::vector<double> &parameters() const { return masterTerm_ ? masterTerm_->parameters_ : parameters_; }
    // Return parameters as name=value string
    std::string parametersAsString() const
    {
        auto id = 0;
        return joinStrings(parameters(), " ",
                           [&](const auto &value) { return fmt::format("{}={}", Functions::parameter(form(), id++), value); });
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
};
