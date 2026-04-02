// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors#pragma once 
#pragma once 

#include <numeric>
#include <string>
#include <vector>

#include "classes/atomType.h"
#include "classes/atomTypeMix.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include "data/formFactors.h"
#include "math/data1D.h"

class CGBead 
{
    public:
    CGBead(const std::string &label, const double r, const int atomDistribution)
        : label_(label), radius_(r), atomDistribution_(atomDistribution)
    {

    };

    ~CGBead() = default;

    CGBead(const CGBead &source) { (*this) = source; }

    void operator=(const CGBead& source)
    { 
        label_ = source.label_;
        radius_ = source.radius_;
        atomDistribution_ = source.atomDistribution_;
        scatteringLength_ = source.scatteringLength_;
        dFraction_ = source.dFraction_;
        atomTypes_ = source.atomTypes_;
        formFactor_ = source.formFactor_;
        intraBeadSQ_ = source.intraBeadSQ_;

    }

    void addAtomType(std::shared_ptr<AtomType> atomType, const double population, const double b)
    {
        std::pair<AtomTypeData &, int> data = atomTypes_.add(atomType, population);
        data.first.setBoundCoherent(b);
    }

    void addAtomType(const std::string &atomSymbol, const double population) 
    {
        const Elements::Element z = Elements::element(atomSymbol);
        double b = Sears91::boundCoherent(Sears91::naturalIsotope(z)) * 0.1;
        std::shared_ptr<AtomType> atomType = std::make_shared<AtomType>(z);
        addAtomType(atomType, population, b);
    }

    void calculateScatteringLength()
    {
        scatteringLength_ = 0.0;
        for (const AtomTypeData &atomType : atomTypes_)
        {
            scatteringLength_ += atomType.population() * atomType.boundCoherent();
        }
    }

    void deuterate(const double fraction)
    {
        dFraction_ = fraction;
        for (std::size_t i = 0; i < atomTypes_.nItems(); ++i) 
        {
            Elements::Element z = atomTypes_[i].atomType()->Z(); 
            if (z == Elements::H)
            {
                double b = (1.0 - fraction) * Sears91::boundCoherent(Sears91::H_Natural);
                b += fraction * Sears91::boundCoherent(Sears91::H_2);
                b *= 0.1;
                atomTypes_[i].setBoundCoherent(b);
            }
        }
        calculateScatteringLength();
    }

