// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/models/renderableGroupManagerModel.h"
#include "gui/ui_dataWidget.h"

// Data Widget
class DataWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    DataWidget(QWidget *parent = 0);
    ~DataWidget() = default;

    private:
    // Main form declaration
    Ui::DataWidget ui_;
    // Model for renderable groups
    RenderableGroupManagerModel renderableGroupManagerModel_;
    // Lock for widget refresh
    Lock refreshLock_;

    public:
    // Return contained DataViewer
    DataViewer *dataViewer();

    /*
     * Tools
     */
    private slots:
    // Graph
    void on_GraphResetButton_clicked(bool checked);
    void on_GraphFollowAllButton_clicked(bool checked);
    void on_GraphFollowXButton_clicked(bool checked);
    void on_GraphFollowXLengthSpin_valueChanged(double value);
    // View
    void on_ViewTypeCombo_currentIndexChanged(int index);
    void on_ViewToggleDataButton_clicked(bool checked);
    void on_ViewAxesVisibleButton_clicked(bool checked);
    void on_ViewCopyToClipboardButton_clicked(bool checked);

    /*
     * Update Functions
     */
    private slots:
    // Data tree item changed
    void renderableGroupManagerDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &);

    public slots:
    // Clear renderable data
    void clearRenderableData();
    // Update toolbar
    void updateToolbar();
    // Update status bar
    void updateStatusBar();
    // Update data tree
    void updateDataTree();
};
