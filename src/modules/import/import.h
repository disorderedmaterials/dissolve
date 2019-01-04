/*
	*** Import Module
	*** src/modules/import/import.h
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

#ifndef DISSOLVE_IMPORTMODULE_H
#define DISSOLVE_IMPORTMODULE_H

#include "module/module.h"
#include "modules/import/formats.h"

// Forward Declarations
/* none */

// Import Module
class ImportModule : public Module
{
	public:
	// Constructor
	ImportModule();
	// Destructor
	~ImportModule();


	/*
	 * Instances
	 */
	protected:
	// List of all instances of this Module type
	static List<Module> instances_;

	public:
	// Return list of all created instances of this Module
	List<Module>& instances();
	// Create instance of this module
	Module* createInstance();


	/*
	 * Definition
	 */
	public:
	// Return type of module
	const char* type() const;
	// Return brief description of module
	const char* brief() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;


	/*
	 * Options
	 */
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


	/*
	 * Data
	 */
	private:
	// Trajectory file source
	TrajectoryImportFileFormat trajectoryFile_;


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Static Functions - Coordinates
	 */
	public:
	// Read coordinates in specified format
	static bool readCoordinates(CoordinateImportFileFormat::CoordinateImportFormat format, LineParser& parser, Array< Vec3<double> >& r);
	// Read xyz coordinates from specified file
	static bool readXYZCoordinates(LineParser& parser, Array< Vec3<double> >& r);
	// Read DL_POLY coordinates from specified file
	static bool readDLPOLYCoordinates(LineParser& parser, Array< Vec3<double> >& r);
	// Read EPSR ATO coordinates from specified file
	static bool readEPSRCoordinates(LineParser& parser, Array< Vec3<double> >& r);


	/*
	 * Static Functions - Forces
	 */
	public:
	// Read forces in specified format
	static bool readForces(ForceImportFileFormat::ForceImportFormat format, LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Read simple forces from specified file
	static bool readSimpleForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Read DL_POLY forces from specified file
	static bool readDLPOLYForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
};

#endif

