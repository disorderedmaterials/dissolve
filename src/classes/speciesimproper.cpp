// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/speciesimproper.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "classes/speciestorsion.h"
#include "templates/enumhelpers.h"

SpeciesImproper::SpeciesImproper() : SpeciesIntra() { clear(); }

SpeciesImproper::SpeciesImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
    : SpeciesIntra(), i_(i), j_(j), k_(k), l_(l)
{
}

SpeciesImproper::SpeciesImproper(SpeciesImproper &&source) : SpeciesIntra(source)
{
    i_ = source.i_;
    j_ = source.j_;
    k_ = source.k_;
    l_ = source.l_;
    form_ = source.form_;

    source.i_ = nullptr;
    source.j_ = nullptr;
    source.k_ = nullptr;
    source.l_ = nullptr;
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void SpeciesImproper::clear()
{
    parent_ = nullptr;
    i_ = nullptr;
    j_ = nullptr;
    k_ = nullptr;
    l_ = nullptr;
    form_ = SpeciesImproper::NoForm;
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
#ifdef CHECKS
    if (i_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesImproper::set().\n");
    if (j_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesImproper::set().\n");
    if (k_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesImproper::set().\n");
    if (l_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* l in SpeciesImproper::set().\n");
#endif

    if (i_)
        i_->addImproper(*this);
    if (j_)
        j_->addImproper(*this);
    if (k_)
        k_->addImproper(*this);
    if (l_)
        l_->addImproper(*this);
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
#ifdef CHECKS
    if (i_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesImproper::indexI(). Returning 0...\n");
        return 0;
    }
#endif
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesImproper::indexJ() const
{
#ifdef CHECKS
    if (j_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesImproper::indexJ(). Returning 0...\n");
        return 0;
    }
#endif
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesImproper::indexK() const
{
#ifdef CHECKS
    if (k_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesImproper::indexK(). Returning 0...\n");
        return 0;
    }
#endif
    return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesImproper::indexL() const
{
#ifdef CHECKS
    if (l_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'l' found in SpeciesImproper::indexL(). Returning 0...\n");
        return 0;
    }
#endif
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
#ifdef CHECKS
    if (i_ == nullptr || j_ == nullptr || k_ == nullptr || l_ == nullptr)
    {
        Messenger::error(
            "NULL_POINTER - NULL SpeciesAtom pointer found in SpeciesImproper::isSelected(). Returning false...\n");
        return false;
    }
#endif
    return (i_->isSelected() && j_->isSelected() && k_->isSelected() && l_->isSelected());
}

/*
 * Interaction Parameters
 */

// Return enum options for ImproperFunction
EnumOptions<SpeciesImproper::ImproperFunction> SpeciesImproper::improperFunctions()
{
    static EnumOptionsList ImproperFunctionOptions = EnumOptionsList()
                                                     << EnumOption(SpeciesTorsion::NoForm, "None", 0, 0)
                                                     << EnumOption(SpeciesTorsion::CosineForm, "Cos", 4, 4)
                                                     << EnumOption(SpeciesTorsion::Cos3Form, "Cos3", 3, 3)
                                                     << EnumOption(SpeciesTorsion::Cos3CForm, "Cos3C", 4, 4)
                                                     << EnumOption(SpeciesTorsion::Cos4Form, "Cos4", 4, 4)
                                                     << EnumOption(SpeciesTorsion::CosNForm, "CosN", 1, 10)
                                                     << EnumOption(SpeciesTorsion::CosNCForm, "CosNC", 1, 11)
                                                     << EnumOption(SpeciesTorsion::UFFCosineForm, "UFFCosine", 3, 3);

    static EnumOptions<SpeciesImproper::ImproperFunction> options("ImproperFunction", ImproperFunctionOptions);

    return options;
}

// Set up any necessary parameters
void SpeciesImproper::setUp() {}

// Calculate and return fundamental frequency for the interaction
double SpeciesImproper::fundamentalFrequency(double reducedMass) const
{
    Messenger::warn("No fundamental frequency can be calculated for this improper interaction.\n");
    return 0.0;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesImproper::type() const { return SpeciesIntra::ImproperInteraction; }

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
