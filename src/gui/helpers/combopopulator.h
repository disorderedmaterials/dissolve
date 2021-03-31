// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptionsbase.h"
#include "templates/list.h"
#include "templates/variantpointer.h"
#include <QComboBox>

// ComboBox Populator from EnumOptions
class ComboEnumOptionsPopulator
{
    public:
    ComboEnumOptionsPopulator(QComboBox *combo, const EnumOptionsBase &options, bool append = false)
    {
        // Clear the combobox
        if (!append)
            combo->clear();

        // Add our text items to the list
        for (auto n = 0; n < options.nOptions(); ++n)
            combo->addItem(QString::fromStdString(std::string(options.keywordByIndex(n))));
    }
};

// ComboBox Populator, from templated List using item's name() function
template <class T> class ComboNameListPopulator
{
    public:
    ComboNameListPopulator<T>(QComboBox *combo, const List<T> &items, bool append = false)
    {
        // Clear the combobox
        if (!append)
            combo->clear();

        // Add our text items to the list
        for (T *item = items.first(); item != nullptr; item = item->next())
            combo->addItem(item->name(), VariantPointer<T>(item));
    }
    ComboNameListPopulator<T>(QComboBox *combo, const std::vector<T> &items, QString prefix, bool append = false)
    {
        // Clear the combobox
        if (!append)
            combo->clear();

        // Add our text items to the list
        for (auto &item : items)
            combo->addItem(QString("%1%2").arg(prefix, QString::fromStdString(std::string(item.name()))),
                           VariantPointer<T>(&item));
    }
    ComboNameListPopulator<T>(QComboBox *combo, const std::list<T> &items, QString prefix, bool append = false)
    {
        // Clear the combobox
        if (!append)
            combo->clear();

        // Add our text items to the list
        for (auto &item : items)
            combo->addItem(QString("%1%2").arg(prefix, QString::fromStdString(std::string(item.name()))),
                           VariantPointer<T>(&item));
    }
};
