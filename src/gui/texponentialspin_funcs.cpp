/*
	*** TExponentialSpin Functions
	*** src/gui/texponentialspin_funcs.cpp
	Copyright T. Youngs 2016-2018

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

#include "gui/texponentialspin.hui"
#include <QLineEdit>
#include <stdio.h>

// Constructor
TExponentialSpin::TExponentialSpin(QWidget* parent) : QAbstractSpinBox(parent)
{
	// Set default values
	value_ = 0.0;
	valueMin_ = -10.0;
	valueMax_ = 10.0;
	valueStep_ = 1.0;
	limitMinValue_ = false;
	limitMaxValue_ = false;
	textChanged_ = false;

	// Set validator
	QRegExp regExp("[-+]?[0-9]*[.]?[0-9]+([eE][-+]?[0-9]+)?");
	validator_.setRegExp(regExp);
	lineEdit()->setValidator(&validator_);

	// Connect signals to slots
	connect(lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
	connect(lineEdit(), SIGNAL(editingFinished()), this, SLOT(updateValueFromText()));
	connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(updateValueFromText()));

	// Update initial text value
	updateTextFromValue();
}

/*
 * Data
 */

// Clamp current value to allowable range
bool TExponentialSpin::clamp()
{
	if (limitMinValue_ && (value_.value() < valueMin_))
	{
		value_ = valueMin_;
		return true;
	}
	else if (limitMaxValue_ && (value_.value() > valueMax_))
	{
		value_ = valueMax_;
		return true;
	}
	return false;
}

// Create text from current value, and display in lineEdit
void TExponentialSpin::updateTextFromValue(int precision)
{
// 	printf("Here we are in updateText, setting [%s].\n", qPrintable(value_.text(precision)));
	lineEdit()->setText(value_.asString().get());
	textChanged_ = false;
}

// Return double value
double TExponentialSpin::value()
{
	return value_.value();
}

// Set value
void TExponentialSpin::setValue(double value)
{
	// Is the new value different from the old one?
	bool different = (value_ != value);

	// Store number, and then clamp it to range
	value_ = value;
	clamp();

	// Update text
	updateTextFromValue();

	// Emit signal (only if new value is different)
	if (different) emit(valueChanged(value_.value()));
}

// Set minimum limit
void TExponentialSpin::setMinimum(double value)
{
	valueMin_ = value;
	limitMinValue_ = true;

	if (clamp()) updateTextFromValue();
}

// Set minimum limit
void TExponentialSpin::setMaximum(double value)
{
	valueMax_ = value;
	limitMaxValue_ = true;

	clamp();
	updateTextFromValue();
}

// Set allowable range of value
void TExponentialSpin::setRange(bool limitMin, double minValue, bool limitMax, double maxValue, int nSteps)
{
	valueMin_ = minValue;
	limitMinValue_ = limitMin;
	valueMax_ = maxValue;
	limitMaxValue_ = limitMax;

	// Set singlestep values, if nSteps > 0
	if (nSteps > 0) setSingleStep((maxValue - minValue) / nSteps);

	// Clamp current value if necessary
	if (clamp())
	{
		updateTextFromValue();
		emit(valueChanged(value_.value()));
		textChanged_ = false;
	}
}

// Remove range limits
void TExponentialSpin::setUnlimitedRange()
{
	limitMinValue_ = false;
	limitMaxValue_ = false;
}

// Set single step value
void TExponentialSpin::setSingleStep(double step)
{
	valueStep_ = step;
}

// Return number of decimals to use when converting to text
int TExponentialSpin::decimals()
{
	return decimals_;
}

// Set number of decimals to use when converting to text
void TExponentialSpin::setDecimals(int nDecimals)
{
	decimals_ = nDecimals;
}

// Return suffix for value
QString TExponentialSpin::suffix()
{
	return suffix_;
}

// Set suffix for value
void TExponentialSpin::setSuffix(QString suffix)
{
	suffix_ = suffix;
}

/*
 * Slots (Private)
 */

// Update value from current text
void TExponentialSpin::updateValueFromText()
{
	if (!textChanged_) return;

	// If the line edit is empty, we will revert to the previous value
	if (lineEdit()->text().isEmpty()) updateTextFromValue();
	else
	{
		value_.set(qPrintable(lineEdit()->text()));
		if (clamp()) updateTextFromValue();
	}

	textChanged_ = false;
	emit(valueChanged(value_.value()));
}

// Flag that the text has been modified since the last emit of valueChanged()
void TExponentialSpin::textChanged(QString text)
{
	textChanged_ = true;
}

/*
 * Virtual Overloads
 */

// Step value by specified amount
void TExponentialSpin::stepBy(int nSteps)
{
	value_ = value_.value() + valueStep_*nSteps;

	// Check new value and update text
	clamp();
	updateTextFromValue();
	emit(valueChanged(value_.value()));
	textChanged_ = false;
}

// Return which steps should be enabled
QAbstractSpinBox::StepEnabled TExponentialSpin::stepEnabled() const
{
	bool up = (value_.value() < valueMax_) || (!limitMaxValue_);
	bool down = (value_.value() > valueMin_) || (!limitMinValue_);

	if (up && down) return (QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled);
	else if (up) return QAbstractSpinBox::StepUpEnabled;
	else if (down) return QAbstractSpinBox::StepDownEnabled;
	else return QAbstractSpinBox::StepNone;
}
