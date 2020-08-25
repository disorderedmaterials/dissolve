/*
    *** Keyword Widget - Module
    *** src/gui/keywordwidgets/module.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_module.h"
#include "keywords/module.h"
#include <QWidget>

// Forward Declarations
/* none */

class ModuleKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ModuleKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ModuleKeywordBase *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ModuleWidget ui_;

    private slots:
    // Value changed
    void on_ModuleCombo_currentIndexChanged(int index);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    private:
    // Return text (for ComboBox item) for supplied Module
    QString uniqueNameOfModule(const Module *module);

    public:
    // Update value displayed in widget
    void updateValue();
};
