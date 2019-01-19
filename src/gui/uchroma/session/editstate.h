/*
	*** EditState
	*** src/gui/uchroma/session/editstate.h
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

#ifndef DISSOLVE_UCHROMAEDITSTATE_H
#define DISSOLVE_UCHROMAEDITSTATE_H

#include "gui/uchroma/session/editstatedata.h"
#include "templates/objectstore.h"
#include "templates/vector3.h"

// Forward Declarations
class UChromaBase;

// Edit State
class EditState : public ListItem<EditState>
{
	public:
	// Constructor
	EditState(ObjectInfo target, int quantity);


	/*
	 * Target Object
	 */
	private:
	// Target data
	ObjectInfo targetObject_;


	/*
	 * Target Quantity
	 */
	public:
	// Axes edit state quantity enum
	enum AxesStateQuantity
	{
		AxesAutoTicksQuantity,
		AxesGridLinesFullQuantity,
		AxesGridLinesMajorQuantity,
		AxesGridLinesMajorStyleQuantity,
		AxesGridLinesMinorQuantity,
		AxesGridLinesMinorStyleQuantity,
		AxesInvertedQuantity,
		AxesLabelAnchorQuantity,
		AxesLabelOrientationQuantity,
		AxesLogarithmicQuantity,
		AxesMaximumQuantity,
		AxesMinimumQuantity,
		AxesMinorTicksQuantity,
		AxesPositionFractionalQuantity,
		AxesPositionIsFractionalQuantity,
		AxesPositionRealQuantity,
		AxesStretchQuantity,
		AxesTickDeltaQuantity,
		AxesTickDirectionQuantity,
		AxesTickFirstQuantity,
		AxesTickSizeQuantity,
		AxesTitleAnchorQuantity,
		AxesTitleOrientationQuantity,
		AxesTitleQuantity,
		AxesVisibleQuantity,
		nAxesStateQuantities
	};
	// Collection edit state quantity enum
	enum CollectionStateQuantity
	{
		CollectionAddQuantity,
		CollectionRemoveQuantity,
		CollectionTitleQuantity,
		CollectionZQuantity,
		nCollectionStateQuantities
	};
	// ViewPane edit state quantity enum
	enum ViewPaneStateQuantity
	{
		ViewPaneAddCollectionTargetQuantity,
		ViewPaneRemoveCollectionTargetQuantity,
		nViewPaneStateQuantities
	};

	protected:
	// Target quantity for edit
	int targetQuantity_;


	/*
	 * Target Data
	 */
	private:
	// Target index for change (if relevant)
	int targetIndex_;
	// Target sub-index for change (if relevant)
	int targetSubIndex_;
	// Old data
	List<EditStateData> oldData_;
	// New data
	List<EditStateData> newData_;

	private:
	// Search for specified integer data in list
	int integerData(const char* name, const List<EditStateData>& dataList, bool& result);
	// Search for specified double data in list
	double doubleData(const char* name, const List<EditStateData>& dataList, bool& result);
	// Search for specified string data in list
	const char* stringData(const char* name, const List<EditStateData>& dataList, bool& result);
	// Search for specified Collection data in list
	Collection& collectionData(const char* name, const List<EditStateData>& dataList, bool& result);
	// Search for specified Data1D data in list
	Data1D& data2DData(const char* name, const List<EditStateData>& dataList, bool& result);
	// Search for specified LineStyle data in list
	LineStyle lineStyleData(const char* name, const List<EditStateData>& dataList, bool& result);

	public:
	// Set target indices
	void setTargetIndices(int index, int subIndex);
	// Add old data
	EditStateData* addOldData();
	// Add new data
	EditStateData* addNewData();


	/*
	 * Apply / Revert
	 */
	protected:
	// Apply change to Axes
	bool changeAxes(const List<EditStateData>& data, bool revert);
	// Apply change to Collection
	bool changeCollection(const List<EditStateData>& data, bool revert);
	// Apply change to ViewPane
	bool changeViewPane(const List<EditStateData>& data, bool revert);

	public:
	// Apply changes
	bool makeChanges(UChromaBase* uChromaBase, bool revert);
};

#endif