    void calculateFormFactor(const std::vector<double> &qvals)
    {
        formFactor_.clear();
        for (double q : qvals)
        {
            double qs = q * sigma();
            if (atomDistribution_ == 0) // gaussian
            {
                double f = -0.5 * std::pow((0.51 * qs), 2);
                f = std::exp(f);
                formFactor_.addPoint(q, f);
            }
            else if (atomDistribution_ == 1) // uniform 
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
    };

    int nqvalues() const { return formFactor_.nValues(); }

    void calculateSelfScattrering(const double c, const double av_noa_bead, const bool exchangeable)
    {
        double innerSum = 0.0;
        bool deuterated = (dFraction_ > 0.0 && dFraction_ < 1.0 && !exchangeable);
        for (auto atmI = atomTypes_.begin(); atmI != atomTypes_.end(); ++atmI)
        {
            for (auto atmJ = atmI; atmJ != atomTypes_.end(); ++atmJ)
            {
                const double multiplier = (atmI == atmJ) ? 1.0 : 2.0;
                double nb = 0.0;
                if (deuterated && (atmI->atomType()->Z() == Elements::H || atmJ->atomType()->Z() == Elements::H))
                {
                    if (atmI == atmJ)
                    {
                        nb = dFraction_ * Sears91::boundCoherent(Sears91::H_2) * Sears91::boundCoherent(Sears91::H_2);
                        nb += (1.0 - dFraction_) * Sears91::boundCoherent(Sears91::H_Natural) * Sears91::boundCoherent(Sears91::H_Natural);
                        nb *= 0.1 * 0.1; // convert to barn
                    }
                    else
                    {
                        if (atmI->atomType()->Z() == Elements::H)
                        {
                            nb = dFraction_ * atmJ->boundCoherent() * Sears91::boundCoherent(Sears91::H_2);
                            nb += (1.0 - dFraction_) * atmJ->boundCoherent() * Sears91::boundCoherent(Sears91::H_Natural);
                            nb *= 0.1; // convert to barn
                        }
                        else
                        {
                            nb = dFraction_ * atmI->boundCoherent() * Sears91::boundCoherent(Sears91::H_2);
                            nb += (1.0 - dFraction_) * atmI->boundCoherent() * Sears91::boundCoherent(Sears91::H_Natural);
                            nb *= 0.1; // convert to barn
                        }
                    }
                    
                }
                else
                {
                    nb = atmI->boundCoherent() * atmJ->boundCoherent();
                }
                innerSum += atmI->population() * atmJ->population() * nb * multiplier;
            }
            if (deuterated && atmI->atomType()->Z() == Elements::H) 
            {
                double bh = Sears91::boundCoherent(Sears91::H_Natural) * 0.1;
                double bd = Sears91::boundCoherent(Sears91::H_2) * 0.1;
                innerSum -= atmI->population() * ((dFraction_ * bd * bd) + ((1.0 - dFraction_) * bh * bh)); 
            }
            else
            {
                innerSum -= (atmI->population() * atmI->boundCoherent() * atmI->boundCoherent());
            }
        }
        innerSum *= c;
        intraBeadSQ_.copyArrays(formFactor_);
        intraBeadSQ_ *= formFactor_.values();
        intraBeadSQ_ *= innerSum * (1.0 / av_noa_bead);

    };

    void calculateXRaySelfScattrering(const double c, const double av_noa_bead)
    {
        intraBeadSQ_.copyArrays(formFactor_);
        intraBeadSQ_ *= formFactor_.values();
        std::vector<double> innerSum(intraBeadSQ_.nValues(), 0.0);

        std::transform(intraBeadSQ_.xAxis().begin(), intraBeadSQ_.xAxis().end(), innerSum.begin(),
                       [&](const double q)
                       {
                           double innerSumq = 0.0;
                           for (auto i = 0; i < atomTypes_.nItems(); ++i)
                           {
                               const double nbi = atomTypes_[i].population() * xRayFormFactorData_[i].get().magnitude(q);
                               for (auto j = i; j < atomTypes_.nItems(); ++j)
                               {
                                   const double multiplier = (i == j) ? 1.0 : 2.0;
                                   const double nbj = atomTypes_[j].population() * xRayFormFactorData_[j].get().magnitude(q);
                                   innerSumq += nbi * nbj * multiplier;
                               }
                               innerSumq -= nbi;
                           }
                           return innerSumq * (c / av_noa_bead);
                       });
        intraBeadSQ_ *= innerSum;
    }

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

    const double nonExchangeScatteringLength(const bool deuterated) const
    { 
        const bool mix = (dFraction_ > 0.0 && dFraction_ < 1.0);
        double nb = 0.0;
        if (mix)
        {
            for (const AtomTypeData& atomType : atomTypes_)
            {
                if (atomType.atomType()->Z() == Elements::H)
                {
                    if (deuterated)
                    {
                        nb += atomType.population() * Sears91::boundCoherent(Sears91::H_2) * 0.1;
                    }
                    else
                    {
                        nb += atomType.population() * Sears91::boundCoherent(Sears91::H_Natural) * 0.1;
                    }
                }
                else
                {
                    nb += atomType.population() * atomType.boundCoherent();
                }
            }
        }
        else
        {
            nb = scatteringLength();
        }
        return nb;
    }

    const char* atomDistribution() const
    {
        if (atomDistribution_ == 0)
        {
            return "Gaussian";
        }
        else if (atomDistribution_ == 1)
        {
            return "Uniform";
        }
        return "None";
    }

    bool initialiseXRayFormFactors()
    { 
        xRayFormFactorData_.clear();
        for (auto& atm : atomTypes_)
        {
            auto at = atm.atomType();
            auto data = XRayFormFactors::formFactorData(XRayFormFactors::WaasmaierKirfel1995, at->Z());
            if (!data)
            {
                return Messenger::error(
                    "No form factor data present for element {} (formal charge {}) in x-ray data set '{}'.\n",
                    Elements::symbol(at->Z()), 0,
                    XRayFormFactors::xRayFormFactorData().keyword(XRayFormFactors::WaasmaierKirfel1995));
            }
            xRayFormFactorData_.push_back(*data);
        }
        return true;
    }

    double xRayFormFactorMagnitude(const double q) const
    {
        double fq = 0.0;
        for (auto i = 0; i < atomTypes_.nItems(); ++i)
        {
            fq += atomTypes_[i].population() * xRayFormFactorData_[i].get().magnitude(q);
        }
        return fq;
    }

    private:
    int atomDistribution_ = 0;
    double radius_ = 0.0;
    double scatteringLength_ = 0.0;
    double dFraction_ = 0.0;
    std::string label_;
    AtomTypeMix atomTypes_;
    // Form factor data for atom types
    std::vector<std::reference_wrapper<const FormFactorData>> xRayFormFactorData_;
    Data1D formFactor_;
    Data1D intraBeadSQ_;

};

class CGBeadMap 
{
    public:
    CGBeadMap()
    {
    };

    CGBeadMap(const CGBeadMap &source) { (*this) = source; }

    void operator=(const CGBeadMap &source)
    {
        beads_ = source.beads_;
        av_noa_bead_ = source.av_noa_bead_;
    }

    ~CGBeadMap() = default;

