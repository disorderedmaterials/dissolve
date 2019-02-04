/*
	*** DGuidEd Window Functions
	*** src/gui/dguided_funcs.cpp
	Copyright T. Youngs 2019

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

#include "gui/dguided.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/modulechartmoduleblock.h"
#include "main/dissolve.h"
#include <QCloseEvent>
#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextList>
#include <QFontDatabase>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// Constructor
DGuidEdWindow::DGuidEdWindow(QMainWindow* parent) : QMainWindow(parent)
{
	// Initialise resources
	Q_INIT_RESOURCE(main);

	// Add our custom font
	QFontDatabase::addApplicationFont(":/dguided/fonts/SourceSansPro-Regular.ttf");

	// Call the main creation function
	ui_.setupUi(this);

	refreshing_ = true;

	ui_.TextEdit->setFocus();
	ui_.TextEdit->setFont(QFont("SourceSansPro-Regular", 12));
	setCurrentFilename(QString());

	// Create action group for text alignment options
	QActionGroup *grp = new QActionGroup(this);
	connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));
	grp->addAction(ui_.FormatAlignLeftAction);
	grp->addAction(ui_.FormatAlignRightAction);
	grp->addAction(ui_.FormatAlignCentreAction);
	grp->addAction(ui_.FormatAlignJustifyAction);

	// Create action group for list formatting
	grp = new QActionGroup(this);
	connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(listFormat(QAction*)));
	grp->addAction(ui_.FormatListNoneAction);
	grp->addAction(ui_.FormatListCircleAction);
	grp->addAction(ui_.FormatListSquareAction);
	grp->addAction(ui_.FormatListNumberAction);

	fontChanged(ui_.TextEdit->font());
	colorChanged(ui_.TextEdit->textColor());
	alignmentChanged(ui_.TextEdit->alignment());

	connect(ui_.TextEdit->document(), SIGNAL(contentsChanged()), this, SLOT(textEditContentChanged()));
	connect(ui_.TextEdit->document(), SIGNAL(undoAvailable(bool)), ui_.EditUndoAction, SLOT(setEnabled(bool)));
	connect(ui_.TextEdit->document(), SIGNAL(redoAvailable(bool)), ui_.EditRedoAction, SLOT(setEnabled(bool)));

	ui_.EditUndoAction->setEnabled(ui_.TextEdit->document()->isUndoAvailable());
	ui_.EditRedoAction->setEnabled(ui_.TextEdit->document()->isRedoAvailable());

	connect(ui_.EditUndoAction, SIGNAL(triggered()), ui_.TextEdit, SLOT(undo()));
	connect(ui_.EditRedoAction, SIGNAL(triggered()), ui_.TextEdit, SLOT(redo()));

	ui_.EditCutAction->setEnabled(false);
	ui_.EditCopyAction->setEnabled(false);

	connect(ui_.EditCutAction, SIGNAL(triggered()), ui_.TextEdit, SLOT(cut()));
	connect(ui_.EditCopyAction, SIGNAL(triggered()), ui_.TextEdit, SLOT(copy()));
	connect(ui_.EditPasteAction, SIGNAL(triggered()), ui_.TextEdit, SLOT(paste()));

	connect(ui_.TextEdit, SIGNAL(copyAvailable(bool)), ui_.EditCutAction, SLOT(setEnabled(bool)));
	connect(ui_.TextEdit, SIGNAL(copyAvailable(bool)), ui_.EditCopyAction, SLOT(setEnabled(bool)));

	// Generate Module icon insertion actions
	CoreData dummyData;
	Dissolve dissolve(dummyData);
	dissolve.registerMasterModules();
	ListIterator<Module> moduleIterator(dissolve.masterModules());
	while (Module* module = moduleIterator.iterate())
	{
		QAction* action = new QAction(ModuleChartModuleBlock::modulePixmap(module), module->type());
		ui_.ModuleIconToolBar->addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(insertModuleIcon()));
	}

	setModified(false);

	refreshing_ = false;

	updatePages();
}

// Destructor
DGuidEdWindow::~DGuidEdWindow()
{
}

/*
 * Window Functions
 */

