// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciesangle.h"
#include "classes/speciesatom.h"

SpeciesAngle::SpeciesAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) : SpeciesIntra()
{
    assign(i, j, k);
    form_ = SpeciesAngle::NoForm;
}

SpeciesAngle::SpeciesAngle(SpeciesAngle &source) : SpeciesIntra(source) { this->operator=(source); }

SpeciesAngle::SpeciesAngle(SpeciesAngle &&source) : SpeciesIntra(source)
{
    // Detach source angle referred to by the species atoms
    if (source.i_ && source.j_ && source.k_)
    {
        source.i_->removeAngle(source);
        source.j_->removeAngle(source);
        source.k_->removeAngle(source);
    }

    // Copy data
    assign(source.i_, source.j_, source.k_);
    form_ = source.form_;

    // Reset source data
    source.i_ = nullptr;
    source.j_ = nullptr;
    source.k_ = nullptr;
}

SpeciesAngle &SpeciesAngle::operator=(SpeciesAngle &source)
{
    // Copy data
    assign(source.i_, source.j_, source.k_);
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesAngle &SpeciesAngle::operator=(SpeciesAngle &&source)
{
    // Detach any current atoms
    if (i_ && j_ && k_)
        detach();

    // Copy data
    assign(source.i_, source.j_, source.k_);
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    // Clean source
    source.detach();

    return *this;
}

/*
 * Atom Information
 */

// Assign the three atoms in the angle
void SpeciesAngle::assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k)
{
    i_ = i;
    j_ = j;
    k_ = k;
    assert(i_ && j_ && k_);

    i_->addAngle(*this);
    j_->addAngle(*this);
    k_->addAngle(*this);
}

// Return first SpeciesAtom
SpeciesAtom *SpeciesAngle::i() const { return i_; }

// Return second (central) SpeciesAtom
SpeciesAtom *SpeciesAngle::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesAngle::k() const { return k_; }

