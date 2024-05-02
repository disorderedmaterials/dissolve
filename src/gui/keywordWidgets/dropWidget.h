// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QWidget>

class DropWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    DropWidget(QWidget *parent = nullptr);

    /*
     * Reimplementations
     */
    private:
    // Hide event
    void hideEvent(QHideEvent *event);

    /*
     * Signals
     */
    Q_SIGNALS:
    void hidden();
};
