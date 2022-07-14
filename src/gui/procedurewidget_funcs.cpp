// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/keywordwidgets/widget.hui"
#include "gui/nodecontrolwidget.h"
#include "gui/procedurewidget.h"
#include "main/dissolve.h"
#include <QMessageBox>

ProcedureWidget::ProcedureWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    // Set up the procedure model
    ui_.NodesList->setModel(&procedureModel_);
    connect(ui_.NodesList, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectedNodeChanged(const QModelIndex &)));

    // Hide the available nodes tree by default
    ui_.AvailableNodesTree->setVisible(false);
    ui_.ShowAvailableNodesButton->setVisible(false);
}

// Set up widget
void ProcedureWidget::setUp(DissolveWindow *dissolveWindow, Procedure &proc)
{
    dissolveWindow_ = dissolveWindow;
    procedure_ = proc;
    procedureModel_.setData(proc);
    updateControls();
}

/*
 * Widgets
 */

// Return control widget for the specified node (if it exists)
NodeControlWidget *ProcedureWidget::getControlWidget(ConstNodeRef node, bool setAsCurrent)
{
    for (auto n = 1; n < ui_.NodeControlsStack->count(); ++n)
    {
        auto *w = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(n));
        if (w && (w->node() == node))
        {
            if (setAsCurrent)
                ui_.NodeControlsStack->setCurrentIndex(n);
            return w;
        }
    }
    return nullptr;
}

// Remove control widget for the specified node (if it exists)
void ProcedureWidget::removeControlWidget(ConstNodeRef node)
{
    for (auto n = 1; n < ui_.NodeControlsStack->count(); ++n)
    {
        auto *w = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(n));
        if (w && (w->node() == node))
        {
            if (ui_.NodeControlsStack->currentIndex() == n)
                ui_.NodeControlsStack->setCurrentIndex((n + 1) < ui_.NodeControlsStack->count() ? n + 1 : n - 1);
            ui_.NodeControlsStack->removeWidget(w);
            w->prepareForDeletion();
            return;
        }
    }
}

void ProcedureWidget::selectedNodeChanged(const QModelIndex &index)
{
    // Get the selected node
    auto node = procedureModel_.data(index, Qt::UserRole).value<std::shared_ptr<ProcedureNode>>();
    if (!node)
    {
        ui_.NodeControlsStack->setCurrentIndex(0);
        return;
    }

    // See if our stack already contains a control widget for the node - if not, create one
    auto *ncw = getControlWidget(node, true);
    if (!ncw)
    {
        // Create a new widget to display this node
        ncw = new NodeControlWidget(dissolveWindow_, node);
        connect(ncw, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
        ui_.NodeControlsStack->setCurrentIndex(ui_.NodeControlsStack->addWidget(ncw));

        // If we're currently running, don;t allow editing in our new widget
        if (dissolveWindow_->dissolveIterating())
            ncw->preventEditing();
    }
    else
        ncw->updateControls();
}

void ProcedureWidget::on_ShowAvailableNodesButton_clicked(bool checked)
{
    // Toggle the visibility of the available nodes tree
    ui_.AvailableNodesTree->setVisible(!ui_.AvailableNodesTree->isVisible());

    // Set correct text on our button
    ui_.ShowAvailableNodesButton->setText(ui_.AvailableNodesTree->isVisible() ? "Hide Available Nodes"
                                                                              : "Show Available Nodes");
}

void ProcedureWidget::layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)
{
    //    dissolveWindow_->setModified({DissolveSignals::nodesMutated});
}

void ProcedureWidget::nodeNameChanged(const QModelIndex &index, const QString &oldName, const QString &newName)
{
    auto node = procedureModel_.data(index, Qt::UserRole).value<std::shared_ptr<ProcedureNode>>();
    assert(node);

    // Find the control widget for the node and update it
    auto *ncw = getControlWidget(node);
    if (ncw)
        ncw->updateControls();
}

// Update the node list
void ProcedureWidget::updateNodeList()
{
    // Refresh the node list
    std::optional<QModelIndex> selectedIndex;
    if (!ui_.NodesList->selectionModel()->selection().indexes().empty())
        selectedIndex = ui_.NodesList->selectionModel()->selection().indexes().front();
    procedureModel_.reset();
    if (selectedIndex)
        ui_.NodesList->selectionModel()->select(selectedIndex.value(), QItemSelectionModel::ClearAndSelect);
}

void ProcedureWidget::on_NodesList_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui_.NodesList->indexAt(pos);
    if (!index.isValid())
        return;
    auto node = procedureModel_.data(index, Qt::UserRole).value<std::shared_ptr<ProcedureNode>>();
    assert(node);

    QMenu menu;
    menu.setFont(font());
    menu.setEnabled(!dissolveWindow_->dissolveIterating());
}

void ProcedureWidget::on_AvailableNodesTree_doubleClicked(const QModelIndex &index)
{
    //    nodeLayerModel_.appendNew(nodePaletteModel_.data(index, Qt::DisplayRole).toString());
}

// Remove all node control widgets
void ProcedureWidget::removeControlWidgets()
{
    // Remove all stack pages but the first (which corresponds to the "No node Selected" widget)
    while (ui_.NodeControlsStack->count() > 1)
    {
        auto *w = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(1));
        ui_.NodeControlsStack->removeWidget(w);
        w->prepareForDeletion();
    }
}

/*
 * Update
 */

// Update controls in tab
void ProcedureWidget::updateControls()
{
    if (!procedure_)
        return;

    auto *ncw = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->currentWidget());
    if (ncw)
        ncw->updateControls();
}

// Prevent editing within tab
void ProcedureWidget::preventEditing()
{
    ui_.NodesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.NodesList->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui_.AvailableNodesTree->setEnabled(false);
    for (auto n = 0; n < ui_.NodeControlsStack->count(); ++n)
    {
        auto *ncw = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(n));
        if (ncw)
            ncw->preventEditing();
    }
}

// Allow editing within tab
void ProcedureWidget::allowEditing()
{
    ui_.AvailableNodesTree->setEnabled(true);
    ui_.NodesList->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui_.NodesList->setDragDropMode(QAbstractItemView::DragDrop);
    for (auto n = 0; n < ui_.NodeControlsStack->count(); ++n)
    {
        auto *ncw = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(n));
        if (ncw)
            ncw->allowEditing();
    }
}
