// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "templates/variantPointer.h"
#include <QComboBox>

#include <memory>

#pragma once

// ComboBoxUpdater - Constructor-only template class to update items in a QComboBox, preserving original items as much as
// possible
template <class I> class ComboBoxUpdater
{
    public:
    // Update QComboBox from supplied vector assuming that class I implements a name() function for the item
    ComboBoxUpdater(QComboBox *comboBox, const std::vector<std::unique_ptr<I>> &data, const I *currentItem, int startIndex = 0,
                    int indexIfNoCurrentItem = -1)
    {
        comboBox_ = comboBox;
        currentIndex_ = startIndex;

        for (const auto &dataItem : data)
            updateItem(dataItem->name(), dataItem.get(), dataItem.get() == currentItem);

        // If there are still rows remaining in the widget, delete them now
        while (currentIndex_ < comboBox_->count())
            comboBox_->removeItem(currentIndex_);

        // If there is no valid current item, make sure this is reflected in the combobox
        if (currentItem == nullptr)
            comboBox->setCurrentIndex(indexIfNoCurrentItem);
    }

    // Update QComboBox from supplied RefDataList, using the QString data as the item's name
    ComboBoxUpdater(QComboBox *comboBox, const std::vector<std::pair<I *, std::string>> &data, const I *currentItem,
                    int startIndex = 0, int indexIfNoCurrentItem = -1)
    {
        comboBox_ = comboBox;
        currentIndex_ = startIndex;

        for (auto dataItem : data)
            updateItem(dataItem.second, dataItem.first, dataItem.first == currentItem);

        // If there are still rows remaining in the widget, delete them now
        while (currentIndex_ < comboBox_->count())
            comboBox_->removeItem(currentIndex_);

        // If there is no valid current item, make sure this is reflected in the combobox
        if (currentItem == nullptr)
            comboBox->setCurrentIndex(indexIfNoCurrentItem);
    }

    private:
    // Target combo box
    QComboBox *comboBox_;
    // Item index
    int currentIndex_;

    private:
    // Update item at specified position in combo box (if it exists)
    void updateItem(std::string_view itemText, I *itemData, bool isCurrent)
    {
        /*
         * If there is an entry already at this index, check it:
         * - If it represents the current pointer data, just update it and move on.
         * - Otherwise, delete it and check the next one.
         *
         * If no entry exists, create a new one.
         */
        while (currentIndex_ < comboBox_->count())
        {
            I *oldData = VariantPointer<I>(comboBox_->itemData(currentIndex_, Qt::UserRole));
            if (oldData == itemData)
            {
                // Data pointer matches - make sure the text is up to date
                comboBox_->setItemText(currentIndex_, QString::fromStdString(std::string(itemText)));

                break;
            }
            else
            {
                // Data pointer does not match - remove the offending item
                comboBox_->removeItem(currentIndex_);
            }
        }

        // If the current row index is (now) out of range, add a new row to the list
        if (currentIndex_ == comboBox_->count())
        {
            // Create new item
            comboBox_->addItem(QString::fromStdString(std::string(itemText)), VariantPointer<I>(itemData));
        }

        // Select this item if it is the current one
        if (isCurrent)
            comboBox_->setCurrentIndex(currentIndex_);

        // Increase index
        ++currentIndex_;
    }
};

// ComboBoxUpdater - Constructor-only template class to update items in a QComboBox, preserving original items as much as
// possible
template <class T, class I> class ComboBoxTextUpdater
{
    // Typedefs for passed functions
    typedef QString (T::*ComboBoxItemTextFunction)(const I *item);
};
