// FROM CONFIGURATION_BOX.CPP

// Set up periodic Box
XXX This needs to be ripped out, along with initialise().  Split Box creation from Cells creation?
Configuration::setUpBox()
{
	// Remove old box if present
	if (box_ != NULL)
	{
		Messenger::printVerbose("Removing existing box definition...\n");
		delete box_;
	}

	// Determine volume for box, if a density was supplied. Otherwise, set to -1.0 to keep current cell lengths
	double volume = -1.0;
	if (density_ > 0.0) volume = nExpectedAtoms / atomicDensity();

	if (nonPeriodic_)
	{
		// Might need to increase pseudo-box volume to accommodate three times the ppRange
		if (volume < pow(ppRange*3.0, 3.0)) volume = pow(ppRange*3.0, 3.0);
		box_ = new NonPeriodicBox(volume);
	}
	else box_ = Box::generate(relativeBoxLengths_, boxAngles_, volume);
	Messenger::print("Configuration box volume is %f cubic Angstroms (reciprocal volume = %e)\n", box_->volume(), box_->reciprocalVolume());

	// Need to calculate atomic density if it wasn't provided
	if (density_ < 0.0) density_ = nExpectedAtoms / box_->volume();

	// Store box axis lengths as new relativeBoxLengths_
	relativeBoxLengths_.set(box_->axisLength(0), box_->axisLength(1), box_->axisLength(2));

	Messenger::print("%s box created for Configuration '%s':\n", Box::boxType(box_->type()), name());
	Matrix3 axes = box_->axes();
	Messenger::print("Axes Matrix : A = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[0], axes[1], axes[2], relativeBoxLengths_.x);
	Messenger::print("              B = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[3], axes[4], axes[5], relativeBoxLengths_.y);
	Messenger::print("              C = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[6], axes[7], axes[8], relativeBoxLengths_.z);

	// Check cell lengths against pair potential range
	if (ppRange > box_->inscribedSphereRadius())
	{
		Messenger::error("PairPotential range (%f) is longer than the shortest non-minimum image distance (%f).\n", ppRange, box_->inscribedSphereRadius());
		return false;
	}

	// Generate cells within unit cell
	cells_.generate(box_, requestedCellDivisionLength_, ppRange, atomicDensity());

	return true;
}

// Initialise from assigned Species populations
bool Configuration::initialise(ProcessPool& procPool, bool randomise, double pairPotentialRange)
{
	Messenger::print("Setting up Configuration from Species / multiplier definition...\n");

	// Clear current contents
	empty();

	/*
	 * Check Species populations, and calculate total number of expected Atoms
	 */
	if (multiplier_ < 1)
	{
		Messenger::error("Configuration multiplier is zero or negative (%i).\n", multiplier_);
		return false;
	}
	int nExpectedAtoms = 0;
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next)
	{
		// Get Species pointer
		Species* sp = spInfo->species();

		// Determine the number of molecules of this component
		int count =  spInfo->population() * multiplier_;

		// Check for zero count
		if (count == 0)
		{
			Messenger::error("Relative population for Species '%s' is too low (%e) to provide any Molecules in this Configuration.\n",  sp->name(),  spInfo->population());
			return false;
		}

		nExpectedAtoms += count * sp->nAtoms();
	}

	/*
	 * Create a Box to contain the system
	 */
	Messenger::print("Creating periodic Box and Cell partitioning...\n");
	if (!setUpBox(procPool, pairPotentialRange, nExpectedAtoms))
	{
		Messenger::error("Failed to set up Box/Cells for Configuration.\n");
		return false;
	}

	/*
	 * Create Molecules
	 */
	Messenger::print("Setting up Molecules...\n");

	procPool.initialiseRandomBuffer(ProcessPool::PoolProcessesCommunicator);
	Vec3<double> r, cog, newCentre, fr;
	Matrix3 transform;

	ListIterator<SpeciesInfo> speciesInfoIterator(usedSpecies_);
	while (SpeciesInfo* spInfo = speciesInfoIterator.iterate())
	{
		// Determine the number of molecules of this component
		int count = spInfo->population() * multiplier_;

		// Add copies of Species as Molecules
		for (int n=0; n<count; ++n)
		{
			// Add the Molecule
			Molecule* mol = addMolecule(spInfo->species());

			// Generate random positions and orientations if needed
			if (randomise)
			{
				// Set / generate position of Molecule
				switch (spInfo->insertionPositioning())
				{
					case (SpeciesInfo::RandomPositioning):
						fr.set(procPool.random(), procPool.random(), procPool.random());
						newCentre = box_->fracToReal(fr);
						mol->setCentreOfGeometry(box_, newCentre);
						break;
					case (SpeciesInfo::CentralPositioning):
						fr.set(0.5, 0.5, 0.5);
						newCentre = box_->fracToReal(fr);
						mol->setCentreOfGeometry(box_, newCentre);
						break;
					case (SpeciesInfo::CurrentPositioning):
						break;
					default:
						Messenger::error("Unrecognised positioning type.\n");
						break;
				}

				// Generate and apply a random rotation matrix
				if (spInfo->rotateOnInsertion())
				{
					transform.createRotationXY(procPool.randomPlusMinusOne()*180.0, procPool.randomPlusMinusOne()*180.0);
					mol->transform(box_, transform);
				}

				// Explore conformation space within the molecule by rotating bonds
				// TODO
			}
		}
	}

	// Set fractional populations in usedAtomTypes_
	usedAtomTypes_.finalise();

	++contentsVersion_;

	return true;
}

			case (ConfigurationBlock::SpeciesInfoKeyword):
				sp = dissolve->findSpecies(parser.argc(1));
				if (sp == NULL)
				{
					Messenger::error("Configuration refers to Species '%s', but no such Species is defined.\n", parser.argc(1));
					error = true;
				}
				else
				{
					// Are we already using this species
					if (cfg->hasUsedSpecies(sp))
					{
						Messenger::error("Configuration already uses Species '%s' - cannot add it a second time.\n", sp->name());
						error = true;
					}

					// Add this species to the list of those used by the Configuration
					spInfo = cfg->addUsedSpecies(sp, parser.hasArg(2) ? parser.argd(2) : 1.0);

					// If no population was given, assume that a block will follow with additional information
					if (!parser.hasArg(2))
					{
						if (!SpeciesInfoBlock::parse(parser, dissolve, spInfo))
						{
							error = true;
							break;
						}
					}

					Messenger::print("Set composition for Species '%s' (relative population = %f).\n", sp->name(), spInfo->population());
				}
				break;

				// WRITE
		// Species
		if (!parser.writeLineF("\n  # Species Information\n")) return false;
		for (SpeciesInfo* spInfo = cfg->usedSpecies().first(); spInfo != NULL; spInfo = spInfo->next)
		{
			Species* sp = spInfo->species();
			if (!parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), sp->name())) return false;

			if (!parser.writeLineF("    %s  %f\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::PopulationKeyword), spInfo->population())) return false;
			if (!spInfo->rotateOnInsertion() && (!parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::NoRotationKeyword), DissolveSys::btoa(false)))) return false;
			if ((spInfo->insertionPositioning() != SpeciesInfo::RandomPositioning) && (!parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::PositioningKeyword), SpeciesInfo::positioningType(spInfo->insertionPositioning())))) return false;

			if (!parser.writeLineF("  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::EndSpeciesInfoKeyword))) return false;
		}
				
// Return the atomic density of the system
double Configuration::atomicDensity() const
{
	// Calculate atomic density from chemical density?
	if (densityIsAtomic_) return density_;
	
	// Determine total atomic mass and number of atoms in system
	double mass = 0.0, nAtoms = 0.0;
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next)
	{
		Species* sp = spInfo->species();

		mass += multiplier_ * spInfo->population() * sp->mass();
		nAtoms += multiplier_ * spInfo->population() * sp->nAtoms();
	}

	// Convert density from g/cm3 to g/A3
	double rho = nAtoms  / ((mass / AVOGADRO) / (density_ / 1.0E24));
	
	Messenger::printVerbose("Converting %f atoms (mass = %f, density = %f) to atomic density = %f\n", nAtoms, mass, density_, rho);

	return rho;
}


