// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/speciesIntra.h"

#include <map>
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;
class CoreData;

// Torsion functional forms
class TorsionFunctions
{
    public:
    enum class Form
    {
        None,
        Cosine,
        Cos3,
        Cos3C,
        Cos4,
        CosN,
        CosNC,
        UFFCosine,
        FourierN
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
};

// SpeciesTorsion Definition
class SpeciesTorsion : public SpeciesIntra<SpeciesTorsion, TorsionFunctions>
{
    public:
    SpeciesTorsion();
    SpeciesTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    ~SpeciesTorsion() override;
    SpeciesTorsion(SpeciesTorsion &source);
    SpeciesTorsion(SpeciesTorsion &&source) noexcept;
    SpeciesTorsion &operator=(const SpeciesTorsion &source);
    SpeciesTorsion &operator=(SpeciesTorsion &&source) noexcept;

    /*
     * Atom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_{nullptr};
    // Second SpeciesAtom in interaction
    SpeciesAtom *j_{nullptr};
    // Third SpeciesAtom in interaction
    SpeciesAtom *k_{nullptr};
    // Fourth SpeciesAtom in interaction
    SpeciesAtom *l_{nullptr};

    private:
    // Detach from current atoms
    void detach();

    public:
    // Set Atoms involved in Torsion
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Rewrite SpeciesAtom pointer
    void switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr);
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return fourth SpeciesAtom
    SpeciesAtom *l() const;
    // Return vector of involved atoms
    std::vector<const SpeciesAtom *> atoms() const override;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of third SpeciesAtom
    int indexK() const;
    // Return index (in parent Species) of fourth SpeciesAtom
    int indexL() const;
    // Return index (in parent Species) of nth SpeciesAtom in interaction
    int index(int n) const;
    // Return whether SpeciesAtoms match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k, const SpeciesAtom *l) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    private:
    // Electrostatic and van der Waals 1-4 scale factors
    double electrostatic14Scaling_{0.5}, vdw14Scaling_{0.5};

    public:
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const override;
    // Set 1-4 scaling factors
    bool set14ScalingFactors(double elecScale, double vdwScale);
    // Set electrostatic 1-4 scaling factor for the interaction
    bool setElectrostatic14Scaling(double scaling);
    // Return electrostatic 1-4 scaling factor for the interaction
    double electrostatic14Scaling() const;
    // Set van der Waals 1-4 scaling factor for the interaction
    bool setVanDerWaals14Scaling(double scaling);
    // Return van der Waals 1-4 scaling factor for the interaction
    double vanDerWaals14Scaling() const;
    // Return energy for specified angle and functional form, given supplied parameters
    static double energy(double angleInDegrees, TorsionFunctions::Form form, const std::vector<double> &params);
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle and functional form, given supplied parameters
    static double force(double angleInDegrees, TorsionFunctions::Form form, const std::vector<double> &params);
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, CoreData &coreData);
};

// MasterTorsion Definition
class MasterTorsion : public SpeciesTorsion
{
    public:
    explicit MasterTorsion(std::string_view name) : SpeciesTorsion(), name_{name} {};
    using SpeciesIntra<SpeciesTorsion, TorsionFunctions>::deserialise;

    /*
     * Identifying Name
     */
    private:
    // Identifying name
    std::string name_;

    public:
    // Set identifying name
    void setName(std::string_view name) override { name_ = name; }
    // Return identifying name
    std::string_view name() const override { return name_; };
};
