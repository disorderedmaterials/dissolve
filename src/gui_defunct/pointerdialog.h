/*
	*** Pointer Dialog Class
	*** src/gui/pointerdialog.h
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_POINTERDIALOG_H
#define DUQ_POINTERDIALOG_H

#include "base/dnchar.h"
#include <QtGui/QDialog>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>

/*
 * \brief Qt Pointer Dialog Class
 * \details This class can be used to easily set-up a list of pointers and associated display strings, display them, and return
 * the pointer of the selected item (unless cancelled).
*/
template <class A> class PointerDialog : public QObject
{
// 	Q_OBJECT

	public:
	// Constructor
	PointerDialog<A>(const char* dialogTitle);
	// Destructor
	~PointerDialog();


	/*
	 * Data
	 */
	///@{
	private:
	// QDialog object
	QDialog dialog_;
	// Dialog title
	Dnchar title_;
	// RefList of Pointers and their associated display text
	RefList<A,Dnchar> items_;
	
	public slots:
	void listWidgetDoubleClicked(QListWidgetItem *item);

	public:
	// Add item to list
	void addItem(A* pointer, const char* text); 
	// Return number of items in the list
	int nItems();


	/*
	 * Execution
	 */
	public:
	// Show dialog
	A* execute(A* defaultItem = NULL);
};

/*
 * \brief Constructor
 * \details Constructor for PointerDialog. 
 */
template <class A> PointerDialog<A>::PointerDialog(const char* title)
{
	title_ = title;
}

/*
 * \brief Destructor
 * \details Destructor for PointerDialog. 
 */
template <class A> PointerDialog<A>::~PointerDialog()
{
}

/*
 * \brief Add item to list
 */
template <class A> void PointerDialog<A>::addItem(A* pointer, const char* text)
{
	items_.add(pointer, text);
}

/*
 * \brief Return number of items in the list
 */
template <class A> int PointerDialog<A>::nItems()
{
	return items_.nItems();
}

/*
 * \brief List Widget double-clicked
 */
template <class A> void PointerDialog<A>::listWidgetDoubleClicked(QListWidgetItem* item)
{
	if (item == NULL) return;
	dialog_.accept();
}

/*
 * \brief Show dialog
 */
template <class A> A* PointerDialog<A>::execute(A* defaultItem)
{
	// Create and execute dialog
	dialog_.setWindowTitle(title_.get());
	dialog_.setModal(true);

	// Add and populate item list
	QListWidget* list = new QListWidget(&dialog_);
	list->setSelectionMode(QAbstractItemView::SingleSelection);
	for (RefListItem<A,Dnchar>* ri = items_.first(); ri != NULL; ri = ri->next)
	{
		list->addItem(ri->data.get());
		if (defaultItem && (defaultItem == ri->item)) list->setCurrentRow(list->count()-1);
	}
	QObject::connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listWidgetDoubleClicked(QListWidgetItem*)));

	// Add buttons
	QPushButton* okButton = new QPushButton("&OK");
	okButton->setDefault(true);
	QObject::connect(okButton, SIGNAL(clicked(bool)), &dialog_, SLOT(accept()));
	QPushButton* cancelButton = new QPushButton("&Cancel");
	QObject::connect(cancelButton, SIGNAL(clicked(bool)), &dialog_, SLOT(reject()));
	
	// Create buttonbox
	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
	buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);
	
	// Create layout, and add widgets
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(list);
	layout->addWidget(buttonBox);
	dialog_.setLayout(layout);
	
	// Execute dialog and check result
	if (dialog_.exec())
	{
		// Get current index from QListWidget
		int currentIndex = list->currentRow();
		if (currentIndex == -1) return NULL;
		else return items_[currentIndex]->item;
	}
	else return NULL;
}

#endif
