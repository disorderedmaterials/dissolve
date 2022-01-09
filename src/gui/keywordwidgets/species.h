// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_species.h"
#include "gui/models/speciesModel.h"
#include "keywords/species.h"
#include <QWidget>

class SpeciesKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesKeywordWidget(QWidget *parent, SpeciesKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    SpeciesKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::SpeciesKeywordWidget ui_;
    // Model for combo box
    SpeciesModel speciesModel_;

    private slots:
    // Value changed
    void on_SpeciesCombo_currentIndexChanged(int index);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue() override;
};
