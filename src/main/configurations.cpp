/*
	*** Dissolve - Configurations
	*** src/main/configurations.cpp
	Copyright T. Youngs 2012-2019

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

#include "main/dissolve.h"
#include "classes/box.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include <string.h>

// Add new Configuration
Configuration* Dissolve::addConfiguration()
{
	Configuration* cfg = coreData_.addConfiguration();

	return cfg;
}

// Own specified Configuration
bool Dissolve::ownConfiguration(Configuration* cfg)
{
	// Sanity check - do we already own this Configuration?
	if (coreData_.configurations().contains(cfg)) return Messenger::error("Already own Configuration '%s', so nothing to do.\n", cfg->name());

	coreData_.configurations().own(cfg);

	return true;
}

// Remove specified Configuration
void Dissolve::removeConfiguration(Configuration* cfg)
{
	if (!cfg) return;

	// Remove any references to the Modules in the Configuration's local processing layer before we delete it
	ListIterator<Module> moduleIterator(cfg->modules());
	while (Module* module = moduleIterator.iterate()) removeReferencesTo(module);

	// Remove references to the Configuration itself
	removeReferencesTo(cfg);

	// Now safe to remove the Configuration
	coreData_.removeConfiguration(cfg);
}

// Return number of defined Configurations
int Dissolve::nConfigurations() const
{
	return coreData_.nConfigurations();
}

// Return Configuration list
List<Configuration>& Dissolve::configurations()
{
	return coreData_.configurations();
}

// Return Configuration list (const)
const List<Configuration>& Dissolve::constConfigurations() const
{
	return coreData_.configurations();
}

// Find configuration by name
Configuration* Dissolve::findConfiguration(const char* name) const
{
	return coreData_.findConfiguration(name);
}

// Find configuration by 'nice' name
Configuration* Dissolve::findConfigurationByNiceName(const char* name) const
{
	for (Configuration* cfg = constConfigurations().first(); cfg != NULL; cfg = cfg->next()) if (DissolveSys::sameString(name, cfg->niceName())) return cfg;

	return NULL;
}

// Write Configuration through specified LineParser
bool Dissolve::writeConfiguration(Configuration* cfg, LineParser& parser)
{
	int molId;
	SpeciesAngle* spa;
	SpeciesBond* spb;
	SpeciesTorsion* spt;

	if (!parser.writeLineF("'%s'  %i  %i  # nMolecules  nGrains\n", cfg->name(), cfg->nMolecules(), cfg->nGrains())) return false;

	// Write unit cell (box) lengths and angles
	Vec3<double> lengths = cfg->box()->axisLengths();
	Vec3<double> angles = cfg->box()->axisAngles();
	if (!parser.writeLineF("%12e %12e %12e  %f  %s\n", lengths.x, lengths.y, lengths.z, cfg->requestedSizeFactor(), DissolveSys::btoa(cfg->box()->type() == Box::NonPeriodicBoxType))) return false;
	if (!parser.writeLineF("%12e %12e %12e\n", angles.x, angles.y, angles.z)) return false;

	// Write Molecule types - write sequential Molecules with same type as single line
	int moleculeCount = 1;
	const Species* lastType = cfg->nMolecules() > 0 ? cfg->molecule(0)->species() : NULL;
	for (int n=1; n<cfg->nMolecules(); ++n)
	{
		// If the last Molecule's Species is the same as this one, increment counter and move on
		if (lastType == cfg->molecule(n)->species())
		{
			++moleculeCount;
			continue;
		}

		// Species is different between this molecule and the last - write this info, and reset the counter
		if (!parser.writeLineF("%i  '%s'\n", moleculeCount, lastType->name())) return false;
		moleculeCount = 1;
		lastType = cfg->molecule(n)->species();
	}
	// Write final 
	if (!parser.writeLineF("%i  '%s'\n", moleculeCount, lastType->name())) return false;

	// Write all Atoms - for each write type, coordinates, charge, mol ID, and grain ID
	if (!parser.writeLineF("%i  # nAtoms\n", cfg->nAtoms())) return false;
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%i %e %e %e\n", i->molecule()->arrayIndex(), i->x(), i->y(), i->z())) return false;
	}

	return true;
}

// Read Configuration through specified LineParser
bool Dissolve::readConfiguration(Configuration* cfg, LineParser& parser)
{
	Molecule* mol;
	Grain* grain;

	// Clear current contents of Configuration
	cfg->empty();

	// Read configuration name, nMolecules, and nGrains, and initialise those arrays
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	cfg->setName(parser.argc(0));
	cfg->empty();

	// Read Box lengths and angles, and create the Box
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	Vec3<double> lengths = parser.arg3d(0);
	cfg->setRequestedSizeFactor(parser.hasArg(3) ? parser.argd(3) : 1.0);
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	Vec3<double> angles = parser.arg3d(0);
	if (!cfg->createBox(lengths, angles)) return false;

	// Read Species types for Molecules
	int nMolsRead = 0;
	while (nMolsRead < cfg->nMolecules())
	{
		// Read line containing number of molecules and Species name
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		Species* sp = findSpecies(parser.argc(1));
		if (!sp) return Messenger::error("Unrecognised Species '%s' found in Configuration '%s' in restart file.\n", parser.argc(1), cfg->name());

		// Set Species pointers for this range of Molecules
		int nMols = parser.argi(0);
		for (int n=0; n<nMols; ++n) cfg->addMolecule(sp);

		// Update the used species population
		cfg->addUsedSpecies(sp, nMols);

		// Increase our counter
		nMolsRead += parser.argi(0);
	}

	// Read in Atoms
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nAtoms = parser.argi(0);
	for (int n=0; n<nAtoms; ++n)
	{
		// Each line contains molecule ID and coordinates only
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		cfg->atom(n)->setCoordinates(parser.arg3d(1));
	}

	cfg->finaliseAfterLoad(worldPool(), pairPotentialRange_);

	return true;
}
