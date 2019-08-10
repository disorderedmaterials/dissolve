/*
	*** Keyword Widget - Vec3Integer
	*** src/gui/keywordwidgets/vec3integer_funcs.cpp
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

#include "gui/keywordwidgets/vec3integer.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "genericitems/listhelper.h"

// Constructor
Vec3IntegerKeywordWidget::Vec3IntegerKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : QWidget(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Setup our UI
	ui_.setupUi(this);

	refreshing_ = true;

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<Vec3IntegerKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword '%s' into Vec3IntegerKeyword.\n", keyword->keyword());
	else
	{
		// Set minimum and maximum values for each component
		ui_.Spin1->setRange(keyword_->hasValidationMin(0) ? keyword_->validationMin(0) : -1e6, keyword_->hasValidationMax(0) ? keyword_->validationMax(0) : 1e6);
		ui_.Spin2->setRange(keyword_->hasValidationMin(1) ? keyword_->validationMin(1) : -1e6, keyword_->hasValidationMax(1) ? keyword_->validationMax(1) : 1e6);
		ui_.Spin3->setRange(keyword_->hasValidationMin(2) ? keyword_->validationMin(2) : -1e6, keyword_->hasValidationMax(2) ? keyword_->validationMax(2) : 1e6);

		// Set current values
		ui_.Spin1->setValue(keyword_->asVec3Int().x);
		ui_.Spin2->setValue(keyword_->asVec3Int().y);
		ui_.Spin3->setValue(keyword_->asVec3Int().z);
	}

	// Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a QScrollArea)
	installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));

	refreshing_ = false;
}

/*
 * Signals / Slots
 */

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin1_valueChanged(int value)
{
	if (refreshing_) return;

	Vec3<int> newVec = keyword_->data();
	newVec.x = value;
	keyword_->setData(newVec);

	emit(keywordValueChanged());
}

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin2_valueChanged(int value)
{
	if (refreshing_) return;

	Vec3<int> newVec = keyword_->data();
	newVec.y = value;
	keyword_->setData(newVec);

	emit(keywordValueChanged());
}

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin3_valueChanged(int value)
{
	if (refreshing_) return;

	Vec3<int> newVec = keyword_->data();
	newVec.z = value;
	keyword_->setData(newVec);

	emit(keywordValueChanged());
}
/*
 * Update
 */

// Update value displayed in widget
void Vec3IntegerKeywordWidget::updateValue()
{
	refreshing_ = true;

	Vec3<int> v = keyword_->asVec3Int();

	ui_.Spin1->setValue(v.x);
	ui_.Spin2->setValue(v.y);
	ui_.Spin3->setValue(v.z);

	refreshing_ = false;
}
