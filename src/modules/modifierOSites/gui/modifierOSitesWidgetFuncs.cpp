// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
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

    auto &view = oSitesGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Qn");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Normalised Frequency");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    auto &view2 = modifierSitesGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Qn");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Normalised Frequency");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void ModifierOSitesModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        oSitesGraph_->clearRenderables();
        modifierSitesGraph_->clearRenderables();
    }

    if (oSitesGraph_->renderables().empty())
        oSitesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//OSites", module_->name()), "O-Sites");

    if (modifierSitesGraph_->renderables().empty())
        modifierSitesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//ModifierSites", module_->name()),
                                                                "Modifier-Sites");

    // Validate renderables if they need it
    oSitesGraph_->validateRenderables(dissolve_.processingModuleData());
    modifierSitesGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.OSitesPlotWidget->updateToolbar();
    ui_.ModifierPlotWidget->updateToolbar();

    oSitesGraph_->postRedisplay();
    modifierSitesGraph_->postRedisplay();
}
