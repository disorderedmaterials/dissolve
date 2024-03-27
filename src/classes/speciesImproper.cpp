// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesImproper.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include "classes/speciesTorsion.h"

SpeciesImproper::SpeciesImproper() : SpeciesIntra(TorsionFunctions::Form::None) {}

SpeciesImproper::SpeciesImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
    : SpeciesIntra(TorsionFunctions::Form::None)
{
    assign(i, j, k, l);
}

SpeciesImproper::SpeciesImproper(SpeciesImproper &source) : SpeciesIntra(source) { this->operator=(source); }

SpeciesImproper::SpeciesImproper(SpeciesImproper &&source) noexcept : SpeciesIntra(source)
{
    // Detach source torsion referred to by the species atoms
    if (source.i_ && source.j_ && source.k_ && source.l_)
    {
        source.i_->removeImproper(source);
        source.j_->removeImproper(source);
        source.k_->removeImproper(source);
        source.l_->removeImproper(source);
    }

    // Copy data
    assign(source.i_, source.j_, source.k_, source.l_);
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;

    // Reset source data
    source.i_ = nullptr;
    source.j_ = nullptr;
    source.k_ = nullptr;
    source.l_ = nullptr;
}

SpeciesImproper::~SpeciesImproper() { detach(); }

SpeciesImproper &SpeciesImproper::operator=(const SpeciesImproper &source)
{
    assign(source.i_, source.j_, source.k_, source.l_);
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesImproper &SpeciesImproper::operator=(SpeciesImproper &&source) noexcept
{
    if (i_ && j_ && k_ && l_)
        detach();

    assign(source.i_, source.j_, source.k_, source.l_);
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    return *this;
}

/*
 * Atom Information
 */

// Rewrite SpeciesAtom pointer
void SpeciesImproper::switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr)
{
    assert(i_ == oldPtr || j_ == oldPtr || k_ == oldPtr || l_ == oldPtr);

    if (i_ == oldPtr)
        i_ = newPtr;
    else if (j_ == oldPtr)
        j_ = newPtr;
    else if (k_ == oldPtr)
        k_ = newPtr;
    else
        l_ = newPtr;
}

// Set Atoms involved in Improper
void SpeciesImproper::assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    i_ = i;
    j_ = j;
    k_ = k;
    l_ = l;

    assert(i_ && j_ && k_ && l_);

    i_->addImproper(*this);
    j_->addImproper(*this);
    k_->addImproper(*this);
    l_->addImproper(*this);
}

// Detach from current atoms
void SpeciesImproper::detach()
{
    if (i_ && j_ && k_ && l_)
    {
        i_->removeImproper(*this);
        j_->removeImproper(*this);
        k_->removeImproper(*this);
        l_->removeImproper(*this);
    }
    i_ = nullptr;
    j_ = nullptr;
    k_ = nullptr;
    l_ = nullptr;
}

// Return first SpeciesAtom
SpeciesAtom *SpeciesImproper::i() const { return i_; }

// Return second SpeciesAtom
SpeciesAtom *SpeciesImproper::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesImproper::k() const { return k_; }

// Return fourth SpeciesAtom
SpeciesAtom *SpeciesImproper::l() const { return l_; }

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesImproper::atoms() const { return {i_, j_, k_, l_}; }

// Return whether the improper uses the specified SpeciesAtom
bool SpeciesImproper::uses(SpeciesAtom *spAtom) const
{
    return ((i_ == spAtom) || (j_ == spAtom) || (k_ == spAtom) || (l_ == spAtom));
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesImproper::indexI() const
{
    assert(i_);
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesImproper::indexJ() const
{
    assert(j_);
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesImproper::indexK() const
{
    assert(k_);
    return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesImproper::indexL() const
{
    assert(l_);
    return l_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesImproper::index(int n) const
{
    if (n == 0)
        return indexI();
    else if (n == 1)
        return indexJ();
    else if (n == 2)
        return indexK();
    else if (n == 3)
        return indexL();

    Messenger::error("SpeciesAtom index {} is out of range in SpeciesImproper::index(int). Returning 0...\n");
    return 0;
}

// Return whether Atoms in Improper match those specified
bool SpeciesImproper::matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k, const SpeciesAtom *l) const
{
    if (i_ != i)
        return false;
    if (j_ == j)
        return (k_ == k && l_ == l) || (k_ == l && l_ == k);
    else if (j_ == k)
        return (k_ == j && l_ == l) || (k_ == l && l_ == j);
    else if (j_ == l)
        return (k_ == j && l_ == k) || (k_ == k && l_ == j);

    return false;
}

// Return whether all atoms in the interaction are currently selected
bool SpeciesImproper::isSelected() const
{
    assert(i_ && j_ && k_ && l_);
    return (i_->isSelected() && j_->isSelected() && k_->isSelected() && l_->isSelected());
}

/*
 * Interaction Parameters
 */

// Return energy for specified angle
double SpeciesImproper::energy(double angleInDegrees) const
{
    return SpeciesTorsion::energy(angleInDegrees, interactionForm(), interactionParameters());
}

// Return force multiplier for specified angle
double SpeciesImproper::force(double angleInDegrees) const
{
    return SpeciesTorsion::force(angleInDegrees, interactionForm(), interactionParameters());
}

// Express as a serialisable value
SerialisedValue SpeciesImproper::serialise() const
{
    auto improper = SpeciesIntra<SpeciesImproper, TorsionFunctions>::serialise();
    if (i_ != nullptr)
        improper["i"] = i_->userIndex();
    if (j_ != nullptr)
        improper["j"] = j_->userIndex();
    if (k_ != nullptr)
        improper["k"] = k_->userIndex();
    if (l_ != nullptr)
        improper["l"] = l_->userIndex();

    return improper;
}
// This method populates the object's members with values read from an 'improper' TOML node
void SpeciesImproper::deserialise(const SerialisedValue &node, CoreData &coreData)
{
    deserialiseForm(node, [&coreData](auto &form) { return coreData.getMasterImproper(form); });
}
