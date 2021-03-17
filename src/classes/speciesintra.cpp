// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciesintra.h"
#include "base/messenger.h"
#include "classes/masterintra.h"
#include "classes/speciesatom.h"

SpeciesIntra::SpeciesIntra()
{
    masterParameters_ = nullptr;

    attached_[0] = {};
    attached_[1] = {};
    inCycle_ = false;
}

SpeciesIntra::SpeciesIntra(const SpeciesIntra &source) { (*this) = source; }

SpeciesIntra &SpeciesIntra::operator=(const SpeciesIntra &source)
{
    masterParameters_ = source.masterParameters_;
    parameters_.clear();
    parameters_.resize(source.parameters_.size());
    std::copy(source.parameters_.begin(), source.parameters_.end(), parameters_.begin());
    attached_[0] = source.attached_[0];
    attached_[1] = source.attached_[1];
    inCycle_ = source.inCycle_;
    form_ = source.form_;

    return *this;
}

/*
 * Interaction Parameters
 */

// Set linked master from which parameters should be taken
void SpeciesIntra::setMasterParameters(MasterIntra *master) { masterParameters_ = master; }

// Return linked master from which parameters should be taken
const MasterIntra *SpeciesIntra::masterParameters() const { return masterParameters_; }

// Detach from MasterIntra, if we are currently referencing one
void SpeciesIntra::detachFromMasterIntra()
{
    if (!masterParameters_)
        return;

    // Copy master term parameters over our own
    form_ = masterParameters_->form();
    parameters_ = masterParameters_->parameters_;

    masterParameters_ = nullptr;
}

// Set functional form index of interaction
void SpeciesIntra::setForm(int form) { form_ = form; }

// Return functional form index of interaction
int SpeciesIntra::form() const { return masterParameters_ ? masterParameters_->form_ : form_; }

// Add parameter to interaction
void SpeciesIntra::addParameter(double param)
{
    // Does this intramolecular interaction reference a set of master parameters?
    if (masterParameters_)
    {
        Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
        return;
    }

    parameters_.push_back(param);
}

// Set existing parameter
void SpeciesIntra::setParameter(int id, double value)
{
    // Does this intramolecular interaction reference a set of master parameters?
    if (masterParameters_)
    {
        Messenger::error("Refused to set intramolecular parameter since master parameters are referenced.\n");
        return;
    }

    parameters_[id] = value;
}

// Set all parameters
void SpeciesIntra::setParameters(const std::vector<double> &params)
{
    // Does this intramolecular interaction reference a set of master parameters?
    if (masterParameters_)
    {
        Messenger::error("Refused to set intramolecular parameters since master parameters are referenced.\n");
        return;
    }

    parameters_ = params;
}

// Return number of parameters defined
int SpeciesIntra::nParameters() const { return parameters().size(); }

// Return nth parameter
double SpeciesIntra::parameter(int id) const
{
    // Does this intramolecular interaction reference a set of master parameters?
    if (masterParameters_)
        return masterParameters_->parameter(id);

    return parameters_[id];
}

// Return array of parameters
const std::vector<double> &SpeciesIntra::parameters() const
{
    return masterParameters_ ? masterParameters_->parameters() : parameters_;
}

/*
 * Connections
 */

// Set attached SpeciesAtoms for the terminus specified
void SpeciesIntra::setAttachedAtoms(int terminus, const RefList<SpeciesAtom> &atoms)
{
    attached_[terminus].clear();

    // Add the SpeciesAtoms in the list
    for (RefListItem<SpeciesAtom> *refAtom = atoms.first(); refAtom != nullptr; refAtom = refAtom->next())
        attached_[terminus].push_back(refAtom->item()->index());
}

// Set attached SpeciesAtoms for terminus specified (single SpeciesAtom)
void SpeciesIntra::setAttachedAtoms(int terminus, SpeciesAtom *atom)
{
    attached_[terminus].clear();
    attached_[terminus].push_back(atom->index());
}

// Return vector of attached indices for terminus specified
const std::vector<int> &SpeciesIntra::attachedAtoms(int terminus) const { return attached_[terminus]; }

// Set whether the term is contained within a cycle
void SpeciesIntra::setInCycle(bool b) { inCycle_ = b; }

// Return whether the term is contained within a cycle
bool SpeciesIntra::inCycle() const { return inCycle_; }
