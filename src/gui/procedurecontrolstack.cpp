// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/procedurecontrolstack.h"
#include "gui/gui.h"

ProcedureControlStack::ProcedureControlStack(QWidget *parent) : QStackedWidget(parent) {}

void ProcedureControlStack::updateProcedureWidget(Dissolve &dissolve, DissolveWindow *dissolveWindow,
                                                  ProcedureModel &procedureModel, const QModelIndex &index)
{
    auto it = widgetMap_.find(index);
    if (it == widgetMap_.end())
    {
        QVariant var = procedureModel.data(index, Qt::UserRole);
        auto data = var.value<std::shared_ptr<ProcedureNode>>();

        // Don't do anything if there is no widget
        if (data == nullptr)
            return;

        KeywordsWidget *widget = new KeywordsWidget(this);
        connect(widget, SIGNAL(keywordChanged(int)), dissolveWindow, SLOT(setModified()));
        widget->setUp(data->keywords(), dissolve.coreData());
        addWidget(widget);
        widgetMap_[index] = widget;
    }

    setCurrentWidget(widgetMap_[index]);
}