// Set that the current data has (not) been modified
void DGuidEdWindow::setModified(bool modified)
{
	setWindowModified(modified);
	ui_.FileSaveAction->setEnabled(modified);
}

// Set current filename being edited
void DGuidEdWindow::setCurrentFilename(const QString& filename)
{
	filename_ = filename;
	ui_.TextEdit->document()->setModified(false);

	QString shownName;
	if (filename_.isEmpty()) shownName = "untitled.txt";
	else shownName = QFileInfo(filename_).fileName();

	setWindowTitle(tr("%1[*] - Dissolve Guide Editor").arg(shownName));
}

// Window close event
void DGuidEdWindow::closeEvent(QCloseEvent* event)
{
	if (saveBeforeClose()) event->accept();
	else event->ignore();
}


/*
 * File Menu
 */

// Save current guide under its original name
bool DGuidEdWindow::saveGuide()
{
	if (filename_.isEmpty()) return saveGuideAs();

	// Stash current content
	if (ui_.TextEdit->document()->isModified()) stashPageContent(currentPage());

	if (guide_.save(qPrintable(filename_)))
	{
		ui_.TextEdit->document()->setModified(false);
		setModified(false);
		return true;
	}

	return false;
}

// Save current guide under a new name
bool DGuidEdWindow::saveGuideAs()
{
	QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Dissolve Guide XML files (*.guide);;All Files (*)"));
	if (fn.isEmpty()) return false;

	if (!fn.endsWith(".guide", Qt::CaseInsensitive)) fn += ".guide";

	setCurrentFilename(fn);

	return saveGuide();
}

// Save modified content before close
bool DGuidEdWindow::saveBeforeClose()
{
	if (!isWindowModified()) return true;

	// Can't save to QResource?
	if (filename_.startsWith(QLatin1String(":/"))) return true;

	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(this, tr("DGuidEd"), tr("The guide has been modified.\n" "Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	if (ret == QMessageBox::Save) return saveGuide();
	else if (ret == QMessageBox::Cancel) return false;

	return true;
}

void DGuidEdWindow::on_FileLoadAction_triggered(bool checked)
{
	if (!saveBeforeClose()) return;

	QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("Dissolve Guide XML files (*.guide);;All Files (*)"));
	if (!fn.isEmpty()) openFile(qPrintable(fn));
}

void DGuidEdWindow::on_FileSaveAction_triggered(bool checked)
{
	saveGuide();
}

void DGuidEdWindow::on_FileSaveAsAction_triggered(bool checked)
{
	saveGuideAs();
}

void DGuidEdWindow::on_FileQuitAction_triggered(bool checked)
{
	if (!saveBeforeClose()) return;

	QApplication::exit(0);
}

// Open specified guide file
bool DGuidEdWindow::openFile(const char* filename)
{
	guide_.clear();

	if (!guide_.load(filename)) return false;

	// Set filename
	setCurrentFilename(filename);

	// Perform a major update of the page data
	updatePages();

	setModified(false);

	return true;
}

/*
 * Page Data
 */

// Page list row update function
void DGuidEdWindow::updatePageListRow(int row, GuidePage* page, bool createItem)
{
	QListWidgetItem* item;
	if (createItem)
	{
		item = new QListWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<GuidePage>(page));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		ui_.PageList->insertItem(row, item);
	}
	else item = ui_.PageList->item(row);

	// Set item data
	item->setText(page->name());
// 	item->setIcon(QIcon(dissolveReference_->findAtomType(atomType->name()) ?  ":/general/icons/general_warn.svg" : ":/general/icons/general_true.svg"));
}

// Return currently-selected page
GuidePage* DGuidEdWindow::currentPage()
{
	QListWidgetItem* item = ui_.PageList->currentItem();
	if (!item) return NULL;

	GuidePage* page = (GuidePage*) VariantPointer<GuidePage>(item->data(Qt::UserRole));

	return page;
}

// RichText Simplification filter - Elements to be discarded
bool DGuidEdWindow::filterElement(const QStringRef& name)
{
	return name != QStringLiteral("meta") && name != QStringLiteral("style");
}

