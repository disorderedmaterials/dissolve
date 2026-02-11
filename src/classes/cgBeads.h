
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors#pragma once 
#pragma once 

#include <vector>

#include "classes/atomType.h"
#include "classes/atomTypeMix.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include "math/data1D.h"

class CGBead 
{
    public:
    CGBead(const std::string &label, const double r) : label_(label), radius_(r)
    {

    };

    ~CGBead() = default;

    void addAtomType(std::shared_ptr<AtomType> atomType, double population, double b)
    {
        std::pair<AtomTypeData &, int> data = atomTypes_.add(atomType, population);
        data.first.setBoundCoherent(b);
        calculateScatteringLength();
    }

    void addAtomType(std::string atomSymbol, double population) 
    {
        const Elements::Element z = Elements::element(atomSymbol);
        double b = Sears91::boundCoherent(Sears91::naturalIsotope(z)) * 0.1;
        std::shared_ptr<AtomType> atomType = std::make_shared<AtomType>(z);
        addAtomType(atomType, population, b);
    }

    void deuterate(const double fraction)
    {
        for (std::size_t i = 0; i < atomTypes_.nItems(); ++i) 
        {
            Elements::Element z = atomTypes_[i].atomType()->Z(); 
            if (z == Elements::H)
            {
                double b = (1.0 - fraction) * atomTypes_[i].boundCoherent();
                b += fraction * Sears91::boundCoherent(Sears91::H_2) * 0.1;
                atomTypes_[i].setBoundCoherent(b);
            }
        }
        calculateScatteringLength();
    }

    void calculateFormFactor(const int formKey, const std::vector<double> &qvals)
    {
        formFactor_.clear();
        for (double q : qvals)
        {
            double qs = q * sigma();
            if (formKey == 0) // gaussian
            {
                double f = -0.5 * std::pow((0.51 * qs), 2);
                f = std::exp(f);
                formFactor_.addPoint(q, f);
            }
            else if (formKey == 1) // uniform 
            {
                double f = 3.0 / std::pow((qs), 3);
                f *= (std::sin(qs) - qs * std::cos(qs));
                formFactor_.addPoint(q, f);
            }
            else // no form factor 
            {
                formFactor_.addPoint(q, 1.0);
            }
        }
        std::cout << "Form Factor for " << label_ << ": ";
        std::cout << std::accumulate(
            formFactor_.values().begin(), formFactor_.values().end(), 0.0
        ) << std::endl;
    };

    int nqvalues() const { return formFactor_.nValues(); }

    void calculateIntraBeadScattering(const double c)
    {
        double innerSum = 0.0;
        for (auto atmI = atomTypes_.begin(); atmI != atomTypes_.end(); ++atmI)
        {
            for (auto atmJ = atmI; atmJ != atomTypes_.end(); ++atmJ)
            {
                const double multiplier = (atmI == atmJ) ? 1.0 : 2.0;
                innerSum += atmI->population() * atmJ->population() * atmI->boundCoherent() * atmJ->boundCoherent() * multiplier;
            }
            innerSum -= (atmI->population() * atmI->boundCoherent() * atmI->boundCoherent());
        }
        innerSum *= c;
        intraBeadSQ_.copyArrays(formFactor_);
        intraBeadSQ_ *= formFactor_.values();
        intraBeadSQ_ *= innerSum * (1.0 / 9.5);
        
        std::cout << "Single Bead Scattering for " << label_ << ": ";
        std::cout << std::accumulate(
            intraBeadSQ_.values().begin(), intraBeadSQ_.values().end(), 0.0
        ) << std::endl;
    };

    const AtomTypeMix &atomTypes() const
    {
        return atomTypes_;
    }

    const std::size_t nAtoms() const 
    { 
        std::size_t natoms = 0;
        for (const auto atm : atomTypes_)
        {
            natoms += atm.population();
        } 
        return natoms;
    }
    
