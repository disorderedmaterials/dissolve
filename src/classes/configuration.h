// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "base/version.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cellArray.h"
#include "classes/molecule.h"
#include "classes/siteStack.h"
#include "generator/generator.h"
#include "io/import/coordinates.h"
#include "items/list.h"
#include "kernels/potentials/base.h"
#include "math/data1D.h"
#include "math/histogram1D.h"
#include "math/interpolator.h"
#include "math/vector3.h"
#include "module/layer.h"
#include <deque>
#include <map>
#include <memory>
#include <vector>

// Forward Declarations
class Cell;
class PotentialMap;
class ProcessPool;
class Species;

// Configuration
class Configuration : public Serialisable<const CoreData &>
{
    public:
    Configuration();
    ~Configuration();
    Configuration(const Configuration &source) = delete;
    Configuration(Configuration &&source) = delete;
    void operator=(Configuration &source) = delete;
    // Clear all data
    void clear();

    /*
     * Definition
     */
    private:
    // Name of the Configuration
    std::string name_;
    // Nice name (generated from name_) used for output files
    std::string niceName_;
    // Generator for the Configuration
    Generator generator_;
    // Temperature of this configuration (K)
    static constexpr double defaultTemperature_ = 300.0;
    double temperature_{defaultTemperature_};

    public:
    // Set name of the Configuration
    void setName(std::string_view name);
    // Return name of the Configuration
    std::string_view name() const;
    // Return nice name of the Configuration
    std::string_view niceName() const;
    // Return the generator for the Configuration
    Generator &generator();
    // Create the Configuration according to its generator
    bool generate(const GeneratorContext &generatorContext);
    // Initialise (generate or load) the basic contents of the Configuration
    bool initialiseContent(const GeneratorContext &generatorContext);
    // Set configuration temperature
    void setTemperature(double t);
    // Return configuration temperature
    double temperature() const;

    /*
     * Content
     */
    private:
    // Species populations present in the Configuration
    KeyedVector<const Species *, int> speciesPopulations_;
    // Molecule vector
    std::vector<std::shared_ptr<Molecule>> molecules_;
    // Atom vector
    std::vector<Atom> atoms_;
    // Configuration version, incremented whenever atom positions or atomic composition changes
    VersionCounter version_;
    // Flag stating whether local Atom type indices are up-to-date
    bool typeIndicesValid_{false};

    private:
    // Add new Atom to Configuration
    Atom &addAtom(const SpeciesAtom *sourceAtom, const std::shared_ptr<Molecule> &molecule, Vector3 r = Vector3());

    public:
    // Empty contents of Configuration
    void empty();
    // Return Species populations within the Configuration
    const KeyedVector<const Species *, int> &speciesPopulations() const;
    // Return atom type populations for this Configuration
    KeyedVector<const AtomType *, int> atomTypePopulations() const;
    // Return atom type index map
    std::map<const AtomType *, int> atomTypeIndexMap() const;
    // Return used atom type vector
    std::vector<const AtomType *> atomTypeVector() const;
    // Return the total charge of the Configuration
    double totalCharge(bool ppIncludeCoulomb) const;
    // Return the total atomic mass present in the Configuration
    double atomicMass() const;
    // Return the atomic density of the Configuration
    std::optional<double> atomicDensity() const;
    // Return the chemical density (g/cm3) of the Configuration
    std::optional<double> chemicalDensity() const;
    // Return version (atomic positions and composition)
    int version() const;
    // Flag that one or more atomic positions have changed
    void notifyAtomicPositionsChanged();
    // Add Molecule to Configuration based on the supplied Species
    std::shared_ptr<Molecule>
    addMolecule(const Species *sp, OptionalReferenceWrapper<const std::vector<Vector3>> sourceCoordinates = std::nullopt);
    // Copy molecule
    std::shared_ptr<Molecule> copyMolecule(const Molecule &sourceMolecule);
    // Remove all Molecules of the target Species from the Configuration
    void removeMolecules(const Species *sp);
    // Remove specified Molecules from the Configuration
    void removeMolecules(const std::vector<std::shared_ptr<Molecule>> &molecules);
    // Return number of Molecules in Configuration
    int nMolecules() const;
    // Return Molecule vector
    std::vector<std::shared_ptr<Molecule>> &molecules();
    const std::vector<std::shared_ptr<Molecule>> &molecules() const;
    // Return nth Molecule
    std::shared_ptr<Molecule> molecule(int n);
    // Return the number of atoms in the configuration (or only those with the specified presence)
    int nAtoms(SpeciesAtom::Presence withPresence = SpeciesAtom::Presence::Any) const;
    // Return Atom array
    std::vector<Atom> &atoms();
    const std::vector<Atom> &atoms() const;
    // Return nth Atom
    Atom &atom(int n);
    // Unfold molecule coordinates
    void unFoldMolecules();
    // Scale contents of the box by the specified factors along each axis
    void scaleContents(Vector3 scaleFactors);
    // Update type indices per Atom
    void updateTypeIndexing();

