// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/widgets/exponentialSpin.hui"
#include <QFocusEvent>
#include <QLineEdit>

ExponentialSpin::ExponentialSpin(QWidget *parent) : QAbstractSpinBox(parent)
{
    // Set up validator
    validator_.setNotation(QDoubleValidator::ScientificNotation);
    lineEdit()->setValidator(&validator_);

    blockSignals(true);
    setValue(value_.value());
    blockSignals(false);

    connect(lineEdit(), SIGNAL(editingFinished()), this, SLOT(updateValueFromText()));
    connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(returnPressed()));
}

/*
 * Data
 */

// Set value
void ExponentialSpin::setValue(double value)
{
    // Copy the existing value for comparison
    auto previousValueText = valueText_;

    if (minimumValue_ && value <= minimumValue_.value())
    {
        value_ = minimumValue_.value();
        valueText_ = specialValueText().isEmpty()
                         ? QString::fromStdString(value_.asString(exponentFormatThreshold_, nDecimals_))
                         : specialValueText();
    }
    else if (maximumValue_ && value >= maximumValue_)
    {
        value_ = maximumValue_.value();
        valueText_ = QString::fromStdString(value_.asString(exponentFormatThreshold_, nDecimals_));
    }
    else
    {
        value_ = value;
        valueText_ = QString::fromStdString(value_.asString(exponentFormatThreshold_, nDecimals_));
    }

    // Set the line edit's text
    lineEdit()->setText(valueText_);

    // If the new text is the same as the existing, don't emit any signals
    if (previousValueText == valueText_)
        return;

    if (valueText_ == specialValueText())
        Q_EMIT(valueNullified());
    else
        Q_EMIT(valueChanged(value_.value()));
}

void ExponentialSpin::setValue(std::optional<double> value)
{
    if (value)
        setValue(value.value());
    else
        setValue(minimumValue_.value() - 1.0);
}

// Return current value
double ExponentialSpin::value() const { return value_.value(); }

// Set value step size
void ExponentialSpin::setSingleStep(double stepSize) { stepSize_ = stepSize; }

// Set number of decimals for text representation
void ExponentialSpin::setDecimals(int n) { nDecimals_ = n; }

// Set minimum value
void ExponentialSpin::setMinimum(double value) { minimumValue_ = value; }

// Set minimum value
void ExponentialSpin::setMaximum(double value) { maximumValue_ = value; }

// Set allowed value range
void ExponentialSpin::setRange(double minValue, double maxValue)
{
    minimumValue_ = minValue;
    maximumValue_ = maxValue;
}

/*
 * Signals / Slots
 */

void ExponentialSpin::returnPressed()
{
    lineEdit()->selectAll();
    updateValueFromText();
}

void ExponentialSpin::updateValueFromText()
{
    if (lineEdit()->text() == valueText_)
        return;

    setValue(lineEdit()->text().toDouble());
}

/*
 * Reimplementations
 */

// Focus events
void ExponentialSpin::focusInEvent(QFocusEvent *event)
{
    if (valueText_ == specialValueText())
        lineEdit()->setText(QString::fromStdString(value_.asString(exponentFormatThreshold_, nDecimals_)));
    QAbstractSpinBox::focusInEvent(event);
}
void ExponentialSpin::focusOutEvent(QFocusEvent *e) { updateValueFromText(); }

// Step value by specified number of increments
void ExponentialSpin::stepBy(int steps) { setValue(value_.value() + steps * stepSize_); }

// Return whether stepping is currently available
QAbstractSpinBox::StepEnabled ExponentialSpin::stepEnabled() const
{
    auto atMin = minimumValue_ && value_.value() <= minimumValue_.value(),
         atMax = maximumValue_ && value_.value() >= maximumValue_.value();
    if (atMin && atMax)
        return QAbstractSpinBox::StepNone;
    else if (atMin)
        return QAbstractSpinBox::StepUpEnabled;
    else if (atMax)
        return QAbstractSpinBox::StepDownEnabled;

    return QAbstractSpinBox::StepDownEnabled | QAbstractSpinBox::StepUpEnabled;
}

// Size Hint
QSize ExponentialSpin::sizeHint() const
{
    const QFontMetrics fm(fontMetrics());

    int h = lineEdit()->sizeHint().height();
    int w = fm.horizontalAdvance("1.000000000e-5");

    return {w, h};
}
