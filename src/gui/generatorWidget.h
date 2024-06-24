// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/generatorModel.h"
#include "gui/models/nodePaletteModel.h"
#include "gui/ui_generatorWidget.h"

// Forward Declarations
class DissolveWindow;
class NodeControlWidget;

// Generator Widget
class GeneratorWidget : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    GeneratorWidget(QWidget *parent = nullptr);
    ~GeneratorWidget() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::GeneratorWidget ui_;
    // Pointer to the main Dissolve window
    DissolveWindow *dissolveWindow_{nullptr};
    // Generator to display
    OptionalReferenceWrapper<Generator> generator_;
    // Model for generator
    GeneratorModel generatorModel_;
    // Model for node palette
    NodePaletteModel nodePaletteModel_;

    public:
    // Set up widget
    void setUp(DissolveWindow *dissolveWindow, Generator &proc);

    /*
     * Widgets
     */
    private:
    // Return control widget for the specified node (if it exists)
    NodeControlWidget *getControlWidget(ConstNodeRef node, bool setAsCurrent = false);
    // Remove control widget for the specified node (if it exists)
    void removeControlWidget(ConstNodeRef node);

    private Q_SLOTS:
    void selectedNodeChanged(const QItemSelection &selected);
    void on_ExpandAllButton_clicked(bool checked);
    void on_CollapseAllButton_clicked(bool checked);
    void on_ShowContextButton_clicked(bool checked);
    void on_ShowAvailableNodesButton_clicked(bool checked);
    void nodeNameChanged(const QModelIndex &, const QString &oldName, const QString &newName);
    void generatorDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &);
    void updateNodeTree();
    void on_NodesTree_customContextMenuRequested(const QPoint &pos);
    void on_AvailableNodesTree_doubleClicked(const QModelIndex &index);
    void on_DeleteNodeButton_clicked(bool checked);

    public:
    // Remove all node control widgets
    void removeControlWidgets();

    /*
     * Update
     */
    protected:
    // Update controls
    void updateControls();
    // Prevent generator editing
    void preventEditing();
    // Allow generator editing
    void allowEditing();
};
