// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QObject>

class ScaleChargesDialogModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double value READ value NOTIFY valueSet)

    public:
    ScaleChargesDialogModel();
    ~ScaleChargesDialogModel();

    bool scale_, scaleTo_;

    Q_INVOKABLE void setOption(int);
    Q_INVOKABLE void updateValue(double);

    double value() const;

    private:
    double currentValue_ = 1.0;

    Q_SIGNALS:
    void valueSet();
    void cancelSelection();
    void acceptSelection();
};