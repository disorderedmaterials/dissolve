// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "gui/gui.h"
#include <QObject>

class ScaleChargesDialogModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double value READ value NOTIFY valueSet)

    public:
    ScaleChargesDialogModel();
    ~ScaleChargesDialogModel();

    // Enum type to differentiate between usage options for dialog
    // Can either "Scale" the current value, or "ScaleTo" a given value 
    typedef enum
    {
        Scale,
        ScaleTo
    } Option;
    Q_ENUM(Option)

    // User has chosen either to apply Scale or ScaleTo to the charges
    Q_INVOKABLE void setOption(Option);
    // Sets the new selected scale value
    Q_INVOKABLE void updateValue(double);

    // Returns the current option
    Option getOption();

    // Returns the current scale value
    double value() const;
    // Apply scaling operation to species atoms
    void scaleCharges(Species*, DissolveWindow*, bool);

    private:
    double currentValue_{1.0};
    Option scaleType_;

    Q_SIGNALS:
    void valueSet();
    void cancelSelection();
    void acceptSelection();
};