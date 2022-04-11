// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/widget.hui"
#include "gui/models/procedureModel.h"
#include <QModelIndex>
#include <QStackedWidget>
#include <map>

class Dissolve;
class DissolveWindow;

class ProcedureControlStack : public QStackedWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ProcedureControlStack(QWidget *parent = nullptr);
    ~ProcedureControlStack() override = default;
    void updateProcedureWidget(Dissolve &dissolve, DissolveWindow *dissolveWindow, ProcedureModel &procedureModel,
			       const QModelIndex &index);

    private:
    // Widget Map
    std::map<const QModelIndex, KeywordsWidget *> widgetMap_;
};
