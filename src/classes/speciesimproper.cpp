// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciesimproper.h"
#include "classes/speciesatom.h"
#include "classes/speciestorsion.h"

SpeciesImproper::SpeciesImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) : SpeciesIntra()
{
    assign(i, j, k, l);
    form_ = SpeciesTorsion::NoForm;
}

SpeciesImproper::SpeciesImproper(SpeciesImproper &source) { this->operator=(source); }

SpeciesImproper::SpeciesImproper(SpeciesImproper &&source) : SpeciesIntra(source)
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
    form_ = source.form_;

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
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesImproper &SpeciesImproper::operator=(SpeciesImproper &&source)
{
    if (i_ && j_ && k_ && l_)
        detach();

    assign(source.i_, source.j_, source.k_, source.l_);
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    return *this;
}

/*
 * Atom Information
 */

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
bool SpeciesImproper::matches(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    if (i_ == i)
    {
        if (j_ != j)
            return false;
        if (k_ != k)
            return false;
        if (l_ == l)
            return true;
    }
    else if (i_ == l)
    {
        if (j_ != k)
            return false;
        if (k_ != j)
            return false;
        if (l_ == i)
            return true;
    }

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

// Set up any necessary parameters
void SpeciesImproper::setUp() {}

// Calculate and return fundamental frequency for the interaction
double SpeciesImproper::fundamentalFrequency(double reducedMass) const
{
    Messenger::warn("No fundamental frequency can be calculated for this improper interaction.\n");
    return 0.0;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesImproper::type() const { return SpeciesIntra::InteractionType::Improper; }

// Return energy for specified angle
double SpeciesImproper::energy(double angleInDegrees) const
{
    return SpeciesTorsion::energy(angleInDegrees, form(), parameters());
}

// Return force multiplier for specified angle
double SpeciesImproper::force(double angleInDegrees) const
{
    return SpeciesTorsion::force(angleInDegrees, form(), parameters());
}
