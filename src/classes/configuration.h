/*
    *** Configuration
    *** src/classes/configuration.h
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

#pragma once

#include "base/processpool.h"
#include "base/version.h"
#include "classes/atom.h"
#include "classes/atomtypelist.h"
#include "classes/cellarray.h"
#include "classes/molecule.h"
#include "classes/sitestack.h"
#include "classes/speciesinfo.h"
#include "genericitems/list.h"
#include "io/import/coordinates.h"
#include "math/data1d.h"
#include "math/histogram1d.h"
#include "math/interpolator.h"
#include "module/layer.h"
#include "procedure/procedure.h"
#include "templates/array.h"
#include "templates/dynamicarray.h"
#include "templates/objectstore.h"
#include "templates/orderedlist.h"
#include "templates/vector3.h"
#include <deque>
#include <memory>

// Forward Declarations
class Box;
class Cell;
class CoordinateSet;
class PotentialMap;
class Species;

// Configuration
class Configuration : public ListItem<Configuration>, public ObjectStore<Configuration>
{
    public:
    Configuration();
    ~Configuration();
    void operator=(Configuration &source);
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
    // Procedure to generate the Configuration
    Procedure generator_;
    // File / format of input coordinates file, if provided
    CoordinateImportFileFormat inputCoordinates_;
    // Temperature of this configuration (K)
    double temperature_;

    public:
    // Set name of the Configuration
    void setName(std::string_view name);
    // Return name of the Configuration
    std::string_view name() const;
    // Return nice name of the Configuration
    std::string_view niceName() const;
    // Return the current generator
    Procedure &generator();
    // Create the Configuration according to its generator Procedure
    bool generate(ProcessPool &procPool, double pairPotentialRange);
    // Return import coordinates file / format
    CoordinateImportFileFormat &inputCoordinates();
    // Load coordinates from specified parser
    bool loadCoordinates(LineParser &parser, CoordinateImportFileFormat::CoordinateImportFormat format);
    // Initialise (generate or load) the basic contents of the Configuration
    bool initialiseContent(ProcessPool &procPool, double pairPotentialRange, bool emptyCurrentContent = false);
    // Set configuration temperature
    void setTemperature(double t);
    // Return configuration temperature
    double temperature() const;

    /*
     * Content
     */
    private:
    // List of Species used by the Configuration and their populations
    List<SpeciesInfo> usedSpecies_;
    // AtomType list, containing unique (non-isotopic) atom types over all Species used in this configuration
    AtomTypeList usedAtomTypes_;
    // Contents version, incremented whenever Configuration content or Atom positions change
    VersionCounter contentsVersion_;
    // Array of Molecules
    std::deque<std::shared_ptr<Molecule>> molecules_;
    // Array of Atoms
    DynamicArray<Atom> atoms_;

    public:
    // Empty contents of Configuration, leaving core definitions intact
    void empty();
    // Initialise content array
    void initialiseArrays(int nMolecules);
    // Return specified used type
    std::shared_ptr<AtomType> usedAtomType(int index);
    // Return specified used type data
    AtomTypeData &usedAtomTypeData(int index);
    // Return first AtomTypeData for this Configuration
    const AtomTypeData &usedAtomTypes() const;
    // Return AtomTypeList for this Configuration
    const AtomTypeList &usedAtomTypesList() const;
    // Return number of atom types used in this Configuration
    int nUsedAtomTypes() const;
    // Add Species to list of those used by the Configuration, setting/adding the population specified
    SpeciesInfo *addUsedSpecies(Species *sp, int population);
    // Return SpeciesInfo for specified Species
    SpeciesInfo *usedSpeciesInfo(Species *sp);
    // Return list of SpeciesInfo for the Configuration
    List<SpeciesInfo> &usedSpecies();
    // Return if the specified Species is present in the usedSpecies list
    bool hasUsedSpecies(Species *sp);
    // Return the total atomic mass present in the Configuration
    double atomicMass() const;
    // Return the atomic density of the Configuration
    double atomicDensity() const;
    // Return the chemical density (g/cm3) of the Configuration
    double chemicalDensity() const;
    // Return version of current contents
    int contentsVersion() const;
    // Increment version of current contents
    void incrementContentsVersion();
    // Add Molecule to Configuration based on the supplied Species
    std::shared_ptr<Molecule> addMolecule(Species *sp, CoordinateSet *sourceCoordinates = nullptr);
    // Return number of Molecules in Configuration
    int nMolecules() const;
    // Return array of Molecules
    std::deque<std::shared_ptr<Molecule>> &molecules();
    // Return nth Molecule
    std::shared_ptr<Molecule> molecule(int n);
    // Add new Atom to Configuration
    Atom *addAtom(const SpeciesAtom *sourceAtom, std::shared_ptr<Molecule> molecule, Vec3<double> r = Vec3<double>());
    // Return number of Atoms in Configuration
    int nAtoms() const;
    // Return Atom array
    DynamicArray<Atom> &atoms();
    // Return Atom array (const)
    const DynamicArray<Atom> &constAtoms() const;
    // Return nth Atom
    Atom *atom(int n);
    // Scale geometric centres of molecules within box
    void scaleMoleculeCentres(double factor);

    /*
     * Periodic Box and Cells
     */
    private:
    // Requested size factor for Box
    double requestedSizeFactor_;
    // Size factor currently applied to Box / Cells
    double appliedSizeFactor_;
    // Periodic Box definition for the Configuration
    Box *box_;
    // Requested side length for individual Cell
    double requestedCellDivisionLength_;
    // Cell array
    CellArray cells_;

    public:
    // Create Box definition with specified lengths and angles
    bool createBox(const Vec3<double> lengths, const Vec3<double> angles, bool nonPeriodic = false);
    // Return Box
    const Box *box() const;
    // Scale Box (and associated Cells) by specified factor
    void scaleBox(double factor);
    // Set requested size factor for Box
    void setRequestedSizeFactor(double factor);
    // Return requested size factor for Box
    double requestedSizeFactor();
    // Return last size factor applied to Box / Cells
    double appliedSizeFactor();
    // Set requested side length for individual Cell
    void setRequestedCellDivisionLength(double a);
    // Return requested side length for individual Cell
    double requestedCellDivisionLength() const;
    // Return cell array
    CellArray &cells();
    // Return cell array
    const CellArray &constCells() const;
    // Scale Box, Cells, and Molecule geometric centres according to current size factor
    void applySizeFactor(const PotentialMap &potentialMap);

    /*
     * Upkeep
     */
    public:
    // Update Cell contents
    void updateCellContents();
    // Update Cell location of specified Atom
    void updateCellLocation(Atom *i);
    // Update Cell location of specified Molecule
    void updateCellLocation(std::shared_ptr<Molecule> mol);
    // Update Cell location of specified Atom indices
    void updateCellLocation(const std::vector<int> &targetAtoms, int indexOffset);

    /*
     * Modules
     */
    private:
    // Module layer associated to this Configuration
    ModuleLayer moduleLayer_;
    // Variables set by Modules
    GenericList moduleData_;

    public:
    // Associate Module to the Configuration
    bool ownModule(Module *module);
    // Return number of Modules associated to this Configuration
    int nModules() const;
    // Return Module layer for this Configuration
    ModuleLayer &moduleLayer();
    // Return list of Modules associated to this Configuration
    ModuleList &modules();
    // Return list of variables set by Modules
    GenericList &moduleData();

    /*
     * Site Stacks
     */
    private:
    // List of current SiteStacks
    List<SiteStack> siteStacks_;

    public:
    // Calculate / retrieve stack of sites for specified SpeciesSite
    const SiteStack *siteStack(SpeciesSite *site);

    /*
     * I/O
     */
    public:
    // Write through specified LineParser
    bool write(LineParser &parser) const;
    // Read through specified LineParser
    bool read(LineParser &parser, const List<Species> &availableSpecies, double pairPotentialRange);

    /*
     * Parallel Comms
     */
    private:
    // Process pool for this Configuration
    ProcessPool processPool_;

    public:
    // Set up process pool for this Configuration
    bool setUpProcessPool(Array<int> worldRanks, int groupPopulation);
    // Return process pool for this Configuration
    ProcessPool &processPool();
    // Broadcast coordinate from specified root process
    bool broadcastCoordinates(ProcessPool &procPool, int rootRank);
};
