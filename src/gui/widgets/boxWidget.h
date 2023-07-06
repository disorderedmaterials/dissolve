// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "gui/widgets/ui_boxWidget.h"

class BoxWidget : public QWidget
{
    Q_OBJECT

    public:
    BoxWidget(QWidget *parent, Configuration *cfg);
    virtual ~BoxWidget();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::BoxWidget ui_;

    private:
    // Configuration data to display
    Configuration *configuration_;

    public:
    void updateDensityLabel();
    void update();

    private slots:
    void on_DensityUnitsCombo_currentIndexChanged(int index);
    /*
     * Event filer
     */
    protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};
