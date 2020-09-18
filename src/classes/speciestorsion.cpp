/*
    *** SpeciesTorsion Definition
    *** src/classes/speciestorsion.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/speciestorsion.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "templates/enumhelpers.h"

SpeciesTorsion::SpeciesTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) : SpeciesIntra()
{

    parent_ = nullptr;
    i_ = i;
    j_ = j;
    k_ = k;
    l_ = l;
    form_ = SpeciesTorsion::NoForm;

    // Add ourself to the list of bonds on each atom
    if (i_ && j_ && k_ && l_)
    {
        i_->addTorsion(*this, 0.5);
        j_->addTorsion(*this, 0.5);
        k_->addTorsion(*this, 0.5);
        l_->addTorsion(*this, 0.5);
    }
}

SpeciesTorsion::SpeciesTorsion(const SpeciesTorsion &source) { this->operator=(source); }

SpeciesTorsion::~SpeciesTorsion() { detach(); }

SpeciesTorsion &SpeciesTorsion::operator=(const SpeciesTorsion &source)
{
    i_ = source.i_;
    j_ = source.j_;
    k_ = source.k_;
    l_ = source.l_;

    if (i_ && j_ && k_ && l_)
    {
        i_->addTorsion(*this, 0.5);
        j_->addTorsion(*this, 0.5);
        k_->addTorsion(*this, 0.5);
        l_->addTorsion(*this, 0.5);
    }
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesTorsion &SpeciesTorsion::operator=(SpeciesTorsion &&source)
{
    if (i_ && j_ && k_ && l_)
        detach();

    i_ = source.i_;
    j_ = source.j_;
    k_ = source.k_;
    l_ = source.l_;

    if (i_ && j_ && k_ && l_)
    {
        i_->addTorsion(*this, 0.5);
        j_->addTorsion(*this, 0.5);
        k_->addTorsion(*this, 0.5);
        l_->addTorsion(*this, 0.5);
    }
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    return *this;
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

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void SpeciesTorsion::clear()
{
    parent_ = nullptr;
    i_ = nullptr;
    j_ = nullptr;
    k_ = nullptr;
    l_ = nullptr;
    form_ = SpeciesTorsion::NoForm;
}

/*
 * Atom Information
 */

// Set Atoms involved in Torsion
void SpeciesTorsion::assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    i_ = i;
    j_ = j;
    k_ = k;
    l_ = l;
