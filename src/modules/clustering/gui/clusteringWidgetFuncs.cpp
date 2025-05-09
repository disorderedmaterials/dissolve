// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "io/export/coordinates.h"
#include "modules/clustering/clustering.h"
#include "modules/clustering/gui/clusteringWidget.h"

ClusteringModuleWidget::ClusteringModuleWidget(QWidget *parent, ClusteringModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    ui_.setupUi(this);
    ui_.ViewerWidget->setConfiguration(clusterConfiguration_);

    // Set up the size distribution plot
    sizeDist_ = ui_.SizePlot->dataViewer();
    auto &viewS = sizeDist_->view();
    viewS.setViewType(View::FlatXYView);
    viewS.axes().setLogarithmic(0, true);
    viewS.axes().setLogarithmic(1, true);
    viewS.axes().setTitle(0, "Cluster Size");
    viewS.axes().setTitle(1, "No. of Clusters");
}

void ClusteringModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (refreshing_)
        return;

    refreshing_ = true;

    if (getNewConfig_ || updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        module_->generateClustersConfig(dissolve_, displaySize_, displayID_);
        clusterConfiguration_ = module_->getClusterConfig();
        ui_.ViewerWidget->setConfiguration(clusterConfiguration_);
        ui_.ViewerWidget->dataModified();
        ui_.ViewerWidget->postRedisplay();
        getNewConfig_ = false;
        // Calculate the coordination numbers
        module_->calculateCN(displaySize_, displayID_);
        buildCNList();
    }

    // Configure the size/mass histograms
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || sizeDist_->renderables().empty())
    {
        sizeDist_->clearRenderables();

        if (sizeDist_->renderables().empty())
            sizeDist_->createRenderable<RenderableData1D>(
                std::format("{}//SizeDist", module_->name()),
                std::format("SizeDist//{}", module_->keywords().getConfiguration("Configuration")->niceName()),
                module_->keywords().getConfiguration("Configuration")->niceName());
    }

    if (!fromBuilder_)
        buildSizeList();

    sizeDist_->validateRenderables(dissolve_.processingModuleData());
    ui_.SizePlot->updateToolbar();
    sizeDist_->postRedisplay();

    fromBuilder_ = false;
    refreshing_ = false;
}

// Produce a list of all the different cluster sizes
void ClusteringModuleWidget::buildSizeList()
{
    auto &sizeDistribution = module_->getSizeDistribution();
    ui_.clusterSizeList->clear();
    ui_.clusterSizeList->addItem("View All");
    for (const auto &[size, _] : sizeDistribution)
    {
        auto *item = new QListWidgetItem(QString::number(size));
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui_.clusterSizeList->addItem(item);
    }
}

// Produce a list of all the cluster IDs of the selected cluster size
void ClusteringModuleWidget::buildIDList(QListWidgetItem *item)
{
    auto sizeDistribution = module_->getSizeDistribution();
    ui_.clusterIDList->clear();
    if (item->text() == "View All")
    {
        displaySize_ = 0;
        getNewConfig_ = true;
        updateControls();
        return;
    }

    ui_.clusterIDList->addItem("View All");
    displaySize_ = item->text().toInt();
    for (const auto &ID : sizeDistribution[displaySize_])
    {
        auto *cell = new QListWidgetItem(QString::number(ID));
        cell->setFlags(cell->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui_.clusterIDList->addItem(cell);
    }
}

// Display all coordination numbers for the currrent view
void ClusteringModuleWidget::buildCNList()
{
    auto cns = module_->getCNs();
    ui_.cnList->clear();
    for (const auto &[site, map] : cns)
        for (const auto &[otherSite, cn] : map)
            ui_.cnList->addItem(QString::fromStdString(std::format("{} - {}: {:.3f}", site->name(), otherSite->name(), cn)));
}

void ClusteringModuleWidget::on_clusterSizeList_itemClicked(QListWidgetItem *item) { buildIDList(item); }

void ClusteringModuleWidget::on_clusterIDList_itemClicked(QListWidgetItem *item)
{
    item->text() == "View All" ? displayID_ = 0 : displayID_ = item->text().toInt();
    getNewConfig_ = true;
    fromBuilder_ = true;
    updateControls();
}

void ClusteringModuleWidget::on_exportButton_clicked()
{
    CoordinateExportFileFormat fileAndFormat(
        std::format("clusterSelectionSize{}ID{}Iteration{}.xyz", displaySize_, displayID_, dissolve_.iteration()),
        CoordinateExportFileFormat::CoordinateExportFormat::XYZ);
    if (!fileAndFormat.exportData(clusterConfiguration_))
        Messenger::error("Failed to export current visualisation");
    else
        QMessageBox::information(this, "Export Successful",
                                 QString::fromStdString(std::format("Successfully exported current visualisation to '{}'.",
                                                                    fileAndFormat.filename())),
                                 QMessageBox::Ok);
}