    void initialiseFromFile(const std::string &filename = "bead_definitions.txt") 
    {
        std::ifstream file(filename);
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
            int atomDistribution = 0;
            ss >> beadLabel;
            ss >> nAtomTypes;
            ss >> r;
            ss >> atomDistribution; // 0 = gaussian, 1 = uniform 

            CGBead &bead = beads_.emplace_back(CGBead(beadLabel, r, atomDistribution));
            
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

            bead.calculateScatteringLength();
        }
    }

    bool initialiseFormFactors()
    {
        dissolve::for_each(ParallelPolicies::par, beads_.begin(), beads_.end(), [](CGBead &bead) { bead.initialiseXRayFormFactors(); });
        return true;
    }

    const std::size_t nBeads() const { return beads_.size(); }

    const double average_natoms_per_bead() const { return av_noa_bead_; }

    const CGBead &operator[](const int i) const { return beads_[i]; };
    CGBead &operator[](const int i) { return beads_[i]; };

    void calculateFormFactors(const std::vector<double> &qvals)
    {
        dissolve::for_each(
            ParallelPolicies::par,
            beads_.begin(), 
            beads_.end(), 
            [&](CGBead &b){ b.calculateFormFactor(qvals); }
        );
    }

    void calculateSelfInteractionTerms(const std::vector<double>& fractions)
    {
        av_noa_bead_ = 0.0;
        std::vector<int> indices(fractions.size());
        std::iota(indices.begin(), indices.end(), 0);
        dissolve::for_each(ParallelPolicies::seq, indices.begin(), indices.end(),
                           [&](const int &i) { av_noa_bead_ += fractions[i] * beads_[i].nAtoms(); });
        Messenger::print("Calculated average atoms per bead = {:7.3f}\n", av_noa_bead_);
        dissolve::for_each(ParallelPolicies::par, indices.begin(), indices.end(),
                           [&](const int &i) { beads_[i].calculateSelfScattrering(fractions[i], av_noa_bead_, false); });
    }

    void calculateXRaySelfInteractionTerms(const std::vector<double>& fractions)
    {
        av_noa_bead_ = 0.0;
        std::vector<int> indices(fractions.size());
        std::iota(indices.begin(), indices.end(), 0);
        dissolve::for_each(ParallelPolicies::seq, indices.begin(), indices.end(),
                           [&](const int &i)
                           {
                               av_noa_bead_ += fractions[i] * beads_[i].nAtoms();
                           });
        Messenger::print("Calculated average atoms per bead = {:7.3f}\n", av_noa_bead_);
        dissolve::for_each(ParallelPolicies::par, indices.begin(), indices.end(),
                           [&](const int &i) 
                           { 
                               beads_[i].calculateXRaySelfScattrering(fractions[i], av_noa_bead_);
                           });
    }

    void deuterate(const double fraction)
    {
        dissolve::for_each(
            ParallelPolicies::seq,
            beads_.begin(), 
            beads_.end(), 
            [fraction=fraction](CGBead &b){ b.deuterate(fraction); }
        );
    }

    void deuterate(const std::vector<double> &dfractions)
    {
        std::vector<int> indices(dfractions.size());
        std::iota(indices.begin(), indices.end(), 0);
        dissolve::for_each(ParallelPolicies::par, indices.begin(), indices.end(),
                           [&](const int &i)
                           {
                               if (dfractions[i] > 0.0)
                               {
                                   beads_[i].deuterate(dfractions[i]);
                               }
                           });
    }
    
    void print() const 
    {
        Messenger::print("CG Bead Representations: \n");
        Messenger::print("  Bead      nb (barn)   Sigma   Distribution  Composition\n");
        Messenger::print("  -------------------------------------------------------\n");
        for (const CGBead &bead : beads_)
        {          
            Messenger::print("  {:8s}  {:9.4f}   {:6.3f}  {:^14s}\n", bead.label(), bead.scatteringLength(), bead.sigma(),
                             bead.atomDistribution());
            dissolve::for_each(bead.atomTypes().begin(), bead.atomTypes().end(),
                               [&](const AtomTypeData &atm)
                               {
                                   Messenger::print("{:47s}{:3d} x {:3s}", " ", atm.population(),
                                                    Elements::symbol(atm.atomType()->Z()));
                               });
        }
        Messenger::print("  -------------------------------------------------------\n");
    }

    void clear()
    {
        beads_.clear();
        av_noa_bead_ = 0.0;
    }

    bool initialiseXRayFormFactors()
    {
        dissolve::for_each(beads_.begin(), beads_.end(), [](CGBead &bead) { bead.initialiseXRayFormFactors(); });
    }

    std::vector<CGBead>::const_iterator begin() const { return beads_.begin(); }
    std::vector<CGBead>::const_iterator end() const { return beads_.end(); }

    private:
    std::vector<CGBead> beads_;
    double av_noa_bead_ = 0.0;
};