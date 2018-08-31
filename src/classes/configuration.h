/*
	*** Configuration
	*** src/classes/configuration.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_CONFIGURATION_H
#define DISSOLVE_CONFIGURATION_H

#include "classes/atom.h"
#include "classes/angle.h"
#include "classes/bond.h"
#include "classes/torsion.h"
#include "classes/atomtypelist.h"
#include "classes/cellarray.h"
#include "classes/molecule.h"
#include "classes/speciesinfo.h"
#include "classes/sitestack.h"
#include "module/list.h"
#include "modules/import/formats.h"
#include "math/histogram1d.h"
#include "math/interpolator.h"
#include "math/data1d.h"
#include "base/processpool.h"
#include "base/genericlist.h"
#include "templates/vector3.h"
#include "templates/orderedlist.h"
#include "templates/array.h"
#include "templates/dynamicarray.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Grain;
class PotentialMap;
class Species;

// Configuration
class Configuration : public ListItem<Configuration>
{
	public:
	// Constructor
	Configuration();
	// Destructor
	~Configuration();
	// Assignment operator
	void operator=(Configuration& source);


	/*
	 * Composition
	 */
	private:
	// Name of the configuration
	CharString name_;
	// Nice name (generated from name_) used for output files
	CharString niceName_;
	// Reference list of Species used by the Configuration and their relative populations
	List<SpeciesInfo> usedSpecies_;
	// Integer multiplier of used relative species populations
	int multiplier_;
	// Density of the configuration
	double density_;
	// Whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
	bool densityIsAtomic_;
	// File containing input coordinates
	CharString inputCoordinatesFile_;
	// Format of input coordinates file
	ImportModuleFormats::CoordinateFormat inputCoordinatesFormat_;
	// Temperature of this configuration (K)
	double temperature_;

	public:
	// Set name of the configuration
	void setName(const char* name);
	// Return name of the configuration
	const char* name();
	// Return nice name of the configuration
	const char* niceName();
	// Add Species to list of those used by the Configuration
	SpeciesInfo* addUsedSpecies(Species* sp, double relativePopulation);
	// Return SpeciesInfo for specified Species
	SpeciesInfo* usedSpeciesInfo(Species* sp);
	// Return list of SpeciesInfo for the Configuration
	List<SpeciesInfo>& usedSpecies();
	// Return if the specifeid Species is present in the usedSpecies list
	bool hasUsedSpecies(Species* sp);
	// Return total relative population of Species used by this Configuration
	double totalRelative() const;
	// Set multiplier for System components
	void setMultiplier(int mult);
	// Return multiplier for System components
	int multiplier() const;
	// Set the atomic density of the system (atoms/A3)
	void setAtomicDensity(double density);
	// Set the chemical density of the system (g/cm3)
	void setChemicalDensity(double density);
	// Return the density of the system
	double density() const;
	// Return whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
	bool densityIsAtomic() const;
	// Return the atomic density of the system
	double atomicDensity() const;
	// Set file containing input coordinates
	void setInputCoordinatesFile(const char* filename);
	// Return file containing input coordinates
	const char* inputCoordinatesFile() const;
	// Return whether a file containing input coordinates has been set
	bool hasInputCoordinatesFile() const;
	// Set input coordinates file format
	void setInputCoordinatesFormat(ImportModuleFormats::CoordinateFormat format);
	// Return input coordinates file format
	ImportModuleFormats::CoordinateFormat inputCoordinatesFormat();
	// Set configuration temperature
	void setTemperature(double t);
	// Return configuration temperature
	double temperature();
	// Clear all data
	void clear();


	/*
	 * Content
	 */
	private:
	// Array of Molecules
	DynamicArray<Molecule> molecules_;
	// Array of Grains
	DynamicArray<Grain> grains_;
	// Array of Atoms
	DynamicArray<Atom> atoms_;
	// Array of Bonds between Atoms
	DynamicArray<Bond> bonds_;
	// Array of Angles between Atoms
	DynamicArray<Angle> angles_;
	// Array of Torsions between Atoms
	DynamicArray<Torsion> torsions_;
	// AtomType list, containing unique (non-isotopic) atom types over all Species used in this configuration
	AtomTypeList usedAtomTypes_;
	// Coordinate index, incremented whenever Atom positions change
	int coordinateIndex_;

	public:
	// Initialise empty Molecule and Grain arrays
	void initialise(int nMolecules, int nGrains);
	// Initialise from assigned Species populations
	bool initialise(ProcessPool& procPool, bool randomise, double pairPotentialRange, int boxNormalisationNPoints);
	// Finalise Configuration after loading contents from restart file
	bool finaliseAfterLoad(ProcessPool& procPool, double pairPotentialRange, int boxNormalisationNPoints);
	// Add Molecule to Configuration based on the supplied Species
	Molecule* addMolecule(Species* sp);
	// Return number of Molecules in Configuration
	int nMolecules() const;
	// Return array of Molecules
	DynamicArray<Molecule>& molecules();
	// Return nth Molecule
	Molecule* molecule(int n);
	// Add new Grain to Configuration, with Molecule parent specified
	Grain* addGrain(Molecule* molecule);
	// Return number of Grains in Configuration
	int nGrains() const;
	// Return Grain array
	DynamicArray<Grain>& grains();
	// Return nth Grain
	Grain* grain(int n);
	// Add new Atom to Configuration, with Molecule and Grain parents specified
	Atom* addAtom(Molecule* molecule, Grain* grain = NULL);
	// Add new Atom with full data
	Atom* addAtom(Molecule* molecule, Grain* grain, AtomType* atomType, Vec3<double> r, double charge);
	// Return number of Atoms in Configuration
	int nAtoms() const;
	// Return Atom array
	DynamicArray<Atom>& atoms();
	// Return nth Atom
	Atom* atom(int n);
	// Add new Bond to Configuration, with Molecule parent specified
	Bond* addBond(Molecule* molecule, Atom* i, Atom* j);
	// Add new Bond to Configuration, with Molecule parent specified, from Atom indices
	Bond* addBond(Molecule* molecule, int i, int j);
	// Return number of Bonds in Configuration
	int nBonds() const;
	// Return Bond array
	DynamicArray<Bond>& bonds();
	// Return nth Bond
	Bond* bond(int n);
	// Add new Angle to Configuration, with Molecule parent specified
	Angle* addAngle(Molecule* molecule, Atom* i, Atom* j, Atom* k);
	// Add new Angle to Configuration, with Molecule parent specified, from Atom indices
	Angle* addAngle(Molecule* molecule, int i, int j, int k);
	// Return number of Angles in Configuration
	int nAngles() const;
	// Return Angle array
	DynamicArray<Angle>& angles();
	// Return nth Angle
	Angle* angle(int n);
	// Add new Torsion to Configuration, with Molecule parent specified
	Torsion* addTorsion(Molecule* molecule, Atom* i, Atom* j, Atom* k, Atom* l);
	// Add new Torsion to Configuration, with Molecule parent specified, from Atom indices
	Torsion* addTorsion(Molecule* molecule, int i, int j, int k, int l);
	// Return number of Torsions in Configuration
	int nTorsions() const;
	// Return Torsion array
	DynamicArray<Torsion>& torsions();
	// Return nth Torsion
	Torsion* torsion(int n);
	// Return specified used type
	AtomType* usedAtomType(int index);
	// Return first AtomTypeData for this Configuration
	AtomTypeData* usedAtomTypes();
	// Return AtomTypeList for this Configuration
	const AtomTypeList& usedAtomTypesList() const;
	// Return number of atom types used in this Configuration
	int nUsedAtomTypes();
	// Return current coordinate index
	int coordinateIndex();
	// Increment current coordinate index
	void incrementCoordinateIndex();
	// Load coordinates from specified parser
	bool loadCoordinates(LineParser& parser, ImportModuleFormats::CoordinateFormat format);


	/*
	 * Periodic Box and Cells
	 */
	private:
	// Relative Box lengths
	Vec3<double> relativeBoxLengths_;
	// Box angles
	Vec3<double> boxAngles_;
	// Requested size factor for Box
	double requestedSizeFactor_;
	// Size factor currently applied to Box / Cells
	double appliedSizeFactor_;
	// Whether the Configuration is non-periodic
	bool nonPeriodic_;
	// Periodic Box definition for the Configuration
	Box* box_;
	// Requested side length for individual Cell
	double requestedCellDivisionLength_;
	// Box normalisation array to load/save for this configuration
	CharString boxNormalisationFileName_;
	// Normalisation function for Box shape/extent in radial distribution functions
	Data1D boxNormalisation_;
	// Interpolation of Box normalisation function
	Interpolator boxNormalisationInterpolation_;
	// Cell array
	CellArray cells_;

	public:
	// Set relative Box lengths
	void setRelativeBoxLengths(const Vec3<double> lengths);
	// Set relative Box length
	void setRelativeBoxLength(int index, double length);
	// Return relative Box lengths
	Vec3<double> relativeBoxLengths() const;
	// Set Box angles
	void setBoxAngles(const Vec3<double> angles);
	// Set Box angle
	void setBoxAngle(int index, double angle);
	// Return Box angles
	Vec3<double> boxAngles() const;
	// Set requested size factor for Box
	void setRequestedSizeFactor(double factor);
	// Return requested size factor for Box
	double requestedSizeFactor();
	// Return last size factor applied to Box / Cells
	double appliedSizeFactor();
	// Set whether the Box is non-periodic
	void setNonPeriodic(bool b);
	// Return whether the Box is non-periodic
	bool nonPeriodic() const;
	// Set requested side length for individual Cell
	void setRequestedCellDivisionLength(double a);
	// Return requested side length for individual Cell
	double requestedCellDivisionLength() const;
	// Return Box
	const Box* box() const;
	// Set up periodic Box
	bool setUpBox(ProcessPool& procPool, double ppRange, int nExpectedAtoms, int boxNormalisationNPoints);
	// Scale Box (and associated Cells) by specified factor
	void scaleBox(double factor);
	// Set box normalisation array to load/save for this configuration
	void setBoxNormalisationFile(const char* filename);
	// Return box normalisation file to load/save for this configuration
	const char* boxNormalisationFileName() const;
	// Load Box normalisation array from specified file
	bool loadBoxNormalisationFile();
	// Return current Box normalisation array
	const Data1D& boxNormalisation() const;
	// Return interpolation of Box normalisation function
	Interpolator& boxNormalisationInterpolation();
	// Return cell array
	CellArray& cells();


	/*
	 * Calculation Limits
	 */
	private:
	// RDF bin width
	double rdfBinWidth_;
	// Maximum extent (actual) of calculated g(r)
	double rdfRange_;
	// Maximum extent (requested) of calculated g(r)
	double requestedRDFRange_;
	// Minimum Q value for Bragg calculation
	double braggQMin_;
	// Maximum Q value for Bragg calculation
	double braggQMax_;
	// Multiplicities reflecting any crystal supercell
	Vec3<int> braggMultiplicity_;

	public:
	// Set RDF bin width
	void setRDFBinWidth(double width);
	// Return RDF bin width
	double rdfBinWidth();
	// Return working RDF extent
	double rdfRange();
	// Set requested RDF extent
	void setRequestedRDFRange(double range);
	// Return requested RDF extent
	double requestedRDFRange();
	// Set minimum Q value for Bragg calculation
	void setBraggQMin(double qMin);
	// Return minimum Q value for Bragg calculation
	double braggQMin();
	// Set maximum Q value for Bragg calculation
	void setBraggQMax(double qMax);
	// Return maximum Q value for Bragg calculation
	double braggQMax();
	// Set multiplicities reflecting any crystal supercell
	void setBraggMultiplicity(Vec3<int> mult);
	// Return multiplicities reflecting any crystal supercell
	Vec3<int> braggMultiplicity();


	/*
	 * Upkeep
	 */
	public:
	// Update Cell contents
	void updateCellContents();
	// Update Cell location of specified Atom
	void updateCellLocation(Atom* i);
	// Update Cell location of specified Molecule
	void updateCellLocation(Molecule* mol);
	// Update Cell location of specified Atoms (in array)
	void updateCellLocation(int nAtoms, Atom** atoms);


	/*
	 * Modules
	 */
	private:
	// List of Modules associated to this Configuration
	ModuleList modules_;
	// Variables set by Modules
	GenericList moduleData_;

	public:
	// Add Module (or an instance of it) to the Configuration
	Module* addModule(Module* masterInstance);
	// Return number of Modules associated to this Configuration
	int nModules() const;
	// Return list of Modules associated to this Configuration
	ModuleList& modules();
	// Return list of variables set by Modules
	GenericList& moduleData();


	/*
	 * Preparation
	 */
	public:
	// Perform any preparative tasks for the Configuration, before Module processing begins
	bool prepare(const PotentialMap& potentialMap);


	/*
	 * Ensemble
	 */
	private:
	// Whether ensemble file is to be appended
	bool appendEnsemble_;
	// Frequency at which to append ensemble
	int ensembleFrequency_;

	public:
	// Set whether ensemble file is to be appended
	void setAppendEnsemble(bool b);
	// Return whether ensemble file is to be appended
	bool appendEnsemble() const;
	// Set  frequency at which to append ensemble
	void setEnsembleFrequency(int frequency);
	// Return frequency at which to append ensemble
	int ensembleFrequency() const;


	/*
	 * Site Stacks
	 */
	private:
	// List of current SiteStacks
	List<SiteStack> siteStacks_;

	public:
	// Calculate / retrieve stack of sites for specified SpeciesSite
	const SiteStack* siteStack(SpeciesSite* site);


	/*
	 * Parallel Comms
	 */
	private:
	// Process pool for this Configuration
	ProcessPool processPool_;

	public:
	// Set up process pool for this Configuration
	bool setUpProcessPool(Array<int> worldRanks);
	// Return process pool for this Configuration
	ProcessPool& processPool();
	// Broadcast coordinate from specified root process
	bool broadcastCoordinates(ProcessPool& procPool, int rootRank);
};

#endif
