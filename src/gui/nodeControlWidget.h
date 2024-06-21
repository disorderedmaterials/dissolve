// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/ui_nodeControlWidget.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class Node;

// Node Control Widget
class NodeControlWidget : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    NodeControlWidget(DissolveWindow *dissolveWindow, NodeRef node);
    ~NodeControlWidget() = default;

    private:
    // Lock for widget refresh
    Lock refreshLock_;

    /*
     * Node Target
     */
    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Associated Node
    NodeRef node_;

    public:
    // Return target node for the widget
    NodeRef node();

    /*
     * Update
     */
    public:
    // Update controls within widget
    void updateControls();
    // Disable editing
    void preventEditing();
    // Allow editing
    void allowEditing();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::NodeControlWidget ui_;

    public:
    // Prepare widget for deletion
    void prepareForDeletion();

    public Q_SLOTS:
    // Local keyword data changed
    void localKeywordChanged(int signalMask);
    // Global data mutated
    void globalDataMutated(int mutationFlags);

    Q_SIGNALS:
    // Notify that the node's data has been modified in some way
    void dataModified();
};