// SpeciesInfo Block Keywords
KeywordData SpeciesInfoBlockData[] = {
	{ "EndSpeciesInfo",		0,	"Signals the end of the SpeciesInfo" },
	{ "NoRotation",			0,	"Flag that the Species should not be rotated when making a random configuration" },
	{ "Population",			1,	"Relative population of the Species" },
	{ "Positioning",		1,	"Positioning type to use for Species" }
};

// Convert text string to SpeciesInfoKeyword
SpeciesInfoBlock::SpeciesInfoKeyword SpeciesInfoBlock::keyword(const char* s)
{
	for (int n=0; n<nSpeciesInfoKeywords; ++n) if (DissolveSys::sameString(s,SpeciesInfoBlockData[n].name)) return (SpeciesInfoBlock::SpeciesInfoKeyword) n;
	return nSpeciesInfoKeywords;
}

// Convert SpeciesInfoKeyword to text string
const char* SpeciesInfoBlock::keyword(SpeciesInfoBlock::SpeciesInfoKeyword id)
{
	return SpeciesInfoBlockData[id].name;
}

// Return minimum number of expected arguments
int SpeciesInfoBlock::nArguments(SpeciesInfoBlock::SpeciesInfoKeyword id)
{
	return SpeciesInfoBlockData[id].nArguments;
}

// Parse SpeciesInfo block
bool SpeciesInfoBlock::parse(LineParser& parser, Dissolve* dissolve, SpeciesInfo* speciesInfo)
{
	Messenger::print("\nParsing %s block '%s'...\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), speciesInfo->species()->name());

	SpeciesInfo::PositioningType pt;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		SpeciesInfoBlock::SpeciesInfoKeyword spInfoKeyword = SpeciesInfoBlock::keyword(parser.argc(0));
		if ((spInfoKeyword != SpeciesInfoBlock::nSpeciesInfoKeywords) && ((parser.nArgs()-1) < SpeciesInfoBlock::nArguments(spInfoKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", SpeciesInfoBlock::keyword(spInfoKeyword));
			error = true;
			break;
		}
		switch (spInfoKeyword)
		{
			case (SpeciesInfoBlock::EndSpeciesInfoKeyword):
				Messenger::print("Found end of %s block.\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword));
				blockDone = true;
				break;
			case (SpeciesInfoBlock::NoRotationKeyword):
				speciesInfo->setRotateOnInsertion(false);
				break;
			case (SpeciesInfoBlock::PopulationKeyword):
				speciesInfo->setPopulation(parser.argd(1));
				break;
			case (SpeciesInfoBlock::PositioningKeyword):
				pt = SpeciesInfo::positioningType(parser.argc(1));
				if (pt == SpeciesInfo::nPositioningTypes)
				{
					Messenger::error("Unrecognised positioning type '%s' found.\n", parser.argc(1));
					error = true;
				}
				else speciesInfo->setInsertionPositioning(pt);
				break;
			case (SpeciesInfoBlock::nSpeciesInfoKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), parser.argc(0));
				BlockKeywords::printValidKeywords(BlockKeywords::SpeciesInfoBlockKeyword);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), SpeciesInfoBlock::keyword(spInfoKeyword));
				error = true;
				break;
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated %s block found.\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword));
		error = true;
	}

	return (!error);
}
