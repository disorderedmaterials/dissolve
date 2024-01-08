// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/widgets/integerSpin.hui"
#include <QFocusEvent>
#include <QLineEdit>

IntegerSpin::IntegerSpin(QWidget *parent) : QAbstractSpinBox(parent)
{
    // Set up validator
    lineEdit()->setValidator(&validator_);

    blockSignals(true);
    setValue(value_);
    blockSignals(false);

    connect(lineEdit(), SIGNAL(editingFinished()), this, SLOT(updateValueFromText()));
    connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(returnPressed()));
}

/*
 * Data
 */

// Set value
void IntegerSpin::setValue(int value)
{
    // Copy the existing value for comparison
    auto previousValueText = valueText_;

    if (minimumValue_ && value <= minimumValue_.value())
    {
        value_ = minimumValue_.value();
        valueText_ = specialValueText().isEmpty() ? QString::number(value_) : specialValueText();
    }
    else if (maximumValue_ && value >= maximumValue_)
    {
        value_ = maximumValue_.value();
        valueText_ = QString::number(value_);
    }
    else
    {
        value_ = value;
        valueText_ = QString::number(value_);
    }

    // Set the line edit's text
    lineEdit()->setText(valueText_);

    // If the new text is the same as the existing, don't emit any signals
    if (previousValueText == valueText_)
        return;

    if (valueText_ == specialValueText())
        Q_EMIT(valueNullified());
    else
        Q_EMIT(valueChanged(value_));
}

void IntegerSpin::setValue(std::optional<int> value)
{
    if (value)
        setValue(value.value());
    else
        setValue(minimumValue_.value() - 1);
}

// Return current value
int IntegerSpin::value() const { return value_; }

// Set value step size
void IntegerSpin::setSingleStep(int stepSize) { stepSize_ = stepSize; }

// Set minimum value
void IntegerSpin::setMinimum(int value) { minimumValue_ = value; }

// Set minimum value
void IntegerSpin::setMaximum(int value) { maximumValue_ = value; }

// Set allowed value range
void IntegerSpin::setRange(int minValue, int maxValue)
{
    minimumValue_ = minValue;
    maximumValue_ = maxValue;
}

/*
 * Signals / Slots
 */

void IntegerSpin::returnPressed()
{
    lineEdit()->selectAll();
    updateValueFromText();
}

void IntegerSpin::updateValueFromText()
{
    if (lineEdit()->text() == valueText_)
        return;

    setValue(lineEdit()->text().toInt());
}

/*
 * Reimplementations
 */

// Focus events
void IntegerSpin::focusInEvent(QFocusEvent *event)
{
    if (valueText_ == specialValueText())
        lineEdit()->setText(QString::number(value_));
    QAbstractSpinBox::focusInEvent(event);
}
void IntegerSpin::focusOutEvent(QFocusEvent *e) { updateValueFromText(); }

// Step value by specified number of increments
void IntegerSpin::stepBy(int steps) { setValue(value_ + steps * stepSize_); }

// Return whether stepping is currently available
QAbstractSpinBox::StepEnabled IntegerSpin::stepEnabled() const
{
    auto atMin = minimumValue_ && value_ <= minimumValue_.value(), atMax = maximumValue_ && value_ >= maximumValue_.value();
    if (atMin && atMax)
        return QAbstractSpinBox::StepNone;
    else if (atMin)
        return QAbstractSpinBox::StepUpEnabled;
    else if (atMax)
        return QAbstractSpinBox::StepDownEnabled;

    return QAbstractSpinBox::StepDownEnabled | QAbstractSpinBox::StepUpEnabled;
}

// Size Hint
QSize IntegerSpin::sizeHint() const
{
    const QFontMetrics fm(fontMetrics());

    int h = lineEdit()->sizeHint().height();
    int w = fm.horizontalAdvance("1000000");

    return {w, h};
}