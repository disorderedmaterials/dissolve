// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/generatorWidget.h"
#include "gui/gui.h"
#include "gui/keywordWidgets/widget.h"
#include "gui/nodeControlWidget.h"
#include "main/dissolve.h"
#include <QMessageBox>

GeneratorWidget::GeneratorWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    // Set up the generator model
    ui_.NodesTree->setModel(&generatorModel_);
    connect(ui_.NodesTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(selectedNodeChanged(const QItemSelection &)));
    connect(&generatorModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(generatorDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)));
    // Set up the available nodes tree
    ui_.AvailableNodesTree->setModel(&nodePaletteModel_);
    ui_.AvailableNodesTree->expandAll();
    ui_.AvailableNodesTree->setVisible(false);
}

// Set up widget
void GeneratorWidget::setUp(DissolveWindow *dissolveWindow, Generator &proc)
{
    dissolveWindow_ = dissolveWindow;
    generator_ = proc;
    ui_.AvailableNodesTree->expandAll();
    generatorModel_.setData(proc);
    ui_.NodesTree->expandAll();
    ui_.NodesTree->resizeColumnToContents(0);
    ui_.NodesTree->resizeColumnToContents(1);
    ui_.NodesTree->hideColumn(1);
    updateControls();
}

/*
 * Widgets
 */

// Return control widget for the specified node (if it exists)
NodeControlWidget *GeneratorWidget::getControlWidget(ConstNodeRef node, bool setAsCurrent)
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
void GeneratorWidget::removeControlWidget(ConstNodeRef node)
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

void GeneratorWidget::selectedNodeChanged(const QItemSelection &selected)
{
    // In the case a row has been removed, the selection is out-of-date
    // For sanity, get the selection again.
    auto actualSelected = ui_.NodesTree->selectionModel()->selection();

    // Check if a node is actually selected
    if (actualSelected.indexes().empty())
    {
        ui_.DeleteNodeButton->setEnabled(false);
        ui_.NodeControlsStack->setCurrentIndex(0);
        return;
    }

    auto selectedIndex = actualSelected.indexes().front();

    // Get the selected node
    auto node = generatorModel_.data(selectedIndex, Qt::UserRole).value<std::shared_ptr<GeneratorNode>>();
    if (!node)
    {
        ui_.NodeControlsStack->setCurrentIndex(0);
        return;
    }

    ui_.DeleteNodeButton->setEnabled(true);

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

void GeneratorWidget::on_ExpandAllButton_clicked(bool checked) { ui_.NodesTree->expandAll(); }

void GeneratorWidget::on_CollapseAllButton_clicked(bool checked) { ui_.NodesTree->collapseAll(); }

void GeneratorWidget::on_ShowContextButton_clicked(bool checked)
{
    if (checked)
        ui_.NodesTree->showColumn(1);
    else
        ui_.NodesTree->hideColumn(1);
}

void GeneratorWidget::on_ShowAvailableNodesButton_clicked(bool checked)
{
    // Toggle the visibility of the available nodes tree
    ui_.AvailableNodesTree->setVisible(!ui_.AvailableNodesTree->isVisible());

    // Set correct text on our button
    ui_.ShowAvailableNodesButton->setText(ui_.AvailableNodesTree->isVisible() ? "Hide Available Nodes"
                                                                              : "Show Available Nodes");
}

void GeneratorWidget::generatorDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)
{
    if (!generator_)
        return;

    // Node order may have changed, or node may have been deleted, so validate related data
    generator_.value().get().rootSequence().validateNodeKeywords();

    updateControls();
    dissolveWindow_->setModified();
}

void GeneratorWidget::nodeNameChanged(const QModelIndex &index, const QString &oldName, const QString &newName)
{
    auto node = generatorModel_.data(index, Qt::UserRole).value<std::shared_ptr<GeneratorNode>>();
    assert(node);

    // Find the control widget for the node and update it
    auto *ncw = getControlWidget(node);
    if (ncw)
        ncw->updateControls();
}

// Update the node tree
void GeneratorWidget::updateNodeTree()
{
    // Refresh the node list
    std::optional<QModelIndex> selectedIndex;
    if (!ui_.NodesTree->selectionModel()->selection().indexes().empty())
        selectedIndex = ui_.NodesTree->selectionModel()->selection().indexes().front();
    generatorModel_.reset();
    if (selectedIndex)
        ui_.NodesTree->selectionModel()->select(selectedIndex.value(), QItemSelectionModel::ClearAndSelect);
}

void GeneratorWidget::on_NodesTree_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui_.NodesTree->indexAt(pos);
    if (!index.isValid())
        return;
    auto node = generatorModel_.data(index, Qt::UserRole).value<std::shared_ptr<GeneratorNode>>();
    assert(node);

    QMenu menu;
    menu.setFont(font());
    menu.setEnabled(!dissolveWindow_->dissolveIterating());
}

void GeneratorWidget::on_AvailableNodesTree_doubleClicked(const QModelIndex &index)
{
    //    nodeLayerModel_.appendNew(nodePaletteModel_.data(index, Qt::DisplayRole).toString());
}

// Delete the currently selected node, and its children
void GeneratorWidget::on_DeleteNodeButton_clicked(bool checked)
{
    auto selectedIndex = ui_.NodesTree->selectionModel()->selectedIndexes().front();

    if (selectedIndex.isValid())
    {
        generatorModel_.removeRow(selectedIndex.row(), generatorModel_.parent(selectedIndex));
        dissolveWindow_->setModified();
    }
}

// Remove all node control widgets
void GeneratorWidget::removeControlWidgets()
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
void GeneratorWidget::updateControls()
{
    if (!generator_)
        return;

    auto *ncw = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->currentWidget());
    if (ncw)
        ncw->updateControls();
}

// Prevent editing within tab
void GeneratorWidget::preventEditing()
{
    ui_.NodesTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.NodesTree->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui_.AvailableNodesTree->setEnabled(false);
    for (auto n = 0; n < ui_.NodeControlsStack->count(); ++n)
    {
        auto *ncw = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(n));
        if (ncw)
            ncw->preventEditing();
    }
}

// Allow editing within tab
void GeneratorWidget::allowEditing()
{
    ui_.AvailableNodesTree->setEnabled(true);
    ui_.NodesTree->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui_.NodesTree->setDragDropMode(QAbstractItemView::DragDrop);
    for (auto n = 0; n < ui_.NodeControlsStack->count(); ++n)
    {
        auto *ncw = dynamic_cast<NodeControlWidget *>(ui_.NodeControlsStack->widget(n));
        if (ncw)
            ncw->allowEditing();
    }
}
