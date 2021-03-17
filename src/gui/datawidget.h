// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/ui_datawidget.h"

// Forward Declarations
/* none */

// Data Widget
class DataWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    DataWidget(QWidget *parent = 0);
    ~DataWidget();

    private:
    // Main form declaration
    Ui::DataWidget ui_;
    // Lock for widget refresh
    Lock refreshLock_;

    public:
    // Return contained DataViewer
    DataViewer *dataViewer();

    /*
     * Tools
     */
    private slots:
    // Interaction
    void on_InteractionViewButton_clicked(bool checked);
    // Graph
    void on_GraphResetButton_clicked(bool checked);
    void on_GraphFollowAllButton_clicked(bool checked);
    void on_GraphFollowXButton_clicked(bool checked);
    void on_GraphFollowXLengthSpin_valueChanged(double value);
    // View
    void on_ViewTypeCombo_currentIndexChanged(int index);
    void on_ViewLinkedViewButton_clicked(bool checked);
    void on_ViewToggleDataButton_clicked(bool checked);
    void on_ViewAxesVisibleButton_clicked(bool checked);
    void on_ViewCopyToClipboardButton_clicked(bool checked);

    /*
     * Update Functions
     */
    private:
    // Data tree top-level item update function
    void dataTreeTopLevelUpdateFunction(QTreeWidget *treeWidget, int topLevelItemIndex, RenderableGroup &data, bool createItem);
    // Data tree item update function
    void dataTreeItemUpdateFunction(QTreeWidgetItem *parentItem, int childIndex, std::shared_ptr<Renderable> &data,
                                    bool createItem);

    private slots:
    // Data tree item changed
    void on_DataTree_itemChanged(QTreeWidgetItem *item, int column);

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
