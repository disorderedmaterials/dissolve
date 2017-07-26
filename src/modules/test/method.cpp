/*
	*** Test Module - Method
	*** src/modules/test/method.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/test/test.h"
#include "main/duq.h"
#include "base/sysfunc.h"

// Perform setup tasks for module
bool TestModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool TestModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool TestModule::process(DUQ& duq, ProcessPool& procPool)
{
	// First, check that we can find the total structure factors that have been specified
	sampleData_.clear();
	
	RefListIterator<Module,XYData> sampleIterator(sampleData_);
	while (Module* module = sampleIterator.iterate())
	{
		// First, need to grab GenericList that will contain the target
		GenericList& moduleData = module->configurationLocal() ? module->targetConfigurations().firstItem()->moduleData() : duq.processingModuleData();
		
	}

	// Create weights matrix based on our defined 
	return true;
}

// Execute post-processing stage
bool TestModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
