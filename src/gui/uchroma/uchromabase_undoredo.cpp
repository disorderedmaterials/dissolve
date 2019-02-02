/*
	*** UChroma Base - Undo Redo
	*** src/gui/uchroma/uchromabase_undoredo.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/uchroma/uchromabase.h"

// Begin a new edit state group
EditStateGroup* UChromaBase::beginEditStateGroup(const char* format, ...)
{
	// If there are any groups after the current undo group, we must delete them before adding the new one
	if (undoEditStateGroup_)
	{
		while (undoEditStateGroup_->next) editStateGroups_.remove(undoEditStateGroup_->next);
	}
	else editStateGroups_.clear();

	va_list arguments;
	static char title[8096];
	title[0] = '\0';
	// Parse the argument list (...) and internally write the output string into title[]
	va_start(arguments, format);
	vsprintf(title, format, arguments);

	currentEditStateGroup_ = editStateGroups_.add();
	currentEditStateGroup_->setTitle(title);
	currentEditState_ = NULL;

	va_end(arguments);

	return currentEditStateGroup_;
}

// Add edit state to group
bool UChromaBase::addEditState(ObjectInfo target, int quantity, int index, int subIndex)
{
	if (!currentEditStateGroup_) return false;
	currentEditState_ = currentEditStateGroup_->addEditState(target, quantity);
	currentEditState_->setTargetIndices(index, subIndex);
	return true;
}

// Add edit state to group with basic integer data
bool UChromaBase::addEditState(ObjectInfo target, int quantity, int oldData, int newData, int index, int subIndex)
{
	if (!addEditState(target, quantity, index, subIndex)) return false;
	addEditStateData(false, "value", oldData);
	addEditStateData(true, "value", newData);
	return true;
}

// Add edit state to group with basic double data
bool UChromaBase::addEditState(ObjectInfo target, int quantity, double oldData, double newData, int index, int subIndex)
{
	if (!addEditState(target, quantity, index, subIndex)) return false;
	addEditStateData(false, "value", oldData);
	addEditStateData(true, "value", newData);
	return true;
}

// Add edit state to group with basic QString data
bool UChromaBase::addEditState(ObjectInfo target, int quantity, const char* oldData, const char* newData, int index, int subIndex)
{
	if (!addEditState(target, quantity, index, subIndex)) return false;
	addEditStateData(false, "value", oldData);
	addEditStateData(true, "value", newData);
	return true;
}

// Add edit state to group with LineStyle data
bool UChromaBase::addEditState(ObjectInfo target, int quantity, LineStyle oldData, LineStyle newData, int index, int subIndex)
{
	if (!addEditState(target, quantity, index, subIndex)) return false;
	addEditStateData(false, "value", oldData);
	addEditStateData(true, "value", newData);
	return true;
}

// Add data to current EditState (int)
void UChromaBase::addEditStateData(bool newData, const char* name, int value)
{
	if (!currentEditState_)
	{
		printf("Internal Error: Attempted to addEditStateData(int) with no currentEditState_.\n");
		return;
	}
	if (newData) currentEditState_->addNewData()->set(name, value);
	else currentEditState_->addOldData()->set(name, value);
}

// Add data to current EditState  (double)
void UChromaBase::addEditStateData(bool newData, const char* name, double value)
{
	if (!currentEditState_)
	{
		printf("Internal Error: Attempted to addEditStateData(double) with no currentEditState_.\n");
		return;
	}
	if (newData) currentEditState_->addNewData()->set(name, value);
	else currentEditState_->addOldData()->set(name, value);
}

// Add data to current EditState  (QString)
void UChromaBase::addEditStateData(bool newData, const char* name, const char* value)
{
	if (!currentEditState_)
	{
		printf("Internal Error: Attempted to addEditStateData(QString) with no currentEditState_.\n");
		return;
	}
	if (newData) currentEditState_->addNewData()->set(name, value);
	else currentEditState_->addOldData()->set(name, value);
}

// Add data to current EditState  (from Collection*)
void UChromaBase::addEditStateData(bool newData, const char* name, Collection* value)
{
	if (!currentEditState_)
	{
		printf("Internal Error: Attempted to addEditStateData(Collection*) with no currentEditState_.\n");
		return;
	}
	if (newData) currentEditState_->addNewData()->set(name, value);
	else currentEditState_->addOldData()->set(name, value);
}

// Add data to current EditState  (from Data1D*)
void UChromaBase::addEditStateData(bool newData, const char* name, Data1D* value)
{
	if (!currentEditState_)
	{
		printf("Internal Error: Attempted to addEditStateData(Data1D*) with no currentEditState_.\n");
		return;
	}
	if (newData) currentEditState_->addNewData()->set(name, value);
	else currentEditState_->addOldData()->set(name, value);
}

// Add data to current EditState  (from LineStyle&)
void UChromaBase::addEditStateData(bool newData, const char* name, LineStyle& value)
{
	if (!currentEditState_)
	{
		printf("Internal Error: Attempted to addEditStateData(LineStyle) with no currentEditState_.\n");
		return;
	}
	if (newData) currentEditState_->addNewData()->set(name, value);
	else currentEditState_->addOldData()->set(name, value);
}

// End the new edit state group
void UChromaBase::endEditStateGroup()
{
	if (!currentEditStateGroup_)
	{
		Messenger::print("Internal Error - No currentEditStateGroup_ to end.\n");
		return;
	}

	undoEditStateGroup_ = currentEditStateGroup_;
	currentEditStateGroup_ = NULL;

	// Update Edit menu
	updateUndoRedo();
}

// Return current EditStateGroup
EditStateGroup* UChromaBase::currentEditStateGroup()
{
	return currentEditStateGroup_;
}

// Return target EditStateGroup for undo
EditStateGroup* UChromaBase::undoEditStateGroup()
{
	return undoEditStateGroup_;
}

// Return first in list of EditStateGroups
EditStateGroup* UChromaBase::editStateGroups()
{
	return editStateGroups_.first();
}

// Clear all edit state data
void UChromaBase::clearEditStates()
{
	editStateGroups_.clear();
	currentEditStateGroup_ = NULL;
	currentEditState_ = NULL;
	undoEditStateGroup_ = NULL;
}

// Perform undo
bool UChromaBase::undo()
{
	bool result = false;
	if (undoEditStateGroup_)
	{
		// Revert changes in current group
		result = undoEditStateGroup_->revert(this);

		// Set new current undo state
		undoEditStateGroup_ = undoEditStateGroup_->prev;
	}

	return result;
}

// Perform redo
bool UChromaBase::redo()
{
	bool result = false;

	EditStateGroup* redoState = NULL;
	if (undoEditStateGroup_) redoState = undoEditStateGroup_->next;
	else redoState = editStateGroups_.first();

	if (redoState)
	{
		// Apply changes in redoState
		result = redoState->apply(this);

		// Set new current undo state
		undoEditStateGroup_ = redoState;
	}

	return result;
}
