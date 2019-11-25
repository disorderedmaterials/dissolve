/*
	*** Keywords Dialog
	*** src/gui/keywordwidgets/dialog.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_DIALOG_KEYWORDS_H
#define DISSOLVE_DIALOG_KEYWORDS_H

#include "gui/keywordwidgets/ui_dialog.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class KeywordList;

// Keywords Dialog
class KeywordsDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	KeywordsDialog(QWidget* parent, const KeywordList& keywords, const CoreData& coreData);
	// Destructor
	~KeywordsDialog();

	private:
	// Main form declaration
	Ui::KeywordsDialog ui_;
	// CoreData reference
	const CoreData& coreData_;
	// Whether any keywords have been modified in the current 'show'
	bool keywordsModified_;

	public:
	// Run the dialog, returning whether any modifications to keyword values were made
	bool showOptions();


	/*
	 * Slots
	 */
	private slots:
	void keywordChanged();
	void on_OKButton_clicked(bool checked);
};

#endif
