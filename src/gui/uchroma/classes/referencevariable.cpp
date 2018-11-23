/*
	*** ReferenceVariable
	*** src/gui/uchroma/classes/referencevariable.cpp
	Copyright T. Youngs 2012-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/classes/referencevariable.h"
#include "base/messenger.h"
#include "expression/variable.h"

// Constructor
ReferenceVariable::ReferenceVariable() : ListItem<ReferenceVariable>()
{
	// Variable Target
	variable_ = NULL;
	used_ = false;

	// Reference Definition
	sourceCollection_ = NULL;
	currentReferenceRange_ = NULL;
}

// Destructor
ReferenceVariable::~ReferenceVariable()
{
}

// Copy constructor
ReferenceVariable::ReferenceVariable(const ReferenceVariable& source)
{
	(*this) = source;
}

// Assignment operator
void ReferenceVariable::operator=(const ReferenceVariable& source)
{
	// Variable Target
	variable_ = source.variable_;
	used_ = source.used_;
	name_ = source.name_;
	
	// Reference Definition
	sourceCollection_ = source.sourceCollection_;
	xIndex_ = source.xIndex_;
	zIndex_ = source.zIndex_;
	zDataSetName_ = source.zDataSetName_;
}

// Set name
void ReferenceVariable::setName(const char* name)
{
	// We do not update the associated Variable's name here - it is up to the calling function to check this using updateVariable()
	name_ = name;
}

// Return name
const char* ReferenceVariable::name()
{
	return name_;
}

// Set variable target
void ReferenceVariable::setVariable(Variable* variable)
{
	variable_ = variable;
}

// Reset variable target (to NULL) and usage status
void ReferenceVariable::resetVariable()
{
	variable_ = NULL;
	used_ = false;
}

// Return variable target
Variable* ReferenceVariable::variable()
{
	return variable_;
}

// Update associated variable to reflect name of ReferenceVariable
bool ReferenceVariable::updateVariable()
{
	if (!variable_) return false;
	if (name_ == variable_->name()) return false;
	variable_->setName(name_);
	return true;
}

// Set whether this variable is used in the current equation
void ReferenceVariable::setUsed(bool used)
{
	used_ = used;
}

// Return whether this variable is used in the current equation
bool ReferenceVariable::used()
{
	return used_;
}

/*
 * Reference Definition
 */

// Set source collection
void ReferenceVariable::setSourceCollection(Collection* source)
{
	sourceCollection_ = source;
}

// Return source collection
Collection* ReferenceVariable::sourceCollection()
{
	return sourceCollection_;
}

// Return x index data
IndexData& ReferenceVariable::xIndex()
{
	return xIndex_;
}

// Return Z index data
IndexData& ReferenceVariable::zIndex()
{
	return zIndex_;
}

// Set Z DataSet name
void ReferenceVariable::setZDataSetName(const char* name)
{
	zDataSetName_ = name;
}

// Return Z DataSet name
const char* ReferenceVariable::zDataSetName()
{
	return zDataSetName_;
}

/*
 * Reference Data
 */

// Reset current reference DataSpaceRange to the first available
void ReferenceVariable::resetCurrentDataSpaceRange()
{
	currentReferenceRange_ = referenceSpace_.dataSpaceRanges();
}

// Set internal pointer to the next available DataSpaceRange
void ReferenceVariable::moveToNextDataSpaceRange()
{
	currentReferenceRange_ = currentReferenceRange_->next;
}

// Generate reference data
bool ReferenceVariable::initialiseDataSpace(Collection* fitCollection, DataSpace& fitDataSpace)
{
	// Check that supplied target fitCollection is the same as the one we have stored
	if (fitCollection != sourceCollection_)
	{
		Messenger::error("Cannot currently use a reference Collection which is different from the target fit collection.\n");
		return false;
	}

	// Create the dataspace, matching that of the fit collection
	if (!referenceSpace_.initialise(fitDataSpace, true)) return false;

	// Generate values within the dataspace, employing offsets defined in the ReferenceVariable
	for (DataSpaceRange* range = referenceSpace_.dataSpaceRanges(); range != NULL; range = range->next)
	{
		range->copyValues(xIndex_, zIndex_);
	}

	return true;
}

// Update value of target variable with DataSpaceRange indices provided
bool ReferenceVariable::updateValue(int rangeXIndex, int rangeZIndex)
{
	// Check target variable
	if (!variable_) return false;

	// Check current range
	if (!currentReferenceRange_) return false;

	// Set value
	variable_->set(currentReferenceRange_->referenceY(rangeXIndex, rangeZIndex));
	
	return true;
}