    const Data1D &formFactor() const 
    {
        return formFactor_;
    }

    const Data1D &intraBeadSQ() const 
    {
        return intraBeadSQ_;
    }

    const double &sigma() const { return radius_; }

    const std::string &label() const { return label_; }

    // void finalise() { atomTypes_.finalise(); }

    const double scatteringLength() const 
    {
        return scatteringLength_; 
    }

    const double scatteringLength(const bool deuterated) const
    {
        double nb = 0.0;
        for (const AtomTypeData &atm : atomTypes_)
        {
            if (atm.atomType()->Z() == Elements::H)
            {
                auto iso = deuterated ? Sears91::H_2 : Sears91::H_Natural;
                nb += atm.population() * Sears91::boundCoherent(iso) * 0.1;
            }
            else 
            {
                nb += atm.population() * atm.boundCoherent();
            }
        }
        return nb;
    }

    private:
    double radius_ = 0.0;
    double scatteringLength_ = 0.0;
    std::string label_;
    AtomTypeMix atomTypes_;
    Data1D formFactor_;
    Data1D intraBeadSQ_;

    void calculateScatteringLength()
    {
        scatteringLength_ = 0.0;
        for (const AtomTypeData &atomType : atomTypes_)
        {
            scatteringLength_ += atomType.population() * atomType.boundCoherent();
        }
    }

};

class CGBeadMap 
{
    public:
    CGBeadMap()
    {
    };

    ~CGBeadMap() = default;

    void initialiseFromFile(const double isoFraction) 
    {
        std::ifstream file("bead_definitions.txt");
        if (!file.is_open())
        {
            throw std::runtime_error("Cannot open bead_definitions.txt");
        }

        std::string line;
        while (getline(file, line)) 
        {
            std::stringstream ss(line);
            std::string beadLabel;
            int nAtomTypes = 0;
            double r;
            ss >> beadLabel;
            ss >> nAtomTypes;
            ss >> r;

            CGBead &bead = beads_.emplace_back(CGBead(beadLabel, r));
            
            for (int i = 0; i < nAtomTypes; i++)
            {
                std::string atom;
                int population;
                getline(file, line);
                std::stringstream ssAtom(line);
                ssAtom >> atom;
                ssAtom >> population;

                bead.addAtomType(atom, population);
            }
            if (isoFraction > 0.0) 
            {
                bead.deuterate(isoFraction);
            }
        }
    }

    const std::size_t nBeads() const { return beads_.size(); }

    const CGBead &operator[](const int i) const { return beads_[i]; };

    void calculateFormFactors(const std::vector<double> &qvals)
    {
        dissolve::for_each(
            ParallelPolicies::par,
            beads_.begin(), 
            beads_.end(), 
            [&](CGBead &b){ b.calculateFormFactor(0, qvals); }
        );
    }

    void calculateIntraBeadSQ()
    {
        dissolve::for_each(
            ParallelPolicies::par,
            beads_.begin(), 
            beads_.end(), 
            [](CGBead &b){ b.calculateIntraBeadScattering(0.5); }
            // TODO :: get this to read in the actual concentrations for each bead 
            //      :: does this vary for isotope mixtures?? (exchangeable atoms)
        );
    }

    void deuterate(const double fraction)
    {
        dissolve::for_each(
            ParallelPolicies::par,
            beads_.begin(), 
            beads_.end(), 
            [fraction=fraction](CGBead &b){ b.deuterate(fraction); }
        );
    }
    
    void print() const 
    {
        Messenger::print("CG Bead Representations: \n");
        Messenger::print("     Bead      nb (barn)\n");
        Messenger::print("    --------------------\n");
        for (const CGBead &bead : beads_)
        {
            Messenger::print("      {}     {:6.3f}\n", bead.label(), bead.scatteringLength());
        }
        Messenger::print("    ---------------------\n");
    }

    private:
    std::vector<CGBead> beads_;
};