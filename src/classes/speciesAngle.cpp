// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesAngle.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include <map>

// Return enum options for AngleFunction
EnumOptions<AngleFunctions::Form> AngleFunctions::forms()
{
    return EnumOptions<AngleFunctions::Form>("AngleFunction", {{AngleFunctions::Form::None, "None"},
                                                               {AngleFunctions::Form::Harmonic, "Harmonic", 2},
                                                               {AngleFunctions::Form::Cosine, "Cos", 4},
                                                               {AngleFunctions::Form::Cos2, "Cos2", 4}});
}

// Return parameters for specified form
const std::vector<std::string> &AngleFunctions::parameters(Form form)
{
    static std::map<AngleFunctions::Form, std::vector<std::string>> params_ = {
        {AngleFunctions::Form::None, {}},
        {AngleFunctions::Form::Harmonic, {"k", "eq"}},
        {AngleFunctions::Form::Cosine, {"k", "n", "eq", "s"}},
        {AngleFunctions::Form::Cos2, {"k", "C0", "C1", "C2"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string AngleFunctions::parameter(Form form, int n) { return DissolveSys::stringAt(parameters(form), n); }

// Return index of parameter in the given form
std::optional<int> AngleFunctions::parameterIndex(Form form, std::string_view name)
{
    return DissolveSys::indexOf(parameters(form), name);
}

SpeciesAngle::SpeciesAngle() : SpeciesIntra(AngleFunctions::Form::None) {}

SpeciesAngle::SpeciesAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) : SpeciesIntra(AngleFunctions::Form::None)
{
    assign(i, j, k);
}

SpeciesAngle::SpeciesAngle(SpeciesAngle &source) : SpeciesIntra(source) { this->operator=(source); }

SpeciesAngle::SpeciesAngle(SpeciesAngle &&source) noexcept : SpeciesIntra(source)
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
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;

    // Reset source data
    source.i_ = nullptr;
    source.j_ = nullptr;
    source.k_ = nullptr;
}

SpeciesAngle &SpeciesAngle::operator=(const SpeciesAngle &source)
{
    // Copy data
    assign(source.i_, source.j_, source.k_);
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesAngle &SpeciesAngle::operator=(SpeciesAngle &&source) noexcept
{
    // Detach any current atoms
    if (i_ && j_ && k_)
        detach();

    // Copy data
    assign(source.i_, source.j_, source.k_);
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    // Clean source
    source.detach();

    return *this;
}

/*
 * Atom Information
 */

// Rewrite SpeciesAtom pointer
void SpeciesAngle::switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr)
{
    assert(i_ == oldPtr || j_ == oldPtr || k_ == oldPtr);

    if (i_ == oldPtr)
        i_ = newPtr;
    else if (j_ == oldPtr)
        j_ = newPtr;
    else
        k_ = newPtr;
}

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

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesAngle::atoms() const { return {i_, j_, k_}; }

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
bool SpeciesAngle::matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k) const
{
    return (j_ == j) && ((i_ == i && k_ == k) || (i_ == k && k_ == i));
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

// Calculate and return fundamental frequency for the interaction
double SpeciesAngle::fundamentalFrequency(double reducedMass) const
{
    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto angleForm = interactionForm();

    double k = 0.0;
    if (angleForm == AngleFunctions::Form::Harmonic)
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

// Return energy for specified angle
double SpeciesAngle::energy(double angleInDegrees) const
{
    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto angleForm = interactionForm();

    if (angleForm == AngleFunctions::Form::None)
        return 0.0;
    else if (angleForm == AngleFunctions::Form::Harmonic)
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
    else if (angleForm == AngleFunctions::Form::Cosine)
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
    else if (angleForm == AngleFunctions::Form::Cos2)
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

    throw(std::runtime_error(fmt::format("Angle functional form '{}' not accounted for, so can't calculate energy.\n",
                                         AngleFunctions::forms().keyword(angleForm))));
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
    const auto &params = interactionParameters();
    const auto angleForm = interactionForm();

    // Convert angle to radians
    const auto angleInRadians = angleInDegrees / DEGRAD;

    if (angleForm == AngleFunctions::Form::None)
        return 0.0;
    else if (angleForm == AngleFunctions::Form::Harmonic)
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
    else if (angleForm == AngleFunctions::Form::Cosine)
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
    else if (angleForm == AngleFunctions::Form::Cos2)
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

    throw(std::runtime_error(fmt::format("Angle functional form '{}' not accounted for, so can't calculate force.\n",
                                         AngleFunctions::forms().keyword(angleForm))));
}

// Express as a serialisable value
SerialisedValue SpeciesAngle::serialise() const
{
    auto angle = SpeciesIntra<SpeciesAngle, AngleFunctions>::serialise();
    if (i_ != nullptr)
        angle["i"] = i_->userIndex();
    if (j_ != nullptr)
        angle["j"] = j_->userIndex();
    if (k_ != nullptr)
        angle["k"] = k_->userIndex();

    return angle;
}
// This method populates the object's members with values read from an 'angle' TOML node
void SpeciesAngle::deserialise(const SerialisedValue &node, CoreData &coreData)
{
    deserialiseForm(node, [&coreData](auto &form) { return coreData.getMasterAngle(form); });
}