    /*
     * Periodic Box and Cells
     */
    private:
    // Default size factor for Box
    static constexpr double defaultSizeFactor_ = 1.0;
    // Requested size factor for Box
    std::optional<double> requestedSizeFactor_;
    // Size factor currently applied to Box / Cells
    std::optional<double> appliedSizeFactor_;
    // Periodic Box
    std::unique_ptr<Box> box_{nullptr};
    // Requested side length for individual Cell
    static constexpr double defaultCellDivisionLength_ = 7.0;
    double requestedCellDivisionLength_{defaultCellDivisionLength_};
    // Cell array
    CellArray cells_;

    public:
    // Create Box definition with specified lengths and angles
    void createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic = false);
    // Create Box definition from axes matrix
    void createBox(const Matrix3 axes);
    // Create Box definition with specified lengths and angles, and initialise cell array
    void createBoxAndCells(const Vector3 lengths, const Vector3 angles, bool nonPeriodic, double pairPotentialRange);
    // Create Box definition from axes matrix, and initialise cell array
    void createBoxAndCells(const Matrix3 axes, double pairPotentialRange);
    // Update cell array, and reassign atoms to cells
    void updateCells(double pairPotentialRange);
    // Return Box
    const Box *box() const;
    // Scale Box lengths (and associated Cells) by specified factors
    void scaleBox(Vector3 scaleFactors);
    // Set requested size factor for Box
    void setRequestedSizeFactor(double factor);
    // Return requested size factor for Box
    std::optional<double> requestedSizeFactor() const;
    // Return last size factor applied to Box / Cells
    std::optional<double> appliedSizeFactor() const;
    // Set requested side length for individual Cell
    void setRequestedCellDivisionLength(double a);
    // Return requested side length for individual Cell
    double requestedCellDivisionLength() const;
    // Return cell array
    CellArray &cells();
    const CellArray &cells() const;
    // Scale Box, Cells, and Molecule geometric centres according to current size factor
    void applySizeFactor(const PotentialMap &potentialMap);

    /*
     * External Potentials
     */
    private:
    // Defined global potentials
    std::vector<std::unique_ptr<ExternalPotential>> globalPotentials_;
    // Defined targeted potentials
    std::vector<std::unique_ptr<ExternalPotential>> targetedPotentials_;

    public:
    // Add global potential
    void addGlobalPotential(std::unique_ptr<ExternalPotential> potential);
    // Return vector of defined global potentials
    const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials() const;
    // Add targeted potential
    void addTargetedPotential(std::unique_ptr<ExternalPotential> potential);
    // Return vector of defined targeted potentials
    const std::vector<std::unique_ptr<ExternalPotential>> &targetedPotentials() const;
    // Link targeted potentials to atoms
    void linkTargetedPotentials();

    /*
     * Upkeep
     */
    public:
    // Rationalise object relationships between atoms, molecules, and cells
    void updateObjectRelationships();
    // Update Cell location of all Atoms
    void updateAtomLocations(bool clearExistingLocations = false);
    // Update Cell location of specified Atom
    void updateAtomLocation(Atom *i);
    // Update Cell locations of atoms within the specified Molecule
    void updateAtomLocations(const std::shared_ptr<Molecule> &mol);
    // Update Cell location of specified Atom indices
    void updateAtomLocations(const std::vector<int> &targetAtoms, int indexOffset);

    /*
     * Site Stacks
     */
    private:
    // List of current SiteStacks
    std::vector<std::unique_ptr<SiteStack>> siteStacks_;

    public:
    // Calculate / retrieve stack of sites for specified SpeciesSite
    const SiteStack *siteStack(const SpeciesSite *site);

    /*
     * Energy
     */
    private:
    // Energy stable flag
    bool energyIsStable_{false};
    // Energy gradient
    double energyGradient_;

    public:
    // Energy stable flag
    void setEnergyStable(bool stable);
    bool energyIsStable() const;
    // Energy gradient
    void setEnergyGradient(double grad);
    double getEnergyGradient() const;

    /*
     * I/O
     */
    public:
    // Write through specified LineParser
    bool serialise(LineParser &parser) const;
    // Read from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData, double pairPotentialRange, bool hasPotentials);
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &data) override;
};
