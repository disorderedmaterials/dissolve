// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/array.h"
#include "templates/reflist.h"
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;
class MasterIntra;

// Base class for intramolecular interactions within Species
class SpeciesIntra
{
    public:
    SpeciesIntra();
    virtual ~SpeciesIntra() = default;
    SpeciesIntra(const SpeciesIntra &source);
    SpeciesIntra(SpeciesIntra &&source) = delete;
    SpeciesIntra &operator=(const SpeciesIntra &source);
    SpeciesIntra &operator=(SpeciesIntra &&source) = delete;

    /*
     * Interaction Parameters
     */
    public:
    // Interaction Type
    enum class InteractionType
    {
        Bond,    /* Interaction is a bond between two atoms  */
        Angle,   /* Interaction is an angle between three atoms */
        Torsion, /* Interaction is a torsion between four atoms */
        Improper /* Interaction is an improper torsion between four atoms */
    };

    protected:
    // Linked master from which parameters should be taken (if relevant)
    MasterIntra *masterParameters_;
    // Index of functional form of interaction
    int form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Set linked master from which parameters should be taken
    void setMasterParameters(MasterIntra *master);
    // Return linked master from which parameters should be taken
    const MasterIntra *masterParameters() const;
    // Detach from MasterIntra, if we are currently referencing one
    void detachFromMasterIntra();
    // Set functional form index of interaction
    void setForm(int form);
    // Return functional form index of interaction
    int form() const;
    // Add parameter to interaction
    void addParameter(double param);
    // Set all parameters
    void setParameters(const std::vector<double> &params);
    // Set existing parameter
    void setParameter(int id, double value);
    // Return number of parameters defined
    int nParameters() const;
    // Return nth parameter
    double parameter(int id) const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
    // Set up any necessary parameters
    virtual void setUp() = 0;
    // Calculate and return fundamental frequency for the interaction
    virtual double fundamentalFrequency(double reducedMass) const = 0;
    // Return type of this interaction
    virtual InteractionType type() const = 0;

    /*
     * Connections
     */
    private:
    // Number of SpeciesAtoms attached to termini (number of items stored in attached_ arrays)
    std::vector<int> attached_[2];
    // Whether the term is contained within a cycle
    bool inCycle_;

    public:
    // Set attached SpeciesAtoms for terminus specified
    void setAttachedAtoms(int terminus, const RefList<SpeciesAtom> &atoms);
    // Set attached SpeciesAtoms for terminus specified (single SpeciesAtom)
    void setAttachedAtoms(int terminus, SpeciesAtom *atom);
    // Return vector of attached indices for terminus specified
    const std::vector<int> &attachedAtoms(int terminus) const;
    // Set whether the term is contained within a cycle
    void setInCycle(bool b);
    // Return whether the term is contained within a cycle
    bool inCycle() const;
};
