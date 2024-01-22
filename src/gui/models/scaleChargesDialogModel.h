// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include <QObject>

class ScaleChargesDialogModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double value READ value NOTIFY valueSet)

    public:
    ScaleChargesDialogModel();
    ~ScaleChargesDialogModel();

    typedef enum
    {
        Scale,
        ScaleTo
    } Option;
    Q_ENUM(Option)

    Q_INVOKABLE void setOption(Option);
    Q_INVOKABLE void updateValue(double);

    Option getOption();

    double value() const;
    bool scaleCharges(Species *);

    private:
    double currentValue_ = 1.0;
    Option scaleType_;

    Q_SIGNALS:
    void valueSet();
    void cancelSelection();
    void acceptSelection();
};