// RichText Simplification filter - Filter attributes
void DGuidEdWindow::filterAttributes(const QStringRef& name, QXmlStreamAttributes* atts, bool* paragraphAlignmentFound)
{
	typedef QXmlStreamAttributes::iterator AttributeIt;

	if (atts->isEmpty()) return;

	// No style attributes for <body>
	if (name == QStringLiteral("body"))
	{
		atts->clear();
		return;
	}

	// Clean out everything except 'align' for 'p'
	if (name == QStringLiteral("p"))
	{
		for (AttributeIt it = atts->begin(); it != atts->end(); ) {
		if (it->name() == QStringLiteral("align")) {
			++it;
			*paragraphAlignmentFound = true;
		} else {
			it = atts->erase(it);
		}
		}
		return;
	}
}

// Simplification filter - check for blank QStringRef
bool DGuidEdWindow::isWhiteSpace(const QStringRef& in)
{
	const int count = in.size();
	for (int i = 0; i < count; i++)
	if (!in.at(i).isSpace()) return false;
	return true;
}

void DGuidEdWindow::on_PageAddButton_clicked(bool checked)
{
	// Stash any changes in the current page
	stashPageContent(currentPage());

	// Add a new page
	GuidePage* page = guide_.addEmptyPage();

	// Update the pages controls, and set our new page as the current one
	updatePages();
	setCurrentPage(page);

	setModified();
}

void DGuidEdWindow::on_PageRemoveButton_clicked(bool checked)
{
	GuidePage* page = currentPage();
	if (!page) return;

	// Determine the new current page before we delete this one
	GuidePage* newCurrentPage = page->next ? page->next : page->prev;

	// Remove the page
	guide_.removePage(page);

	// Update the list and set the new current page
	updatePages();
	setCurrentPage(newCurrentPage);

	setModified();
}

void DGuidEdWindow::on_PageCloneButton_clicked(bool checked)
{
}

void DGuidEdWindow::on_PageMoveDownButton_clicked(bool checked)
{
	GuidePage* page = currentPage();
	if (!page) return;

	// Stash any changes in the current page
	stashPageContent(page);

	// Move it down in the pages list
	guide_.shiftPageDown(page);

	// Update the pages list
	updatePages();

	// Retain this page as the currently-selected one
	setCurrentPage(page);

	setModified();
}

void DGuidEdWindow::on_PageMoveUpButton_clicked(bool checked)
{
	GuidePage* page = currentPage();
	if (!page) return;

	// Stash any changes in the current page
	stashPageContent(page);

	// Move it up in the pages list
	guide_.shiftPageUp(page);

	// Update the pages list
	updatePages();

	// Retain this page as the currently-selected one
	setCurrentPage(page);

	setModified();
}

void DGuidEdWindow::on_PageList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (refreshing_) return;

	// If the current document has been modified, stash its content back into the current page
	if (previous && ui_.TextEdit->document()->isModified())
	{
		GuidePage* page = (GuidePage*) VariantPointer<GuidePage>(previous->data(Qt::UserRole));
		stashPageContent(page);
	}

	updatePageData();
}

void DGuidEdWindow::on_PageList_itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	GuidePage* page = currentPage();
	if (!page) return;

	page->setName(qPrintable(item->text()));

	setModified();
}

void DGuidEdWindow::on_NextPageCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	GuidePage* page = currentPage();
	if (!page) return;

	page->setNextPage((GuidePage*) VariantPointer<GuidePage>(ui_.NextPageCombo->currentData()));

	setModified();
}

// Set the current page
void DGuidEdWindow::setCurrentPage(GuidePage* page)
{
	if (!page) return;

	int index = guide_.pages().indexOf(page);
	if (index == -1) return;

	ui_.PageList->setCurrentRow(index);
}

