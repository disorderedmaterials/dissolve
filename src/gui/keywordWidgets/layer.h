// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_layer.h"
#include "gui/models/moduleLayersModel.h"
#include "keywords/layer.h"
#include <QWidget>

class LayerKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    LayerKeywordWidget(QWidget *parent, LayerKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    LayerKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::LayerKeywordWidget ui_;
    // Model for combo box
    ModuleLayersModel moduleLayersModel_;

    private slots:
    void on_LayerCombo_currentIndexChanged(int index);
    void on_ClearButton_clicked(bool checked);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Reset model data
    void updateLayersCombo();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
