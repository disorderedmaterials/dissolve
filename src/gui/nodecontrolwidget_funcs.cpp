// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/keywordwidgets/producers.h"
#include "gui/nodecontrolwidget.h"

NodeControlWidget::NodeControlWidget(DissolveWindow *dissolveWindow, NodeRef node)
    : dissolve_(dissolveWindow->dissolve()), node_(node)
{
    // Set up user interface
    ui_.setupUi(this);

    assert(node_);

    // Connect signals
    connect(ui_.NodeKeywordsWidget, SIGNAL(keywordChanged(int)), this, SLOT(localKeywordChanged(int)));
    connect(dissolveWindow, SIGNAL(dataMutated(int)), this, SLOT(globalDataMutated(int)));

    // Set the icon label
    ui_.NodeIconLabel->setPixmap(
        QPixmap(QString(":/nodes/icons/nodes_%1.svg")
                    .arg(QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node_->type()))).toLower())));

    // We can only display a single group of widgets at present, so check the size of the index
    auto &&[keywordIndex, keywordMap] = node_->keywords().keywordOrganisation();
    if (keywordIndex.size() > 1)
        Messenger::warn("There are {} keyword groups defined, but only one can be displayed. Tell the developer!\n");

    if (!keywordIndex.empty())
        ui_.NodeKeywordsWidget->setUp(keywordIndex.front(), keywordMap, dissolve_.coreData());

    updateControls();
}

// Return target node for the widget
NodeRef NodeControlWidget::node() { return node_; }

/*
 * Update
 */

// Update controls within widget
void NodeControlWidget::updateControls()
{
    Locker refreshLocker(refreshLock_);

    // Ensure node name and icon status are up to date
    ui_.NodeNameLabel->setText(
        QString("%1 (%2)").arg(QString::fromStdString(std::string(node_->name())),
                               QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node_->type())))));

    // Update keywords
    ui_.NodeKeywordsWidget->updateControls();
}

// Disable editing
void NodeControlWidget::preventEditing() { ui_.NodeKeywordsWidget->setEnabled(false); }

// Allow editing
void NodeControlWidget::allowEditing() { ui_.NodeKeywordsWidget->setEnabled(true); }

/*
 * UI
 */

// Prepare widget for deletion
void NodeControlWidget::prepareForDeletion()
{
    // Nullify the node - this will flag to the update functions that they shouldn't proceed
    node_ = nullptr;

    deleteLater();
}

// Target keyword data changed
void NodeControlWidget::localKeywordChanged(int signalMask)
{
    if (refreshLock_.isLocked())
        return;

    // Always emit the 'dataModified' signal
    emit(dataModified());

    // Act on keyword signals
    Flags<KeywordBase::KeywordSignal> keywordSignals(signalMask);
}

// Global data mutated
void NodeControlWidget::globalDataMutated(int mutationFlags)
{
    // If we have no valid node, don't try to update keyword data
    if (!node_)
        return;

    Flags<DissolveSignals::DataMutations> dataMutations(mutationFlags);
    if (!dataMutations.anySet())
        return;

    // Control keywords
    ui_.NodeKeywordsWidget->updateControls(mutationFlags);
}
