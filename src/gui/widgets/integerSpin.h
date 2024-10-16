// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractSpinBox>

// Integer SpinBox with Special Limit
class IntegerSpin : public QAbstractSpinBox
{
    Q_OBJECT

    public:
    explicit IntegerSpin(QWidget *parent = 0);

    /*
     * Data
     */
    private:
    // Current value
    int value_;
    // Text representation of current value
    QString valueText_;
    // Value step size
    int stepSize_{1};
    // Limiting values
    std::optional<int> minimumValue_, maximumValue_;
    // Validator for line edit
    QIntValidator validator_;

    public:
    // Set value
    void setValue(int value);
    void setValue(std::optional<int> value);
    // Return current value
    int value() const;
    // Set value step size
    void setSingleStep(int stepSize);
    // Set minimum value
    void setMinimum(int value);
    // Set maximum value
    void setMaximum(int value);
    // Set allowed value range
    void setRange(int minValue, int maxValue);
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
    void valueChanged(int);
    void valueNullified();

    /*
     * Reimplementations
     */
    public:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    // Step value by specified number of increments
    void stepBy(int steps) override;
    // Return whether stepping is currently available
    QAbstractSpinBox::StepEnabled stepEnabled() const override;
    // Size Hint
    QSize sizeHint() const override;
};
