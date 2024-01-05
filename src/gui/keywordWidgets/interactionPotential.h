// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_interactionPotential.h"
#include "gui/models/enumOptionsModel.h"
#include "keywords/interactionPotential.h"
#include <QWidget>

class InteractionPotentialKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    InteractionPotentialKeywordWidget(QWidget *parent, InteractionPotentialBaseKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    InteractionPotentialBaseKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::InteractionPotentialWidget ui_;
    // Model for functional form options
    EnumOptionsModel formModel_;

    private slots:
    void on_ParametersEdit_editingFinished();
    void on_ParametersEdit_returnPressed();
    void on_FormCombo_currentIndexChanged(int index);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values, returning if parameters were set successfully
    bool updateKeywordData();
};
