/*
	*** DGuidEd Main Window
	*** src/gui/dguided.h
	Copyright T. Youngs 2019

	This file is part of DGuidEd.

	DGuidEd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	DGuidEd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with DGuidEd.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DGUIDED_MAINWINDOW_H
#define DGUIDED_MAINWINDOW_H

#include "gui/ui_dguided.h"
#include "gui/guide.h"

// Forward Declarations
class QXmlStreamAttributes;

class DGuidEdWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	DGuidEdWindow(QMainWindow* parent = 0);
	~DGuidEdWindow();


	/*
	 * Main UI
	 */
	private:
	// Main form declaration
	Ui::DGuidEdWindow ui_;
	// Whether the window is currently refreshing
	bool refreshing_;

	private:
	// Set that the current data has been modified in some way
	void setModified(bool modified = true);
	// Set current filename being edited
	void setCurrentFilename(const QString& filename);

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);


	/*
	 * File Handling
	 */
	private:
	// Resource location for the current file
	QString filename_;
	// Guide upon which we are working
	Guide guide_;

	private:
	// Save current guide under its original name
	bool saveGuide();
	// Save current guide under a new name
	bool saveGuideAs();
	// Save modified content before close
	bool saveBeforeClose();

	private slots:
	void on_FileLoadAction_triggered(bool checked);
	void on_FileSaveAction_triggered(bool checked);
	void on_FileSaveAsAction_triggered(bool checked);
	void on_FileQuitAction_triggered(bool checked);

	public:
	// Open specified guide file
	bool openFile(const char* filename);


	/*
	 * Page Data
	 */
	private:
	// Page list row update function
	void updatePageListRow(int row, GuidePage* page, bool createItem);
	// Return currently-selected page
	GuidePage* currentPage();
	// RichText Simplification filter - Elements to be discarded
	bool filterElement(const QStringRef& name);
	// RichText Simplification filter - Filter attributes
	void filterAttributes(const QStringRef& name, QXmlStreamAttributes* atts, bool* paragraphAlignmentFound);
	// Simplification filter - check for blank QStringRef
	bool isWhiteSpace(const QStringRef& in);


	private slots:
	void on_PageAddButton_clicked(bool checked);
	void on_PageRemoveButton_clicked(bool checked);
	void on_PageCloneButton_clicked(bool checked);
	void on_PageMoveDownButton_clicked(bool checked);
	void on_PageMoveUpButton_clicked(bool checked);
	void on_PageList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void on_PageList_itemChanged(QListWidgetItem* item);
	void on_NextPageCombo_currentIndexChanged(int index);

	public:
	// Set the current page
	void setCurrentPage(GuidePage* page);
	// Stash current page content
	void stashPageContent(GuidePage* page);
	// Update page lists
	void updatePages();
	// Update data for current page
	void updatePageData();


	/*
	 * Content Editing
	 */
	private:
	// List of generated QActions that insert Module icons
	QList<QAction*> moduleIconActions_;

	private slots:
	void insertModuleIcon();
	void textEditContentChanged();
	void on_TextEdit_currentCharFormatChanged(const QTextCharFormat &format);
	void on_TextEdit_cursorPositionChanged();
	void on_InsertAngstromAction_triggered();


	/*
	 * Text Formatting
	 */
	private slots:
	void on_FormatBoldAction_triggered(bool checked);
	void on_FormatItalicAction_triggered(bool checked);
	void on_FormatUnderlineAction_triggered(bool checked);
	void on_FormatSuperscriptAction_triggered(bool checked);
	void on_FormatSubscriptAction_triggered(bool checked);
	void on_FormatColourAction_triggered(bool checked);
	void on_FormatModuleNameAction_triggered(bool checked);
	void on_FormatClearAction_triggered(bool checked);
	void textAlign(QAction *a);


	/*
	 * Private Functions
	 */
	private:
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
	void clearFormatOnWordOrSelection();
	void fontChanged(const QFont &f);
	void colorChanged(const QColor &c);
	void alignmentChanged(Qt::Alignment a);
	void verticalAlignmentChanged(QTextCharFormat::VerticalAlignment alignment);


	/*
	 * Tools
	 */
	private slots:
	void on_ToolsSetLinearAction_triggered(bool checked);
};

#endif
