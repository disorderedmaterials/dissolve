// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/models/procedureModel.h"
#include "gui/ui_procedurewidget.h"

// Forward Declarations
class DissolveWindow;
class NodeControlWidget;

// Procedure Widget
class ProcedureWidget : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ProcedureWidget(QWidget *parent = nullptr);
    ~ProcedureWidget() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ProcedureWidget ui_;
    // Pointer to the main Dissolve window
    DissolveWindow *dissolveWindow_{nullptr};
    // Procedure to display
    OptionalReferenceWrapper<Procedure> procedure_;
    // Model for procedure
    ProcedureModel procedureModel_;

    public:
    // Set up widget
    void setUp(DissolveWindow *dissolveWindow, Procedure &proc);

    /*
     * Widgets
     */
    private:
    // Return control widget for the specified node (if it exists)
    NodeControlWidget *getControlWidget(ConstNodeRef node, bool setAsCurrent = false);
    // Remove control widget for the specified node (if it exists)
    void removeControlWidget(ConstNodeRef node);

    private slots:
    void selectedNodeChanged(const QModelIndex &);
    void on_ShowAvailableNodesButton_clicked(bool checked);
    void nodeNameChanged(const QModelIndex &, const QString &oldName, const QString &newName);
    void layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &);
    void updateNodeList();
    void on_NodesList_customContextMenuRequested(const QPoint &pos);
    void on_AvailableNodesTree_doubleClicked(const QModelIndex &index);

    public:
    // Remove all node control widgets
    void removeControlWidgets();

    /*
     * Update
     */
    protected:
    // Update controls
    void updateControls();
    // Prevent procedure editing
    void preventEditing();
    // Allow procedure editing
    void allowEditing();
};