// Stash current content to specified page
void DGuidEdWindow::stashPageContent(GuidePage* page)
{
	if (!page) return;

	/*
	 * From QtDesigner src - https://github.com/cedrus/qt/blob/master/qttools_hold_for_concurrent/src/designer/src/lib/shared/richtexteditor.cpp
	 * Richtext simplification filter: Remove hard-coded font settings,
	 * <style> elements, <p> attributes other than 'align' and
	 * and unnecessary meta-information.
	 */
	unsigned elementCount = 0;
	bool paragraphAlignmentFound = false;
	QString simplifiedHtml;
	QXmlStreamReader reader(ui_.TextEdit->toHtml());
	QXmlStreamWriter writer(&simplifiedHtml);
	writer.setAutoFormatting(false);
	writer.setAutoFormattingIndent(0);

	while (!reader.atEnd())
	{
		switch (reader.readNext())
		{
			case (QXmlStreamReader::StartElement):
				elementCount++;
				if (filterElement(reader.name()))
				{
					const QStringRef name = reader.name();
					QXmlStreamAttributes attributes = reader.attributes();
					filterAttributes(name, &attributes, &paragraphAlignmentFound);
					writer.writeStartElement(name.toString());
					if (!attributes.isEmpty())
					writer.writeAttributes(attributes);
				}
				else
				{
					reader.readElementText(); // Skip away all nested elements and characters.
				}
				break;
			case (QXmlStreamReader::Characters):
				if (!isWhiteSpace(reader.text())) writer.writeCharacters(reader.text().toString());
				break;
			case (QXmlStreamReader::EndElement):
				writer.writeEndElement();
				break;
			default:
				break;
		}
	}

	page->setRichTextContent(simplifiedHtml);
}

// Update page lists
void DGuidEdWindow::updatePages()
{
	refreshing_ = true;

	// Make sure that the PageList and NextPageCombo are up to date
	ListWidgetUpdater<DGuidEdWindow,GuidePage>(ui_.PageList, guide_.pages(), this, &DGuidEdWindow::updatePageListRow);
	ui_.NextPageCombo->clear();
	ListIterator<GuidePage> pageIterator(guide_.pages());
	while (GuidePage* page = pageIterator.iterate()) ui_.NextPageCombo->addItem(page->name(), VariantPointer<GuidePage>(page));

	// Set a current Page if we don't have one already
	if ((!currentPage()) && (ui_.PageList->count() > 0)) ui_.PageList->setCurrentRow(0);

	refreshing_ = false;

	updatePageData();
}

// Update data for current page
void DGuidEdWindow::updatePageData()
{
	refreshing_ = true;

	GuidePage* page = currentPage();
	if (page)
	{
		// Get next page pointer
		ui_.NextPageCombo->setCurrentIndex(page->nextPage() ? guide_.pages().indexOf(page->nextPage()) : -1);
		ui_.NextPageCombo->setEnabled(true);

		// Set content of text edit
		ui_.TextEdit->setText(page->richTextContent());
		ui_.TextEdit->setEnabled(true);
	}
	else
	{
		ui_.NextPageCombo->setCurrentIndex(-1);
		ui_.NextPageCombo->setEnabled(false);
		ui_.TextEdit->clear();
		ui_.TextEdit->setEnabled(false);
	}

	// Set relevant button availability
	ui_.PageRemoveButton->setEnabled(page);
	ui_.PageCloneButton->setEnabled(page);
	ui_.PageMoveDownButton->setEnabled(page);
	ui_.PageMoveUpButton->setEnabled(page);

	refreshing_ = false;
}

/*
 * Content Editing
 */

void DGuidEdWindow::insertModuleIcon()
{
	// Cast sender into a QAction
	QAction* action = qobject_cast<QAction*> (sender());
	if (!action) return;

	// Reconstruct the full path the the icon for the module in our resource
	QString iconResource = QString(":/modules/icons/modules_%1.svg").arg(action->text().toLower());

	// Create a QImageFormat
	QTextImageFormat imageFormat;
	imageFormat.setWidth(32);
	imageFormat.setHeight(32);
	imageFormat.setName(iconResource);

	// Insert the picture into our document at the current cursor position
	QTextCursor cursor = ui_.TextEdit->textCursor();
	cursor.insertImage(imageFormat);
}

void DGuidEdWindow::textEditContentChanged()
{
	if (refreshing_) return;

	setModified();
}

