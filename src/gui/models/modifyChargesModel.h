// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include <QObject>

class ModifyChargesModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double scaleValue READ scaleValue NOTIFY valueSet)
    Q_PROPERTY(double smoothValue READ smoothValue NOTIFY valueSet)
    Q_PROPERTY(int sigFigValue READ sigFigValue NOTIFY valueSet)

    public:
    ModifyChargesModel();
    ~ModifyChargesModel();

    // Enum type to differentiate between operations for mofiying charges
    enum Modify { Scaling, Smoothing, ReduceSigFig };
    Q_ENUM(Modify)

    // Enum type to differentiate between usage options for dialog
    // Can either "Scale" the current value, or "ScaleTo" a given value
    enum ScaleType { Scale, ScaleTo };
    Q_ENUM(ScaleType)

    // Sets the new selected scale value
    Q_INVOKABLE void updateScaleValue(double);
    // Sets the new selected smooth value
    Q_INVOKABLE void updateSmoothValue(double);
    // Sets the new selected significant figures
    Q_INVOKABLE void updateSigFigValue(int);

    // User has chosen either to apply Scale or ScaleTo to the charges
    Q_INVOKABLE void setScaleType(ScaleType);
    // Returns the current scale type option
    ScaleType getScaleType();
    // Returns the current modifier option
    Modify getModifier();
    // Sets the current modifier option
    void setModifier(Modify);
    // Returns the current scale value
    double scaleValue() const;
    // Returns the current smooth value
    double smoothValue() const;
    // Returns the current significant figures
    int sigFigValue() const;
    // Apply scaling operation to species atoms charges
    bool scale(Species*, bool);
    // Apply smoothing operation to species atoms charges
    void smooth(Species*);
    // Reduce significant figures on species atoms charges
    void reduceSignificantFigures(Species*);
    // Sets a pointer to the dialog's spinbox
    void setSpinBox(QObject*);

    public Q_SLOTS:
    // Resets the spinbox to the current modifier value
    void resetSpinBox();

    private:
    double currentScaleValue_{1.0};
    double currentSmoothValue_{0.0};
    int currentSigFigValue_{3};

    ScaleType scaleType_;
    Modify modify_;

    QObject *spinBox_;

    Q_SIGNALS:
    void valueSet();
    void cancelSelection();
    void acceptSelection();
};