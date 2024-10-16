// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/doubleExp.h"
#include <QAbstractSpinBox>

// Spinbox with Exponential Notation
class ExponentialSpin : public QAbstractSpinBox
{
    Q_OBJECT

    public:
    explicit ExponentialSpin(QWidget *parent = 0);

    /*
     * Data
     */
    private:
    // Current value
    DoubleExp value_;
    // Text representation of current value
    QString valueText_;
    // Threshold (10^[+/-]n) for switching to exponential notation
    const int exponentFormatThreshold_{3};
    // Value step size
    double stepSize_{0.1};
    // Number of decimals for text representation
    int nDecimals_{3};
    // Validator for line edit
    QDoubleValidator validator_;
    // Limiting values
    std::optional<double> minimumValue_, maximumValue_;

    public:
    // Set value
    void setValue(double value);
    void setValue(std::optional<double> value);
    // Return current value
    double value() const;
    // Set value step size
    void setSingleStep(double stepSize);
    // Set number of decimals for text representation
    void setDecimals(int n);
    // Set minimum value
    void setMinimum(double value);
    // Set maximum value
    void setMaximum(double value);
    // Set allowed value range
    void setRange(double minValue, double maxValue);
    // Remove range limits
    void removeLimits();

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    void returnPressed();
    public Q_SLOTS:
    void updateValueFromText();

    Q_SIGNALS:
    void valueChanged(double);
    void valueNullified();

    /*
     * Reimplementations
     */
    public:
    // Focus events
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    // Step value by specified number of increments
    void stepBy(int steps) override;
    // Return whether stepping is currently available
    QAbstractSpinBox::StepEnabled stepEnabled() const override;
    // Size Hint
    QSize sizeHint() const override;
};