// Return index (in parent Species) of first SpeciesAtom
int SpeciesAngle::indexI() const
{
    assert(i_);
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesAngle::indexJ() const
{
    assert(j_);
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesAngle::indexK() const
{
    assert(k_);
    return k_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesAngle::index(int n) const
{
    if (n == 0)
        return indexI();
    else if (n == 1)
        return indexJ();
    else if (n == 2)
        return indexK();

    Messenger::error("SpeciesAtom index {} is out of range in SpeciesAngle::index(int). Returning 0...\n");
    return 0;
}

// Return whether Atoms in Angle match those specified
bool SpeciesAngle::matches(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const
{
    if (j_ != j)
        return false;
    if ((i_ == i) && (k_ == k))
        return true;
    if ((i_ == k) && (k_ == i))
        return true;
    return false;
}

// Return whether all atoms in the interaction are currently selected
bool SpeciesAngle::isSelected() const
{
    assert(i_ && j_ && k_);
    return (i_->isSelected() && j_->isSelected() && k_->isSelected());
}

// Detach from current atoms
void SpeciesAngle::detach()
{
    if (i_ && j_ && k_)
    {
        i_->removeAngle(*this);
        j_->removeAngle(*this);
        k_->removeAngle(*this);
    }
    i_ = nullptr;
    j_ = nullptr;
    k_ = nullptr;
}

/*
 * Interaction Parameters
 */

// Return enum options for AngleFunction
EnumOptions<SpeciesAngle::AngleFunction> SpeciesAngle::angleFunctions()
{
    return EnumOptions<SpeciesAngle::AngleFunction>("AngleFunction", {{SpeciesAngle::NoForm, "None"},
                                                                      {SpeciesAngle::HarmonicForm, "Harmonic", 2},
                                                                      {SpeciesAngle::CosineForm, "Cos", 4},
                                                                      {SpeciesAngle::Cos2Form, "Cos2", 4}});
}

// Set up any necessary parameters
void SpeciesAngle::setUp() {}

// Calculate and return fundamental frequency for the interaction
double SpeciesAngle::fundamentalFrequency(double reducedMass) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    double k = 0.0;
    if (form() == SpeciesAngle::HarmonicForm)
        k = params[0];
    else
    {
        Messenger::error("Functional form of SpeciesAngle term not set, or no force constant available, so can't "
                         "determine fundamental frequency.\n");
        return 0.0;
    }

    // Convert force constant from (assumed) kJ mol-1 A-2 into J m-2 (kg s-2)
    k *= 1000.0 * 1.0e20 / AVOGADRO;

    // Convert reduced mass from amu to kg
    double mu = reducedMass / (AVOGADRO * 1000.0);

    // Calculate fundamental frequency
    double v = (1.0 / TWOPI) * sqrt(k / mu);

    return v;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesAngle::type() const { return SpeciesIntra::InteractionType::Angle; }

// Return energy for specified angle
double SpeciesAngle::energy(double angleInDegrees) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    if (form() == SpeciesAngle::NoForm)
        return 0.0;
    else if (form() == SpeciesAngle::HarmonicForm)
    {
        /*
         * U(theta) = 0.5 * forcek * (theta - eq)**2
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Equilibrium angle, eq (degrees)
         */
        const auto delta = (angleInDegrees - params[1]) / DEGRAD;
        return 0.5 * params[0] * delta * delta;
    }
    else if (form() == SpeciesAngle::CosineForm)
    {
        /*
         * U(theta) = forcek * (1 + s * cos(n*theta - eq))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Periodicity, N
         * 2 : Equilibrium angle, eq (degrees)
         * 3 : Sign, s
         */
        return params[0] * (1.0 + params[3] * cos(params[1] * angleInDegrees / DEGRAD - params[2] / DEGRAD));
    }
    else if (form() == SpeciesAngle::Cos2Form)
    {
        /*
         * U(theta) = forcek * (C0 + C1 * cos(theta) + C2 * cos(2*theta))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Constant C0
         * 2 : Constant C1
         * 3 : Constant C2
         */
        const auto angleInRadians = angleInDegrees / DEGRAD;
        return params[0] * (params[1] + params[2] * cos(angleInRadians) + params[3] * cos(2.0 * angleInRadians));
    }

    Messenger::error("Functional form of SpeciesAngle term not accounted for, so can't calculate energy.\n");
    return 0.0;
}

// Return force multiplier for specified angle
double SpeciesAngle::force(double angleInDegrees) const
{
    /*
     * Force of any angle form is given via the chain rule:
     *
     *                    dU     dTheta
     *     F(theta) = - ------ ----------
     *                  dTheta cos(theta)
     *
     *                  dU       1
     *              = ------ ---------
     *                dTheta sin(theta)
     */

    // Get pointer to relevant parameters array
    const auto &params = parameters();

    // Convert angle to radians
    const auto angleInRadians = angleInDegrees / DEGRAD;

    if (form() == SpeciesAngle::NoForm)
        return 0.0;
    else if (form() == SpeciesAngle::HarmonicForm)
    {
        /*
         * dU/dTheta = k * (theta - eq)
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Equilibrium angle, eq (degrees)
         */

        return params[0] * ((angleInDegrees - params[1]) / DEGRAD) / sin(angleInRadians);
    }
    else if (form() == SpeciesAngle::CosineForm)
    {
        /*
         * dU/dTheta = -k * n * s * sin(n*theta - eq)
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Periodicity, N
         * 2 : Equilibrium angle, eq (degrees)
         * 3 : Sign, s
         */

        return -params[0] * params[1] * params[3] * sin(params[1] * angleInRadians - params[2] / DEGRAD) / sin(angleInRadians);
    }
    else if (form() == SpeciesAngle::Cos2Form)
    {
        /*
         * dU/dTheta = -k * (c1 * sin(theta) + 2 * c2 * sin(2*theta))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Constant C0 (unused)
         * 2 : Constant C1
         * 3 : Constant C2
         */

        return -params[0] * (params[2] * sin(angleInRadians) + 2.0 * params[3] * sin(2.0 * angleInRadians)) /
               sin(angleInRadians);
    }

    Messenger::error("Functional form of SpeciesAngle term not accounted for, so can't calculate force.\n");
    return 0.0;
}