void DGuidEdWindow::on_TextEdit_currentCharFormatChanged(const QTextCharFormat &format)
{
	fontChanged(format.font());
	verticalAlignmentChanged(format.verticalAlignment());
	colorChanged(format.foreground().color());
}

void DGuidEdWindow::on_TextEdit_cursorPositionChanged()
{
	alignmentChanged(ui_.TextEdit->alignment());

	QTextList *list = ui_.TextEdit->textCursor().currentList();
	if (list) listFormatChanged(list->format().style());
	else listFormatChanged(QTextListFormat::ListStyleUndefined);
}

void DGuidEdWindow::on_InsertAngstromAction_triggered()
{
	ui_.TextEdit->textCursor().insertHtml("&#8491;");
}

/*
 * Text Formatting
 */

void DGuidEdWindow::on_FormatBoldAction_triggered(bool checked)
{
	QTextCharFormat fmt;
	fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
	mergeFormatOnWordOrSelection(fmt);
}

void DGuidEdWindow::on_FormatItalicAction_triggered(bool checked)
{
	QTextCharFormat fmt;
	fmt.setFontItalic(checked);
	mergeFormatOnWordOrSelection(fmt);
}

void DGuidEdWindow::on_FormatUnderlineAction_triggered(bool checked)
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(checked);
	mergeFormatOnWordOrSelection(fmt);
}

void DGuidEdWindow::on_FormatSuperscriptAction_triggered(bool checked)
{
	QTextCharFormat fmt;
	fmt.setVerticalAlignment(checked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);
	mergeFormatOnWordOrSelection(fmt);
}

void DGuidEdWindow::on_FormatSubscriptAction_triggered(bool checked)
{
	QTextCharFormat fmt;
	fmt.setVerticalAlignment(checked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);
	mergeFormatOnWordOrSelection(fmt);
}

