// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/nodeControlWidget.h"
#include "gui/gui.h"
#include "gui/keywordWidgets/producers.h"

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
    ui_.NodeIconLabel->setPixmap(QPixmap(
        QString(":/nodes/icons/nodes/%1.svg").arg(QString::fromStdString(GeneratorNode::nodeTypes().keyword(node->type())))));

    // Set up any keyword widgets we have
    if (!node_->keywords().sections().empty())
    {
        // We can only display a single section of widgets at present
        if (node_->keywords().sections().size() > 1)
            Messenger::warn("There are {} keyword sections defined, but only one can be displayed. Tell the developer!\n");

        ui_.NodeKeywordsWidget->setUp(node_->keywords().sections().front(), dissolve_.coreData());
    }

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
                               QString::fromStdString(GeneratorNode::nodeTypes().keyword(node_->type()))));

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
    Q_EMIT(dataModified());

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
