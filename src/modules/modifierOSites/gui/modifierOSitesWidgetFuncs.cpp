// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "math/integerHistogram1D.h"
#include "modules/modifierOSites/gui/modifierOSitesWidget.h"
#include "modules/modifierOSites/modifierOSites.h"

ModifierOSitesModuleWidget::ModifierOSitesModuleWidget(QWidget *parent, ModifierOSitesModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up Oxygen Sites Graph
    oSitesGraph_ = ui_.OSitesPlotWidget->dataViewer();
    modifierSitesGraph_ = ui_.ModifierPlotWidget->dataViewer();
    mOBondLengthGraph_ = ui_.MOLengthPlotWidget->dataViewer();

    auto &oSitesView = oSitesGraph_->view();
    oSitesView.setViewType(View::FlatXYView);
    oSitesView.axes().setTitle(0, "NF bound to O");
    oSitesView.axes().setMax(0, 10.0);
    oSitesView.axes().setTitle(1, "Normalised Frequency");
    oSitesView.axes().setMin(1, 0.0);
    oSitesView.axes().setMax(1, 1.0);
    oSitesView.setAutoFollowType(View::AllAutoFollow);

    auto &modifierSitesView = modifierSitesGraph_->view();
    modifierSitesView.setViewType(View::FlatXYView);
    modifierSitesView.axes().setTitle(0, "Total O bound to SiteA");
    modifierSitesView.axes().setMax(0, 10.0);
    modifierSitesView.axes().setTitle(1, "Normalised Frequency");
    modifierSitesView.axes().setMin(1, 0.0);
    modifierSitesView.axes().setMax(1, 1.0);
    modifierSitesView.setAutoFollowType(View::AllAutoFollow);

    auto &mOLengthView = mOBondLengthGraph_->view();
    mOLengthView.setViewType(View::FlatXYView);
    mOLengthView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    mOLengthView.axes().setMax(0, 10.0);
    mOLengthView.axes().setTitle(1, "Normalised Frequency");
    mOLengthView.axes().setMin(1, 0.0);
    mOLengthView.axes().setMax(1, 1.0);
    mOLengthView.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void ModifierOSitesModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        oSitesGraph_->clearRenderables();
        modifierSitesGraph_->clearRenderables();
        mOBondLengthGraph_->clearRenderables();
    }

    if (oSitesGraph_->renderables().empty())
        oSitesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//OTypes", module_->name()), "O-Types");

    if (modifierSitesGraph_->renderables().empty())
        modifierSitesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//TotalOSites", module_->name()),
                                                                "TotalO-Sites");
    if (mOBondLengthGraph_->renderables().empty())
    {
        mOBondLengthGraph_
            ->createRenderable<RenderableData1D>(fmt::format("{}//MFOBondLength", module_->name()), "MFO-Bond-Length")
            ->setColour(StockColours::GreenStockColour);
        mOBondLengthGraph_
            ->createRenderable<RenderableData1D>(fmt::format("{}//MNBOBondLength", module_->name()), "MNBO-Bond-Length")
            ->setColour(StockColours::RedStockColour);
        ;
        mOBondLengthGraph_
            ->createRenderable<RenderableData1D>(fmt::format("{}//MBOBondLength", module_->name()), "MBO-Bond-Length")
            ->setColour(StockColours::BlueStockColour);
        mOBondLengthGraph_->createRenderable<RenderableData1D>(fmt::format("{}//MOtherOBondLength", module_->name()),
                                                               "MOtherO-Bond-Length");
    }

    // Validate renderables if they need it
    oSitesGraph_->validateRenderables(dissolve_.processingModuleData());
    modifierSitesGraph_->validateRenderables(dissolve_.processingModuleData());
    mOBondLengthGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.OSitesPlotWidget->updateToolbar();
    ui_.ModifierPlotWidget->updateToolbar();
    ui_.MOLengthPlotWidget->updateToolbar();

    oSitesGraph_->postRedisplay();
    modifierSitesGraph_->postRedisplay();
    mOBondLengthGraph_->postRedisplay();
}
