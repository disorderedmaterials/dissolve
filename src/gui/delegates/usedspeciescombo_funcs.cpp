/*
    *** UsedSpecies Combo Delegate Functions
    *** src/gui/delegates/usedspeciescombo_funcs.cpp
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

#include "classes/configuration.h"
#include "classes/species.h"
#include "gui/delegates/usedspeciescombo.hui"
#include "gui/helpers/combopopulator.h"
#include "templates/variantpointer.h"

UsedSpeciesComboDelegate::UsedSpeciesComboDelegate(QObject *parent) : QItemDelegate(parent) {}

UsedSpeciesComboDelegate::~UsedSpeciesComboDelegate() {}

// Create editor
QWidget *UsedSpeciesComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    // Create editor widget (in this case a combo box) and add the available options
    auto *editor = new QComboBox(parent);

    // Get the model UserData for the current index - it should be a Configuration
    Configuration *cfg = VariantPointer<Configuration>(index.data(Qt::UserRole));
    if (cfg)
    {
        for (auto *spInfo = cfg->usedSpecies().first(); spInfo != nullptr; spInfo = spInfo->next())
        {
            editor->addItem(QString::fromStdString(std::string(spInfo->species()->name())),
                            VariantPointer<Species>(spInfo->species()));
        }
    }
    else
        Messenger::error("Underlying model did not contain a Configuration*, so UsedSpeciesCombo cannot provide options.\n");

    return editor;
}

// Set initial value in editor
void UsedSpeciesComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Get the current text and search for it in the combo
    QString value = index.model()->data(index, Qt::EditRole).toString();

    for (int n = 0; n < comboBox->count(); ++n)
    {
        if (comboBox->itemText(n) == value)
        {
            comboBox->setCurrentIndex(n);
            break;
        }
    }
}

// Get value from editing widget, and set back in model
void UsedSpeciesComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Set the current text in the model
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
void UsedSpeciesComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                    const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
