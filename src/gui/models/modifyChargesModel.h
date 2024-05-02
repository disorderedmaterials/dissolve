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
    enum ModifyBy
    {
        Scaling,
        Smoothing,
        ReduceSigFig
    };
    Q_ENUM(ModifyBy)

    // Enum type to differentiate between usage options for dialog
    // Can either "Scale" the current value, or "ScaleTo" a given value
    enum ScaleType
    {
        Scale,
        ScaleTo
    };
    Q_ENUM(ScaleType)

    private:
    // Contains the current value to be used for scaling charges
    double currentScaleValue_{1.0};
    // Contains the current value to be used for smoothing charges
    double currentSmoothValue_{0.0};
    // Contains the current value to be used for reducing significant figures on charges
    int currentSigFigValue_{3};

    // Contains the current setting for scale type (Scale or ScaleTo)
    ScaleType scaleType_;
    // Contains the attribute (scaling, smoothing, sig fig) that will currently be modified by the model
    ModifyBy modify_;

    public:
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
    ModifyBy getModifier();
    // Sets the current modifier option
    void setModifier(ModifyBy);
    // Returns the current scale value
    double scaleValue() const;
    // Returns the current smooth value
    double smoothValue() const;
    // Returns the current significant figures
    int sigFigValue() const;
    // Apply scaling operation to species atoms charges
    bool scale(Species *, bool);
    // Apply smoothing operation to species atoms charges
    void smooth(Species *);
    // Reduce significant figures on species atoms charges
    void reduceSignificantFigures(Species *);

    Q_SIGNALS:
    void valueSet();
    void cancelSelection();
    void acceptSelection();
};