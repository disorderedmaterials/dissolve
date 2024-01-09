// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include <QComboBox>
#include <functional>

#pragma once

/*
 * QComboBox Updater
 *
 * Manage the items displayed in a QComboBox from a reference container, preserving the widget as much as possible on item
 * addition or deletion. The lambda function should return the text to display for the item it is passed. Pointers to the items
 * are stored as data in the QComboBox to allow tests of equivalence.
 */

template <class Data> void combo_box_update_item(QComboBox *comboBox, Data item, QString itemText, int index)
{
    while (index < comboBox->count())
    {
        // If the data pointer matches, update the text and break. Otherwise, delete the item
        auto itemData = comboBox->itemData(index, Qt::UserRole).value<Data>();
        if (itemData == item)
        {
            comboBox->setItemText(index, itemText);
            break;
        }
        else
            comboBox->removeItem(index);
    }

    // If the current row index is (now) out of range, add a new row to the list
    if (index == comboBox->count())
        comboBox->addItem(itemText, QVariant::fromValue(item));
}

template <class Iter, class Lam>
Iter combo_box_updater(QComboBox *comboBox, Iter begin, Iter end, Lam getItemText, bool allowNone = false)
{
    using itemType = typename std::iterator_traits<Iter>::value_type;

    auto index = 0;
    Iter currentItem = end;

    // Add [None] option?
    if (allowNone)
        combo_box_update_item<itemType>(comboBox, nullptr, QString("[None]"), index++);

    for (auto item = begin; item != end; ++item)
    {
        /*
         * If there is an entry already at this index, check it:
         * - If it represents the current pointer data, just update it and move on.
         * - Otherwise, delete it and check the next one.
         *
         * If no entry exists, create a new one.
         */

        // Get the text for this item (as a QString for convenience)
        auto itemText = QString::fromStdString(std::string());

        // Update the item
        combo_box_update_item(comboBox, *item, QString::fromStdString(std::string(getItemText(*item))), index);

        // Check for current item
        if (comboBox->currentIndex() == index)
            currentItem = item;

        // Increase index
        ++index;
    }

    // If there are still rows remaining in the widget, delete them now
    while (index < comboBox->count())
        comboBox->removeItem(index);

    // Return the current item
    if (comboBox->currentIndex() == -1)
        return {};
    else
        return currentItem;
}

template <class Data> int combo_box_set_current(QComboBox *comboBox, Data item)
{
    for (auto index = 0; index < comboBox->count(); ++index)
    {
        // If the data pointer matches, update the text and break. Otherwise, delete the item
        auto itemData = comboBox->itemData(index, Qt::UserRole).value<Data>();
        if (itemData == item)
        {
            comboBox->setCurrentIndex(index);
            return index;
        }
    }

    throw(std::runtime_error("Tried to set a ComboBox to an item that it doesn't contain.\n"));
}
