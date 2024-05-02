// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include <QComboBox>
#include <QModelIndex>
#include <QObject>
#include <QtWidgets/QItemDelegate>
#include <memory>
#include <vector>

// Base class for source List
class ComboListItems
{
    public:
    ComboListItems();
    virtual ~ComboListItems();

    /*
     * Access
     */
    public:
    // Return number of items in list
    virtual int nItems() = 0;
    // Restart list iteration
    virtual void restartIterator() = 0;
    // Move to next item in list
    virtual bool nextItem() = 0;
    // Return text entry for nth item
    virtual QString currentItemText() = 0;
};

// Templated Item Source for ComboList (utilising T's name())
template <class T> class ComboSharedNameListItems : public ComboListItems
{
    public:
    ComboSharedNameListItems(const std::vector<std::shared_ptr<T>> &items) : items_(items), currentIndex_(-1) {}

    private:
    // Associated item list
    const std::vector<std::shared_ptr<T>> &items_;
    // Current iteration item index
    int currentIndex_;

    /*
     * Access
     */
    public:
    // Return number of items in list
    int nItems() { return items_.size(); }
    // Restart list iteration
    void restartIterator() { currentIndex_ = -1; }
    // Move to next item in list
    bool nextItem()
    {
        ++currentIndex_;
        return currentIndex_ < items_.size();
    }
    // Return text entry for nth item
    QString currentItemText()
    {
        return ((currentIndex_ != -1) && (currentIndex_ < items_.size())
                    ? QString::fromStdString(std::string((items_[currentIndex_])->name()))
                    : "NULL");
    }
};

// Templated EnumOptions Source for ComboList
template <class E> class ComboEnumOptionsItems : public ComboListItems
{
    public:
    ComboEnumOptionsItems(EnumOptions<E> enumOptions) : items_(enumOptions) { currentItemIndex_ = 0; }

    private:
    // Associated item list
    const EnumOptions<E> items_;
    // Current iterator index
    int currentItemIndex_;

    /*
     * Access
     */
    public:
    // Return number of items in list
    int nItems() { return items_.nOptions(); }
    // Restart list iteration
    void restartIterator() { currentItemIndex_ = -1; }
    // Move to next item in list
    bool nextItem()
    {
        ++currentItemIndex_;
        return (currentItemIndex_ < nItems());
    }
    // Return text entry for nth item
    QString currentItemText()
    {
        return (currentItemIndex_ < nItems() ? QString::fromStdString(std::string(items_.keywordByIndex(currentItemIndex_)))
                                             : "NULL");
    }
};

// Delegate class, providing list of name()s in a QComboBox
class ComboListDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
    ComboListDelegate(QObject *parent, ComboListItems *items, bool allowNewItems = false);
    virtual ~ComboListDelegate();

    /*
     * Data
     */
    private:
    // Associated item list
    ComboListItems *items_;
    // Parameters for editor
    bool allowNewItems_;

    /*
     * QItemDelegate Virtuals
     */
    private:
    // Create editing widget
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // Set the data to appear when the editor is called
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // Extract data from editor, and place back into source model
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    // Resize editor
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Index changed in combo box
    void comboIndexChanged(int index);
};
