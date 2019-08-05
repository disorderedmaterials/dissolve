/*
	*** Keyword Widget - FileAndFormat
	*** src/gui/keywordwidgets/fileandformat.h
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

#ifndef DISSOLVE_KEYWORDWIDGET_FILEANDFORMAT_H
#define DISSOLVE_KEYWORDWIDGET_FILEANDFORMAT_H

#include "gui/keywordwidgets/ui_fileandformat.h"
#include "gui/keywordwidgets/dropdown.h"
#include "keywords/fileandformat.h"
#include "gui/keywordwidgets/base.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class QComboBox;

class FileAndFormatKeywordWidget : public QWidget, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	FileAndFormatKeywordWidget(QWidget* parent, KeywordBase* keyword, const Dissolve& dissolve, const CoreData& coreData, GenericList& moduleData, const char* prefix);
        // Main form declaration
        Ui::FileAndFormatWidget ui;


	/*
	 * Reference to Dissolve
	 */
	private:
	// Reference to Dissolve (used to get input file dir in order to generate relative path names)
	const Dissolve& dissolve_;


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	FileAndFormatKeyword* keyword_;


	/*
	 * Signals / Slots
	 */
	private slots:
	void on_FileEdit_editingFinished();
	void on_FileEdit_returnPressed();
	void on_FileFormatCombo_currentIndexChanged(int index);
	void on_FileSelectButton_clicked(bool checked);

	signals:
	// Keyword value changed
	void keywordValueChanged();


	/*
	 * Update
	 */
	private:
	// Check / set validity of current filename
	void checkFileValidity();

	public:
	// Update value displayed in widget, using specified source if necessary
	void updateValue();
	// Update widget values data based on keyword data
	void updateWidgetValues(const CoreData& coreData);
	// Update keyword data based on widget values
	void updateKeywordData();
};

#endif
