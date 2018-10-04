/*
	*** Export Module
	*** src/modules/export/export.h
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

#ifndef DISSOLVE_EXPORTMODULE_H
#define DISSOLVE_EXPORTMODULE_H

#include "module/module.h"
#include "modules/export/formats.h"

// Forward Declarations
class Data1D;
class Data2D;
class Data3D;

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
	// Return instance type for module
	InstanceType instanceType() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;
	// Modules upon which this Module depends to have run first
	const char* dependentModules();
	// Set up supplied dependent module (only if it has been auto-added)
	bool setUpDependentModule(Module* depMod);


	/*
	 * Options
	 */
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


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
	// Write Configuration in specified format
	static bool writeCoordinates(CoordinateExportFileFormat::CoordinateExportFormat format, LineParser& parser, Configuration* cfg);
	// Write Configuration as XYZ
	static bool writeXYZCoordinates(LineParser& parser, Configuration* cfg);
	// Write Configuration as DL_POLY CONFIG
	static bool writeDLPOLYCoordinates(LineParser& parser, Configuration* cfg);


	/*
	 * Static Functions - Data2D
	 */
	public:
	// Write Data2D in specified format
	static bool writeData2D(Data2DExportFileFormat::Data2DExportFormat format, LineParser& parser, Data2D& data);
	// Write Data2D as simple block data
	static bool writeBlockData2D(LineParser& parser, Data2D& data);
};

#endif

