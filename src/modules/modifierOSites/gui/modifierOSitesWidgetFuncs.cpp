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
    mFOBondLengthGraph_ = ui_.MFOLengthPlotWidget->dataViewer();
    mNBOBondLengthGraph_ = ui_.MNBOLengthPlotWidget->dataViewer();
    mBOBondLengthGraph_ = ui_.MBOLengthPlotWidget->dataViewer();
    mOtherOBondLengthGraph_ = ui_.MOtherOLengthPlotWidget->dataViewer();

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

    auto &mFOLengthView = mFOBondLengthGraph_->view();
    mFOLengthView.setViewType(View::FlatXYView);
    mFOLengthView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    mFOLengthView.axes().setMax(0, 10.0);
    mFOLengthView.axes().setTitle(1, "Normalised Frequency");
    mFOLengthView.axes().setMin(1, 0.0);
    mFOLengthView.axes().setMax(1, 1.0);
    mFOLengthView.setAutoFollowType(View::AllAutoFollow);

    auto &mNBOLengthView = mNBOBondLengthGraph_->view();
    mNBOLengthView.setViewType(View::FlatXYView);
    mNBOLengthView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    mNBOLengthView.axes().setMax(0, 10.0);
    mNBOLengthView.axes().setTitle(1, "Normalised Frequency");
    mNBOLengthView.axes().setMin(1, 0.0);
    mNBOLengthView.axes().setMax(1, 1.0);
    mNBOLengthView.setAutoFollowType(View::AllAutoFollow);

    auto &mBOLengthView = mBOBondLengthGraph_->view();
    mBOLengthView.setViewType(View::FlatXYView);
    mBOLengthView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    mBOLengthView.axes().setMax(0, 10.0);
    mBOLengthView.axes().setTitle(1, "Normalised Frequency");
    mBOLengthView.axes().setMin(1, 0.0);
    mBOLengthView.axes().setMax(1, 1.0);
    mBOLengthView.setAutoFollowType(View::AllAutoFollow);

    auto &mOtherOLengthView = mOtherOBondLengthGraph_->view();
    mOtherOLengthView.setViewType(View::FlatXYView);
    mOtherOLengthView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    mOtherOLengthView.axes().setMax(0, 10.0);
    mOtherOLengthView.axes().setTitle(1, "Normalised Frequency");
    mOtherOLengthView.axes().setMin(1, 0.0);
    mOtherOLengthView.axes().setMax(1, 1.0);
    mOtherOLengthView.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void ModifierOSitesModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        oSitesGraph_->clearRenderables();
        modifierSitesGraph_->clearRenderables();
        mFOBondLengthGraph_->clearRenderables();
        mNBOBondLengthGraph_->clearRenderables();
        mBOBondLengthGraph_->clearRenderables();
        mOtherOBondLengthGraph_->clearRenderables();
    }

    if (oSitesGraph_->renderables().empty())
        oSitesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//OTypes", module_->name()), "O-Types");

    if (modifierSitesGraph_->renderables().empty())
        modifierSitesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//TotalOSites", module_->name()),
                                                                "TotalO-Sites");
    if (mFOBondLengthGraph_->renderables().empty())
        mFOBondLengthGraph_->createRenderable<RenderableData1D>(fmt::format("{}//MFOBondLength", module_->name()),
                                                                "MFO-Bond-Length");
    if (mNBOBondLengthGraph_->renderables().empty())
        mNBOBondLengthGraph_->createRenderable<RenderableData1D>(fmt::format("{}//MNBOBondLength", module_->name()),
                                                                 "MNBO-Bond-Length");
    if (mBOBondLengthGraph_->renderables().empty())
        mBOBondLengthGraph_->createRenderable<RenderableData1D>(fmt::format("{}//MBOBondLength", module_->name()),
                                                                "MBO-Bond-Length");
    if (mOtherOBondLengthGraph_->renderables().empty())
        mOtherOBondLengthGraph_->createRenderable<RenderableData1D>(fmt::format("{}//MOtherOBondLength", module_->name()),
                                                                    "MOtherO-Bond-Length");

    // Validate renderables if they need it
    oSitesGraph_->validateRenderables(dissolve_.processingModuleData());
    modifierSitesGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.OSitesPlotWidget->updateToolbar();
    ui_.ModifierPlotWidget->updateToolbar();

    oSitesGraph_->postRedisplay();
    modifierSitesGraph_->postRedisplay();
}
