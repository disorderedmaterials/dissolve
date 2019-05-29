/*
	*** Export Module
	*** src/modules/export/export.h
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

#ifndef DISSOLVE_EXPORTMODULE_H
#define DISSOLVE_EXPORTMODULE_H

#include "module/module.h"
#include "modules/export/formats.h"

// Forward Declarations
class Data1D;
class Data2D;
class Data3D;
class PairPotential;

// Export Module
class ExportModule : public Module
{
	/*
	 * Export data in various formats
	 */

	public:
	// Constructor
	ExportModule();
	// Destructor
	~ExportModule();


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	Module* createInstance() const;


	/*
	 * Definition
	 */
	public:
	// Return type of module
	const char* type() const;
	// Return category for module
	const char* category() const;
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
	// Filename and format for coordinate export
	CoordinateExportFileFormat coordinatesFormat_;
	// Basename and format for PairPotential export
	PairPotentialExportFileFormat pairPotentialFormat_;
	// Filename and format for trajectory export
	TrajectoryExportFileFormat trajectoryFormat_;


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
	// Write Configuration in specified format through parser
	static bool writeCoordinates(CoordinateExportFileFormat::CoordinateExportFormat format, LineParser& parser, Configuration* cfg);
	// Write Configuration in specified format to file
	static bool writeCoordinates(CoordinateExportFileFormat::CoordinateExportFormat format, const char* filename, Configuration* cfg);
	// Write Configuration as XYZ
	static bool writeXYZCoordinates(LineParser& parser, Configuration* cfg);
	// Write Configuration as DL_POLY CONFIG
	static bool writeDLPOLYCoordinates(LineParser& parser, Configuration* cfg);


	/*
	 * Static Functions - Data2D
	 */
	public:
	// Write Data2D in specified format through parser
	static bool writeData2D(Data2DExportFileFormat::Data2DExportFormat format, LineParser& parser, Data2D& data);
	// Write Data2D in specified format to file
	static bool writeData2D(Data2DExportFileFormat::Data2DExportFormat format, const char* filename, Data2D& data);
	// Write Data2D as simple block data
	static bool writeBlockData2D(LineParser& parser, Data2D& data);


	/*
	 * Static Functions - PairPotentials
	 */
	public:
	// Write PairPotential in specified format through parser
	static bool writePairPotential(PairPotentialExportFileFormat::PairPotentialExportFormat format, LineParser& parser, PairPotential* pp);
	// Write PairPotential in specified format to file
	static bool writePairPotential(PairPotentialExportFileFormat::PairPotentialExportFormat format, const char* filename, PairPotential* pp);
	// Write PairPotential as simple block data
	static bool writeBlockPairPotential(LineParser& parser, PairPotential* pp);
	// Write PairPotential as a DL_POLY TABLE file
	static bool writeDLPOLYTABLEPairPotential(LineParser& parser, PairPotential* pp);


	/*
	 * Static Functions - Trajectory
	 */
	public:
	// Append trajectory in specified format to file
	static bool writeTrajectory(TrajectoryExportFileFormat::TrajectoryExportFormat format, const char* filename, Configuration* cfg);
	// Append XYZ frame to trajectory
	static bool writeXYZTrajectory(LineParser& parser, Configuration* cfg);
};

#endif