void DGuidEdWindow::on_FormatColourAction_triggered(bool checked)
{
	QColor col = QColorDialog::getColor(ui_.TextEdit->textColor(), this);
	if (!col.isValid()) return;

	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void DGuidEdWindow::on_FormatModuleNameAction_triggered(bool checked)
{
	QColor col = QColor(0,0,187);
	
	QTextCharFormat fmt;
	fmt.setForeground(col);
	fmt.setFontWeight(QFont::Bold);
	fmt.setFontItalic(false);

	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void DGuidEdWindow::on_FormatWidgetNameAction_triggered(bool checked)
{
	QColor col = QColor(0,187,0);
	
	QTextCharFormat fmt;
	fmt.setForeground(col);
	fmt.setFontWeight(QFont::Normal);
	fmt.setFontItalic(true);

	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void DGuidEdWindow::on_FormatStepsAction_triggered(bool checked)
{
	QColor col = QColor(187,187,187);
	
	QTextCharFormat fmt;
	fmt.setForeground(col);
	fmt.setFontWeight(QFont::Bold);
	fmt.setFontItalic(false);

	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void DGuidEdWindow::on_FormatClearAction_triggered(bool checked)
{
	clearFormatOnWordOrSelection();
}

void DGuidEdWindow::textAlign(QAction *a)
{
	if (a == ui_.FormatAlignLeftAction) ui_.TextEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
	else if (a == ui_.FormatAlignCentreAction) ui_.TextEdit->setAlignment(Qt::AlignHCenter);
	else if (a == ui_.FormatAlignRightAction) ui_.TextEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
	else if (a == ui_.FormatAlignJustifyAction) ui_.TextEdit->setAlignment(Qt::AlignJustify);
}

void DGuidEdWindow::listFormat(QAction *a)
{
	QTextCursor cursor = ui_.TextEdit->textCursor();
	QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

	if (a == ui_.FormatListNoneAction) style = QTextListFormat::ListStyleUndefined;
	else if (a == ui_.FormatListCircleAction) style = QTextListFormat::ListCircle;
	else if (a == ui_.FormatListSquareAction) style = QTextListFormat::ListSquare;
	else if (a == ui_.FormatListNumberAction) style = QTextListFormat::ListDecimal;

	cursor.beginEditBlock();

	QTextBlockFormat blockFmt = cursor.blockFormat();

	if (style == QTextListFormat::ListStyleUndefined) {
		blockFmt.setObjectIndex(-1);
		cursor.setBlockFormat(blockFmt);
		QTextCharFormat fmt;
		cursor.select(QTextCursor::LineUnderCursor);
		cursor.mergeCharFormat(fmt);
		ui_.TextEdit->mergeCurrentCharFormat(fmt);
	}
	else
	{
		QTextListFormat listFmt;
		if (cursor.currentList()) listFmt = cursor.currentList()->format();
		else
		{
			listFmt.setIndent(blockFmt.indent() + 1);
			blockFmt.setIndent(0);
			cursor.setBlockFormat(blockFmt);
		}
		listFmt.setStyle(style);
		cursor.createList(listFmt);
	}

	cursor.endEditBlock();
}

/*
 * Private Functions
 */

void DGuidEdWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = ui_.TextEdit->textCursor();
	if (!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	ui_.TextEdit->mergeCurrentCharFormat(format);
}

void DGuidEdWindow::clearFormatOnWordOrSelection()
{
	QTextCursor cursor = ui_.TextEdit->textCursor();
	if (!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
	cursor.setCharFormat(QTextCharFormat());
	ui_.TextEdit->mergeCurrentCharFormat(QTextCharFormat());
}

void DGuidEdWindow::fontChanged(const QFont &f)
{
//     comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
//     comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
	ui_.FormatBoldAction->setChecked(f.bold());
	ui_.FormatItalicAction->setChecked(f.italic());
	ui_.FormatUnderlineAction->setChecked(f.underline());
}

void DGuidEdWindow::colorChanged(const QColor &c)
{
	QPixmap pix(16, 16);
	pix.fill(c);
	ui_.FormatColourAction->setIcon(pix);
}

void DGuidEdWindow::alignmentChanged(Qt::Alignment a)
{
	if (a & Qt::AlignLeft)
	{
		ui_.FormatAlignLeftAction->setChecked(true);
	}
	else if (a & Qt::AlignHCenter)
	{
		ui_.FormatAlignCentreAction->setChecked(true);
	}
	else if (a & Qt::AlignRight)
	{
		ui_.FormatAlignRightAction->setChecked(true);
	}
	else if (a & Qt::AlignJustify)
	{
		ui_.FormatAlignJustifyAction->setChecked(true);
	}
}

void DGuidEdWindow::verticalAlignmentChanged(QTextCharFormat::VerticalAlignment alignment)
{
	ui_.FormatSuperscriptAction->setChecked(alignment == QTextCharFormat::AlignSuperScript);
	ui_.FormatSubscriptAction->setChecked(alignment == QTextCharFormat::AlignSubScript);
}

void DGuidEdWindow::listFormatChanged(QTextListFormat::Style style)
{
	switch (style)
	{
		case (QTextListFormat::ListCircle):
			ui_.FormatListCircleAction->setChecked(true);
			break;
		case (QTextListFormat::ListSquare):
			ui_.FormatListSquareAction->setChecked(true);
			break;
		case (QTextListFormat::ListDecimal):
			ui_.FormatListNumberAction->setChecked(true);
			break;
		default:
			ui_.FormatListNoneAction->setChecked(true);
			break;
	}
}

/*
 * Tools
 */

void DGuidEdWindow::on_ToolsSetLinearAction_triggered(bool checked)
{
	// Step through all pages and set the next page pointer to be the one immediately after
	ListIterator<GuidePage> pageIterator(guide_.pages());
	while (GuidePage* page = pageIterator.iterate())
	{
		if (pageIterator.isLast())
		{
			page->setNextPage(NULL);
			page->setNextPageName("");
			page->setFinishPoint(true);
		}
		else
		{
			GuidePage* nextPage = pageIterator.peek();
			page->setNextPage(nextPage);
			page->setNextPageName(nextPage ? nextPage->name() : "");
			page->setFinishPoint(false);
		}
	}

	// Update the current page data, in case its links have changed
	updatePageData();

	setModified(true);
}