#ifdef CHECKS
    if (i_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesTorsion::set().\n");
    if (j_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesTorsion::set().\n");
    if (k_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesTorsion::set().\n");
    if (l_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* l in SpeciesTorsion::set().\n");
#endif
    if (i_)
        i_->addTorsion(*this, 0.5);
    if (j_)
        j_->addTorsion(*this, 0.5);
    if (k_)
        k_->addTorsion(*this, 0.5);
    if (l_)
        l_->addTorsion(*this, 0.5);
}

// Return first SpeciesAtom
SpeciesAtom *SpeciesTorsion::i() const { return i_; }

// Return second SpeciesAtom
SpeciesAtom *SpeciesTorsion::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesTorsion::k() const { return k_; }

// Return fourth SpeciesAtom
SpeciesAtom *SpeciesTorsion::l() const { return l_; }

// Return index (in parent Species) of first SpeciesAtom
int SpeciesTorsion::indexI() const
{
#ifdef CHECKS
    if (i_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesTorsion::indexI(). Returning 0...\n");
        return 0;
    }
#endif
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesTorsion::indexJ() const
{
#ifdef CHECKS
    if (j_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesTorsion::indexJ(). Returning 0...\n");
        return 0;
    }
#endif
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesTorsion::indexK() const
{
#ifdef CHECKS
    if (k_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesTorsion::indexK(). Returning 0...\n");
        return 0;
    }
#endif
    return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesTorsion::indexL() const
{
#ifdef CHECKS
    if (l_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'l' found in SpeciesTorsion::indexL(). Returning 0...\n");
        return 0;
    }
#endif
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
bool SpeciesTorsion::matches(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
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
bool SpeciesTorsion::isSelected() const
{
#ifdef CHECKS
    if (i_ == nullptr || j_ == nullptr || k_ == nullptr || l_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer found in SpeciesTorsion::isSelected(). Returning false...\n");
        return false;
    }
#endif
    return (i_->isSelected() && j_->isSelected() && k_->isSelected() && l_->isSelected());
}

/*
 * Interaction Parameters
 */

// Return enum options for TorsionFunction
EnumOptions<SpeciesTorsion::TorsionFunction> SpeciesTorsion::torsionFunctions()
{
    static EnumOptionsList TorsionFunctionOptions = EnumOptionsList()
                                                    << EnumOption(SpeciesTorsion::NoForm, "None", 0, 0)
                                                    << EnumOption(SpeciesTorsion::CosineForm, "Cos", 4, 4)
                                                    << EnumOption(SpeciesTorsion::Cos3Form, "Cos3", 3, 3)
                                                    << EnumOption(SpeciesTorsion::Cos3CForm, "Cos3C", 4, 4)
                                                    << EnumOption(SpeciesTorsion::Cos4Form, "Cos4", 4, 4)
                                                    << EnumOption(SpeciesTorsion::CosNForm, "CosN", 1, 10)
                                                    << EnumOption(SpeciesTorsion::CosNCForm, "CosNC", 1, 11)
                                                    << EnumOption(SpeciesTorsion::UFFCosineForm, "UFFCosine", 3, 3);

    static EnumOptions<SpeciesTorsion::TorsionFunction> options("TorsionFunction", TorsionFunctionOptions);

    return options;
}

// Set up any necessary parameters
void SpeciesTorsion::setUp() {}

// Calculate and return fundamental frequency for the interaction
double SpeciesTorsion::fundamentalFrequency(double reducedMass) const
{
    Messenger::warn("No fundamental frequency can be calculated for this torsion interaction.\n");
    return 0.0;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesTorsion::type() const { return SpeciesIntra::TorsionInteraction; }

// Return energy for specified angle
double SpeciesTorsion::energy(double angleInDegrees) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    // Convert torsion angle from degrees to radians
    double phi = angleInDegrees / DEGRAD;

    if (form() == SpeciesTorsion::NoForm)
        return 0.0;
    else if (form() == SpeciesTorsion::CosineForm)
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
    else if (form() == SpeciesTorsion::Cos3Form)
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
    else if (form() == SpeciesTorsion::Cos3CForm)
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
    else if (form() == SpeciesTorsion::Cos4Form)
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
    else if (form() == SpeciesTorsion::CosNForm)
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
        auto result = 0.0;
        for (auto n = 0; n < params.size(); ++n)
            result += params[n] * (1.0 + cos((n + 1) * phi));

        return result;
    }
    else if (form() == SpeciesTorsion::CosNCForm)
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
        auto result = 0.0;
        for (auto n = 0; n < params.size(); ++n)
            result += params[n] * (1.0 + cos(n * phi));

        return result;
    }
    else if (form() == SpeciesTorsion::UFFCosineForm)
    {
        /*
         * U(phi) = 0.5 * V * (1 - cos(n*eq) * cos(n*phi))
         *
         * Parameters:
         * 0 : Force constant, V
         * 1 : Periodicity, n
         * 2 : Equilibrium angle, eq (degrees)
         */
        return 0.5 * params[0] * (1.0 - cos(params[1] * params[2] / DEGRAD) * cos(params[1] * phi));
    }

    Messenger::error("Functional form of SpeciesTorsion term not accounted for, so can't calculate energy.\n");
    return 0.0;
}

// Return force multiplier for specified angle
double SpeciesTorsion::force(double angleInDegrees) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    // Convert torsion angle from degrees to radians, and calculate derivative w.r.t. change in torsion angle
    double phi = angleInDegrees / DEGRAD;
    double dphi_dcosphi = (phi < 1E-8 ? 0.0 : -1.0 / sin(phi));

    if (form() == SpeciesTorsion::NoForm)
        return 0.0;
    else if (form() == SpeciesTorsion::CosineForm)
    {
        /*
         * dU/dphi = k * n * s * -sin(n*phi - eq)
         *
         * Parameters:
         * 0 : Force constant 'k'
         * 1 : Period 'n'
         * 2 : Equilibrium angle (degrees)
         * 3 : Sign 's'
         */
        return dphi_dcosphi * params[1] * params[0] * params[3] * -sin(params[1] * phi - (params[2] / DEGRAD));
    }
    else if (form() == SpeciesTorsion::Cos3Form)
    {
        /*
         * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) )
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : force constant k3
         */
        return dphi_dcosphi * 0.5 *
               (-params[0] * sin(phi) + 2.0 * params[1] * sin(2.0 * phi) - 3.0 * params[2] * sin(3.0 * phi));
    }
    else if (form() == SpeciesTorsion::Cos3CForm)
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
        return dphi_dcosphi * 0.5 *
               (-params[1] * sin(phi) + 2.0 * params[2] * sin(2.0 * phi) - 3.0 * params[3] * sin(3.0 * phi));
    }
    else if (form() == SpeciesTorsion::Cos4Form)
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
        return dphi_dcosphi * 0.5 *
               (-params[0] * sin(phi) + 2.0 * params[1] * sin(2.0 * phi) - 3.0 * params[2] * sin(3.0 * phi) +
                4.0 * params[3] * sin(4.0 * phi));
    }
    else if (form() == SpeciesTorsion::CosNForm)
    {
        /*
         *           1
         * U(phi) = SUM  -k(n) * ( n * sin( n * phi ) )
         *           n
         *
         * Parameters:
         * 0 : force constant k1
         * 1 : force constant k2
         * 2 : ...
         * n-1 : force constant kn
         */
        auto result = 0.0;
        auto c = 1;
        for (auto n = 0; n < params.size(); ++n)
        {
            result -= params[n] * (c * sin(c * phi));
            ++c;
        }
        return dphi_dcosphi * result;
    }
    else if (form() == SpeciesTorsion::CosNCForm)
    {
        /*
         *           0
         * U(phi) = SUM  -k(n) * ( n + sin( n * phi ) )
         *           n
         *
         * Parameters:
         * 0 : force constant k0
         * 1 : force constant k1
         * 2 : ...
         * n : force constant kn
         */
        auto result = 0.0;
        for (auto n = 1; n < params.size(); ++n)
            result -= params[n] * (n * sin(n * phi));

        return dphi_dcosphi * result;
    }
    else if (form() == SpeciesTorsion::UFFCosineForm)
    {
        /*
         * dU/d(phi) = 0.5 * V * cos(n*eq) * n * sin(n*phi)
         *
         * Parameters:
         * 0 : Force constant, V
         * 1 : Periodicity, n
         * 2 : Equilibrium angle, eq (degrees)
         */
        return 0.5 * params[0] * params[0] * cos(params[1] * params[2] / DEGRAD) * params[1] * sin(params[1] * phi);
    }

    Messenger::error("Functional form of SpeciesTorsion term not accounted for, so can't calculate force.\n");
    return 0.0;
}
