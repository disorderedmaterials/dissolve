// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesTorsion.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include <map>

// Return enum options for TorsionFunction
EnumOptions<TorsionFunctions::Form> TorsionFunctions::forms()
{
    return EnumOptions<TorsionFunctions::Form>("TorsionFunction",
                                               {{TorsionFunctions::Form::None, "None"},
                                                {TorsionFunctions::Form::Cosine, "Cos", 4},
                                                {TorsionFunctions::Form::Cos3, "Cos3", 3},
                                                {TorsionFunctions::Form::Cos3C, "Cos3C", 4},
                                                {TorsionFunctions::Form::Cos4, "Cos4", 4},
                                                {TorsionFunctions::Form::CosN, "CosN", 1, OptionArguments::AnyNumber},
                                                {TorsionFunctions::Form::CosNC, "CosNC", 1, OptionArguments::AnyNumber},
                                                {TorsionFunctions::Form::UFFCosine, "UFFCosine", 3},
                                                {TorsionFunctions::Form::FourierN, "FourierN", 2, OptionArguments::AnyNumber}});
}

// Return parameters for specified form
const std::vector<std::string> &TorsionFunctions::parameters(Form form)
{
    static std::map<TorsionFunctions::Form, std::vector<std::string>> params_ = {
        {TorsionFunctions::Form::None, {}},
        {TorsionFunctions::Form::Cosine, {"k", "n", "eq", "s"}},
        {TorsionFunctions::Form::Cos3, {"k1", "k2", "k3"}},
        {TorsionFunctions::Form::Cos3C, {"k0", "k1", "k2", "k3"}},
        {TorsionFunctions::Form::Cos4, {"k1", "k2", "k3", "k4"}},
        {TorsionFunctions::Form::UFFCosine, {"k", "n", "eq"}},
        {TorsionFunctions::Form::CosN, {}},
        {TorsionFunctions::Form::CosNC, {}},
        {TorsionFunctions::Form::FourierN, {}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string TorsionFunctions::parameter(Form form, int n)
{
    if (form == TorsionFunctions::Form::CosN)
        return fmt::format("k{}", n + 1);
    else if (form == TorsionFunctions::Form::CosNC)
        return fmt::format("k{}", n);
    else if (form == TorsionFunctions::Form::FourierN)
        return n == 0 ? "k" : fmt::format("C{}", n);
    else
        return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

std::optional<std::pair<std::string_view, int>> parseVariableParameter(std::string_view name)
{
    auto alpha = DissolveSys::beforeChar(name, "0123456789");
    if (!alpha.empty())
    {
        bool isFP = true;
        if (DissolveSys::isNumber(name.substr(alpha.size()), isFP) && !isFP)
            return std::pair<std::string_view, int>(alpha, std::stoi(std::string(name.substr(alpha.size()))));
    }
    return {};
}

// Return index of parameter in the given form
std::optional<int> TorsionFunctions::parameterIndex(Form form, std::string_view name)
{
    if (form == TorsionFunctions::Form::CosN)
    {
        auto ai = parseVariableParameter(name);
        if (ai && ai.value().first == "k" && ai.value().second > 0)
            return ai.value().second - 1;
        else
            return {};
    }
    else if (form == TorsionFunctions::Form::CosNC)
    {
        auto ai = parseVariableParameter(name);
        if (ai && ai.value().first == "k" && ai.value().second >= 0)
            return ai.value().second;
        else
            return {};
    }
    else if (form == TorsionFunctions::Form::FourierN)
    {
        if (name == "k")
            return 0;
        else
        {
            auto ai = parseVariableParameter(name);
            if (ai && ai.value().first == "C" && ai.value().second >= 0)
                return ai.value().second;
            else
                return {};
        }
    }
    else
    {
        auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                               [name](const auto &param) { return DissolveSys::sameString(name, param); });
        if (it == parameters(form).end())
            return {};

        return it - parameters(form).begin();
    }
}

SpeciesTorsion::SpeciesTorsion() : SpeciesIntra(TorsionFunctions::Form::None) {}

SpeciesTorsion::SpeciesTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
    : SpeciesIntra(TorsionFunctions::Form::None)
{
    assign(i, j, k, l);
}

SpeciesTorsion::SpeciesTorsion(SpeciesTorsion &source) : SpeciesIntra(source) { this->operator=(source); }

SpeciesTorsion::SpeciesTorsion(SpeciesTorsion &&source) noexcept : SpeciesIntra(source)
{
    // Detach source torsion referred to by the species atoms
    if (source.i_ && source.j_ && source.k_ && source.l_)
    {
        source.i_->removeTorsion(source);
        source.j_->removeTorsion(source);
        source.k_->removeTorsion(source);
        source.l_->removeTorsion(source);
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

SpeciesTorsion::~SpeciesTorsion() { detach(); }

SpeciesTorsion &SpeciesTorsion::operator=(const SpeciesTorsion &source)
{
    assign(source.i_, source.j_, source.k_, source.l_);
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesTorsion &SpeciesTorsion::operator=(SpeciesTorsion &&source) noexcept
{
    if (i_ && j_ && k_ && l_)
        detach();

    // Copy data
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
void SpeciesTorsion::switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr)
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

// Set Atoms involved in Torsion
void SpeciesTorsion::assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    i_ = i;
    j_ = j;
    k_ = k;
    l_ = l;
    assert(i_ && j_ && k_ && l_);

    i_->addTorsion(*this);
    j_->addTorsion(*this);
    k_->addTorsion(*this);
    l_->addTorsion(*this);
}

// Detach from current atoms
void SpeciesTorsion::detach()
{
    if (i_ && j_ && k_ && l_)
    {
        i_->removeTorsion(*this);
        j_->removeTorsion(*this);
        k_->removeTorsion(*this);
        l_->removeTorsion(*this);
    }
    i_ = nullptr;
    j_ = nullptr;
    k_ = nullptr;
    l_ = nullptr;
}

// Return first SpeciesAtom
SpeciesAtom *SpeciesTorsion::i() const { return i_; }

// Return second SpeciesAtom
SpeciesAtom *SpeciesTorsion::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesTorsion::k() const { return k_; }

// Return fourth SpeciesAtom
SpeciesAtom *SpeciesTorsion::l() const { return l_; }

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesTorsion::atoms() const { return {i_, j_, k_, l_}; }

// Return index (in parent Species) of first SpeciesAtom
int SpeciesTorsion::indexI() const
{
    assert(i_);
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesTorsion::indexJ() const
{
    assert(j_);
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesTorsion::indexK() const
{
    assert(k_);
    return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesTorsion::indexL() const
{
    assert(l_);
    return l_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesTorsion::index(int n) const
{
    if (n == 0)
        return indexI();
    else if (n == 1)
        return indexJ();
    else if (n == 2)
        return indexK();
    else if (n == 3)
        return indexL();

    Messenger::error("SpeciesAtom index {} is out of range in SpeciesTorsion::index(int). Returning 0...\n");
    return 0;
}

// Return whether Atoms in Torsion match those specified
bool SpeciesTorsion::matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k, const SpeciesAtom *l) const
{
    return (i_ == i && j_ == j && k_ == k && l_ == l) || (i_ == l && j_ == k && k_ == j && l_ == i);
}

// Return whether all atoms in the interaction are currently selected
bool SpeciesTorsion::isSelected() const
{
    assert(i_ && j_ && k_ && l_);
    return (i_->isSelected() && j_->isSelected() && k_->isSelected() && l_->isSelected());
}

/*
 * Interaction Parameters
 */

// Set 1-4 scaling factors
bool SpeciesTorsion::set14ScalingFactors(double elecScale, double vdwScale)
{
    if (masterTerm_)
        return Messenger::error("Refused to set 1-4 scaling factors since master parameters are referenced.\n");
    electrostatic14Scaling_ = elecScale;
    vdw14Scaling_ = vdwScale;
    return true;
}

// Set electrostatic 1-4 scaling factor for the interaction
bool SpeciesTorsion::setElectrostatic14Scaling(double scaling)
{
    if (masterTerm_)
        return Messenger::error("Refused to set electrostatic 1-4 scaling factor since master parameters are referenced.\n");
    electrostatic14Scaling_ = scaling;
    return true;
}

// Return electrostatic 1-4 scaling factor for the interaction
double SpeciesTorsion::electrostatic14Scaling() const { return electrostatic14Scaling_; }

// Set van der Waals 1-4 scaling factor for the interaction
bool SpeciesTorsion::setVanDerWaals14Scaling(double scaling)
{
    if (masterTerm_)
        return Messenger::error("Refused to set van der Waals 1-4 scaling factor since master parameters are referenced.\n");
    vdw14Scaling_ = scaling;
    return true;
}

// Return van der Waals 1-4 scaling factor for the interaction
double SpeciesTorsion::vanDerWaals14Scaling() const { return vdw14Scaling_; }

// Return energy for specified angle and functional form, given supplied parameters
double SpeciesTorsion::energy(double angleInDegrees, TorsionFunctions::Form form, const std::vector<double> &params)
{
    // Convert torsion angle from degrees to radians
    const double phi = angleInDegrees / DEGRAD;

    if (form == TorsionFunctions::Form::None)
        return 0.0;
    else if (form == TorsionFunctions::Form::Cosine)
    {
        /*
         * U(phi) = k * (1 + s*cos(n*phi - eq))
         *
         * Parameters:
         * 0 : force constant k
         * 1 : Period 'n'
         * 2 : equilibrium angle (degrees)
         * 3 : Sign 's'
         */
        return params[0] * (1.0 + params[3] * cos(params[1] * phi - (params[2] / DEGRAD)));
    }
    else if (form == TorsionFunctions::Form::Cos3)
    {
        /*
         * U(phi) = 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) )
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : force constant k3
         */
        return 0.5 * (params[0] * (1.0 + cos(phi)) + params[1] * (1.0 - cos(2.0 * phi)) + params[2] * (1.0 + cos(3.0 * phi)));
    }
    else if (form == TorsionFunctions::Form::Cos3C)
    {
        /*
         * U(phi) = k0 + 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) )
         *
         * Parameters:
         * 0 : force constant k0
         * 1 : force constant k1
         * 2 : force constant k2
         * 3 : force constant k3
         */
        return params[0] +
               0.5 * (params[1] * (1.0 + cos(phi)) + params[2] * (1.0 - cos(2.0 * phi)) + params[3] * (1.0 + cos(3.0 * phi)));
    }
    else if (form == TorsionFunctions::Form::Cos4)
    {
        /*
         * U(phi) = 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) + k4*(1-cos(4*phi)) )
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : force constant k3
         * 3 : force constant k4
         */
        return 0.5 * (params[0] * (1.0 + cos(phi)) + params[1] * (1.0 - cos(2.0 * phi)) + params[2] * (1.0 + cos(3.0 * phi)) +
                      params[3] * (1.0 - cos(4.0 * phi)));
    }
    else if (form == TorsionFunctions::Form::CosN)
    {
        /*
         *           1
         * U(phi) = SUM  k(n) * ( 1 + cos( n * phi ) )
         *           n
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : ...
         * n-1 : force constant kn
         */
        auto U = 0.0;
        for (auto n = 0; n < params.size(); ++n)
            U += params[n] * (1.0 + cos((n + 1) * phi));

        return U;
    }
    else if (form == TorsionFunctions::Form::CosNC)
    {
        /*
         *           0
         * U(phi) = SUM  k(n) * ( 1 + cos( n * phi ) )
         *           n
         *
         * Parameters:
         * 0 : force constant k0
         * 1 : force constant k1
         * 2 : ...
         * n : force constant kn
         */
        auto U = 0.0;
        for (auto n = 0; n < params.size(); ++n)
            U += params[n] * (1.0 + cos(n * phi));

        return U;
    }
    else if (form == TorsionFunctions::Form::UFFCosine)
    {
        /*
         * U(phi) = 0.5 * k * (1 - cos(n*eq) * cos(n*phi))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Periodicity, n
         * 2 : Equilibrium angle, eq (degrees)
         */
        return 0.5 * params[0] * (1.0 - cos(params[1] * params[2] / DEGRAD) * cos(params[1] * phi));
    }
    else if (form == TorsionFunctions::Form::FourierN)
    {
        /*
         * U(phi) = k * (C0 + C1 cos(phi) + C2 cos(2*phi) ... Cn cos(n*phi))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1...N : Coefficients C0 - CN
         */
        auto U = params[0] * params[1];
        for (auto n = 2; n < params.size(); ++n)
            U += params[0] * params[n] * cos((n - 1) * phi);

        return U;
    }

    throw(std::runtime_error(fmt::format("Torsion functional form '{}' not accounted for, so can't calculate energy.\n",
                                         TorsionFunctions::forms().keyword(form))));
}

// Return energy for specified angle
double SpeciesTorsion::energy(double angleInDegrees) const
{
    return SpeciesTorsion::energy(angleInDegrees, interactionForm(), interactionParameters());
}

// Return force multiplier for specified angle and functional form, given supplied parameters
double SpeciesTorsion::force(double angleInDegrees, TorsionFunctions::Form form, const std::vector<double> &params)
{
    /*
     * Force of any angle form is given via the chain rule:
     *
     *                 dU    dPhi
     *     F(phi) = - ---- --------
     *                dPhi cos(phi)
     *
     *               dU     1
     *            = ---- --------
     *              dPhi sin(phi)
     */

    // Convert torsion angle from degrees to radians, and calculate derivative w.r.t. change in torsion angle, avoiding step in
    // 1/sin(phi)
    double phi = angleInDegrees / DEGRAD;
    auto sinPhi = sin(phi);
    // TODO Avoid singularities (#542)
    double dphi_dcosphi = -1.0 / DissolveMath::sgn(std::max(1.0e-8, fabs(sinPhi)), sinPhi);

    if (form == TorsionFunctions::Form::None)
        return 0.0;
    else if (form == TorsionFunctions::Form::Cosine)
    {
        /*
         * dU/dphi = -k * n * s * sin(n*phi - eq)
         *
         * Parameters:
         * 0 : Force constant 'k'
         * 1 : Period 'n'
         * 2 : Equilibrium angle (degrees)
         * 3 : Sign 's'
         */

        return params[0] * params[1] * params[3] * sin(params[1] * phi - (params[2] / DEGRAD)) * dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::Cos3)
    {
        /*
         * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) )
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : force constant k3
         */
        return -0.5 * (-params[0] * sin(phi) + 2.0 * params[1] * sin(2.0 * phi) - 3.0 * params[2] * sin(3.0 * phi)) *
               dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::Cos3C)
    {
        /*
         * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) + 4 * k4*(sin(4*phi)))
         *
         * Parameters:
         * 0 : force constant k0
         * 1 : force constant k1
         * 2 : force constant k2
         * 3 : force constant k3
         */
        return -0.5 * (-params[1] * sin(phi) + 2.0 * params[2] * sin(2.0 * phi) - 3.0 * params[3] * sin(3.0 * phi)) *
               dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::Cos4)
    {
        /*
         * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) + 4 * k4*sin(4*phi) )
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : force constant k3
         * 3 : force constant k4
         */
        return -0.5 *
               (-params[0] * sin(phi) + 2.0 * params[1] * sin(2.0 * phi) - 3.0 * params[2] * sin(3.0 * phi) +
                4.0 * params[3] * sin(4.0 * phi)) *
               dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::CosN)
    {
        /*
         *            1
         * dU/dphi = SUM  -k(n) * n * sin( n * phi )
         *            n
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : ...
         * n-1 : force constant kn
         */
        auto dU_dphi = 0.0;
        auto c = 1;
        for (double param : params)
        {
            dU_dphi -= param * (c * sin(c * phi));
            ++c;
        }
        return -dU_dphi * dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::CosNC)
    {
        /*
         *            0
         * dU/dphi = SUM  -k(n) * n * sin( n * phi )
         *            n
         *
         * Parameters:
         * 0 : force constant k0
         * 1 : force constant k1
         * 2 : ...
         * n : force constant kn
         */
        auto dU_dphi = 0.0;
        for (auto n = 1; n < params.size(); ++n)
            dU_dphi -= params[n] * (n * sin(n * phi));

        return -dU_dphi * dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::UFFCosine)
    {
        /*
         * dU/d(phi) = 0.5 * k * cos(n*eq) * n * sin(n*phi)
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Periodicity, n
         * 2 : Equilibrium angle, eq (degrees)
         */

        return -0.5 * params[0] * params[1] * cos(params[1] * params[2] / DEGRAD) * sin(params[1] * phi) * dphi_dcosphi;
    }
    else if (form == TorsionFunctions::Form::FourierN)
    {
        /*
         *            1
         * dU/dphi = SUM  -k * Cn * sin( n * phi )
         *            n
         *
         * Parameters:
         * 0 : Force constant, k
         * 1...N : Coefficients C0 - CN
         */
        auto dU_dphi = 0.0;
        for (auto n = 2; n < params.size(); ++n)
            dU_dphi -= params[0] * params[n] * (n - 1) * sin((n - 1) * phi);

        return -dU_dphi * dphi_dcosphi;
    }

    throw(std::runtime_error(fmt::format("Torsion functional form '{}' not accounted for, so can't calculate force.\n",
                                         TorsionFunctions::forms().keyword(form))));
}

// Return force multiplier for specified angle
double SpeciesTorsion::force(double angleInDegrees) const
{
    return SpeciesTorsion::force(angleInDegrees, interactionForm(), interactionParameters());
}

// Express as a serialisable value
SerialisedValue SpeciesTorsion::serialise() const
{
    auto torsion = SpeciesIntra<SpeciesTorsion, TorsionFunctions>::serialise();
    if (i_ != nullptr)
        torsion["i"] = i_->userIndex();
    if (j_ != nullptr)
        torsion["j"] = j_->userIndex();
    if (k_ != nullptr)
        torsion["k"] = k_->userIndex();
    if (l_ != nullptr)
        torsion["l"] = l_->userIndex();

    torsion["q14"] = electrostatic14Scaling_;
    torsion["v14"] = vdw14Scaling_;

    return torsion;
}

// This method populates the object's members with values read from a 'torsion' TOML node
void SpeciesTorsion::deserialise(const SerialisedValue &node, CoreData &coreData)
{
    deserialiseForm(node, [&coreData](auto &form) { return coreData.getMasterTorsion(form); });

    electrostatic14Scaling_ = toml::find_or<double>(node, "q14", 0.5);

    Serialisable::optionalOn(node, "v14", [this](const auto node) { vdw14Scaling_ = node.as_floating(); });
}
