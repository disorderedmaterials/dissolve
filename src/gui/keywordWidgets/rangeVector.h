// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_rangeVector.h"
#include "gui/models/rangeVectorModel.h"
#include "keywords/rangeVector.h"
#include "math/range.h"
#include <QWidget>

class RangeVectorKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    RangeVectorKeywordWidget(QWidget *parent, RangeVectorKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    RangeVectorKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::RangeVectorWidget ui_;
    // Model for combo box
    RangeVectorModel rangeVectorModel_;

    private slots:
    void modelDataChanged(const QModelIndex &, const QModelIndex &);
    void on_RangeAddButton_clicked(bool checked);
    void on_RangeRemoveButton_clicked(bool checked);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Reset model data
    void resetModelData();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Returns the index of the selected row
    int selectedRow() const;
};