/*
	*** uChroma Base 
	*** src/gui/uchroma/uchromabase.h
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_UCHROMABASE_H
#define DUQ_UCHROMABASE_H

#include "base/charstring.h"

#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/classes/viewlayout.h"
#include "gui/uchroma/session/editstategroup.h"
#include "gui/uchroma/gui/viewer.hui"
#include "gui/uchroma/gui/axes.h"
#include "gui/uchroma/gui/create.h"
#include "gui/uchroma/gui/data.h"
#include "gui/uchroma/gui/editfitkernel.h"
#include "gui/uchroma/gui/style.h"
#include "gui/uchroma/gui/transform.h"
#include "gui/uchroma/gui/view.h"
#include "gui/uchroma/gui/saveimage.h"
#include "gui/uchroma/gui/import.h"

// Forward Declarations
class Axes;
class DataSet;
class LineParser;
class Viewer;

// uChroma Base
class UChromaBase
{
	public:
	// Constructor // Destructor
	UChromaBase(QWidget* parent);
	virtual ~UChromaBase();

	protected:
	// Pointer to parent (derived) widget
	QWidget* parent_;

	public:
	// Return pointer to parent (derived) widget
	QWidget* parent();


	/*
	 * Window
	 */
	protected:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Return centre coordinate of parent widget
	virtual QPoint centrePos() = 0;


	/*
	 * Update
	 */
	public:
	// Update all aspects of GUI
	virtual void updateGUI() = 0;
	// Update controls in all subwindows
	virtual void updateSubWindows() = 0;
	// Update tool bars
	virtual void updateToolBars() = 0;
	// Update main display
	virtual void updateDisplay() = 0;
	// Update undo/redo menu
	virtual void updateUndoRedo() = 0;
	// Update coordinate info
	virtual void updateCoordinateInfo() = 0;
	// Update text of renderTimeLabel_ in statusbar
	virtual void updateRenderTimeLabel(QString text) = 0;
	// Update title bar of main window
	virtual void updateTitleBar() = 0;


	/*
	 * Interaction
	 */
	public:
	// Interaction Mode enum
	enum InteractionMode
	{
		FitSetupSelectXInteraction,
		FitSetupSelectZInteraction,
		OperateBGSubSelectYValueInteraction,
		OperateBGSubSelectXRangeInteraction,
		OperateBGSubSelectZRangeInteraction,
		ViewInteraction,
		ZoomInteraction,
		nInteractionModes
	};
	// Return InteractionMode name
	static const char* interactionModeName(InteractionMode mode);
	// Return InteractionMode help text
	static const char* interactionModeHelp(InteractionMode mode);

	protected:
	// Viewer present in derived class
	Viewer* viewer_;
	// Current interaction mode
	UChromaBase::InteractionMode interactionMode_;
	// Current axis target for interaction
	int interactionAxis_;
	// Whether interaction has started (i.e. mouse is down)
	bool interactionStarted_;
	// Starting (clicked) value when interacting
	double clickedInteractionValue_;
	// Starting (clicked) keyboard modifiers when interacting
	Qt::KeyboardModifiers clickedInteractionModifiers_;
	// Object at clicked position (if ViewInteraction)
	Viewer::ViewObject clickedObject_;
	// Info for object at clicked position (if ViewInteraction)
	CharString clickedObjectInfo_;
	// Current interaction position on axis
	double currentInteractionValue_;

	public:
	// Return Viewer present in derived class
	Viewer* viewer();
	// Set interaction mode and target axis
	void setInteractionMode(UChromaBase::InteractionMode mode, int axis);
	// Return interaction mode
	UChromaBase::InteractionMode interactionMode();
	// Return current axis target for interaction
	int interactionAxis();
	// Return whether interaction has started (i.e. mouse is down)
	bool interactionStarted();
	// Cancel current interaction
	void cancelInteraction();
	// Start interaction at the specified screen coordinates
	void startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers);
	// Update current interaction value
	void updateInteractionPosition(int mouseX, int mouseY);
	// End interaction at the specified screen coordinates
	void endInteraction(int mouseX, int mouseY);
	// Return clicked interaction value on axis
	double clickedInteractionValue();
	// Return current interaction value on axis
	double currentInteractionValue();
	// Return clicked interaction coordinate on axis
	double clickedInteractionCoordinate();
	// Return current interaction coordinate on axis
	double currentInteractionCoordinate();
	// Return current slice data
	DataSet* currentSlice();
	// Perform relevant double-click action, occurring at specified coordinate
	void doubleClickInteraction(int mouseX, int mouseY);
	// Calculate local coordinate from supplied screen coordinates
	Vec3<double> screenToLocal(int mouseX, int mouseY);


	/*
	 * Tool Windows
	 */
	protected:
	// Axes Window
	AxesWindow axesWindow_;
	// Data Window
	DataWindow dataWindow_;
	// Style Window
	StyleWindow styleWindow_;
	// Transform Window
	TransformWindow transformWindow_;
	// View Window
	ViewWindow viewWindow_;
	// Create Collection Dialog
	CreateCollectionDialog createCollectionDialog_;
	// Import Dialog
	ImportDialog importDialog_;
	// Fit Window
	EditFitKernelDialog editFitKernelDialog_;
	// Save Image Dialog
	SaveImageDialog saveImageDialog_;


	/*
	 * Modification Flag
	 */
	protected:
	// Modified flag
	bool modified_;

	public:
	// Set status to true
	void setAsModified();
	// Set status to false
	void setAsNotModified();
	// Return modified status
	bool isModified();


	/*
	 * General Data
	 */
	public:
	// Available image export formats
        enum ImageFormat { BMPFormat, PGFormat, PNGFormat, PPMFormat, XBMFormat, X11Format, nImageFormats };
        static ImageFormat imageFormat(const char* s);
        static ImageFormat imageFormatFromFilter(const char* s);
        static const char* imageFormatFilter(ImageFormat bf);
        static const char* imageFormatExtension(ImageFormat bf);

	protected:
	// Current image export filename
	CharString imageExportFileName_;
	// Format for exported image
	ImageFormat imageExportFormat_;
	// Size of exported image
	int imageExportWidth_, imageExportHeight_;
	// Whether to maintain current aspect ratio on image export
	bool imageExportMaintainAspect_;
	// Font file to use for viewer
	static CharString viewerFontFileName_;

	public:
	// Set current image export filename
	void setImageExportFileName(const char* fileName);
	// Return current image export filename
	const char* imageExportFileName();
	// Set format for exported image
	void setImageExportFormat(ImageFormat fmt);
	// Return format for exported image
	ImageFormat imageExportFormat();
	// Set width of exported image
	void setImageExportWidth(int width);
	// Return width of exported image
	int imageExportWidth();
	// Set height of exported image
	void setImageExportHeight(int height);
	// Return height of exported image
	int imageExportHeight();
	// Set whether to maintain current aspect ratio on image export
	void setImageExportMaintainAspect(bool b);
	// Return whether to maintain current aspect ratio on image export
	bool imageExportMaintainAspect();
	// Set font filename to use for viewer
	static void setViewerFontFileName(const char* fileName);
	// Return font file to use for viewer
	static const char* viewerFontFileName();


	/*
	 * Collections
	 */
	protected:
	// List of Collections
	List<Collection> collections_;
	// Currently-selected Collection
	Collection* currentCollection_;

	protected:
	// Return unique name based on supplied baseName
	const char* uniqueCollectionName(const char* baseName);

	public:
	// Setup new, empty session
	void startNewSession(bool createDefaults);
	// Add new collection
	Collection* addCollection(const char* name = NULL, int index = -1);
	// Add new collection to the current view pane
	Collection* addCollectionToCurrentViewPane(const char* name = NULL, int identifier = -1);
	// Add new collection at the specified location
	Collection* addCollectionFromLocator(const char* locator, Collection::CollectionType type, int listIndex);
	// Add new Collection from Collection block data
	Collection* addCollectionFromBlock(const char* blockData);
	// Remove existing collection
	void removeCollection(Collection* collection);
	// Clear all collections
	void clearCollections();
	// Remove current collection
	void removeCurrentCollection();
	// Return number of collections
	int nCollections();
	// Return first collection in list
	Collection* collections();
	// Return nth collection in list
	Collection* collection(int index);
	// Return index of specified collection
	int collectionIndex(Collection* collection);
	// Locate named collection
	Collection* locateCollection(const char* locator);
	// Move collection focus to next in list
	void focusNextCollection();
	// Move collection focus to previous in list
	void focusPreviousCollection();
	// Set current Collection
	void setCurrentCollection(Collection* collection);
	// Return current Collection
	Collection* currentCollection();
	// Return whether specified collection is the current collection
	bool isCurrentCollection(Collection* collection);
	// Identify collection
	Collection* identifyCollection(int id);
	// Set collection visibility
	bool setCollectionVisible(const char* name, bool visible);
	// Return collection visibility
	bool collectionVisible(const char* name);
	// Refresh referenced DataSets in all Collections
	void refreshReferencedDataSets();


	/*
	 * View Layout
	 */
	protected:
	// Current view layout
	ViewLayout viewLayout_;
	// Current view pane
	ViewPane* currentViewPane_;

	public:
	// Return current view layout
	ViewLayout& viewLayout();
	// Set view layout
	void setViewLayout(ViewLayout& source);
	// Recalculate layout pane positions / sizes (after context resize etc.)
	void recalculateViewLayout(int contextWidth, int contextHeight);
	// Set current view pane to the one under the specified screen coordinates
	bool setCurrentViewPane(int layoutX, int layoutY);
	// Set current view pane
	bool setCurrentViewPane(ViewPane* viewPane);
	// Return current view pane
	ViewPane* currentViewPane();


	/*
	 * Data
	 */
	protected:
	// Current session file directory
	QDir sessionFileDirectory_;
	// Current input filename
	CharString inputFile_;
	// Whether to enforce hard fail on input file error
	bool hardIOFail_;

	public:
	// Input File Block Keyword Enum
	enum InputBlock
	{
		CollectionBlock,
		EndUChromaBlock,
		SettingsBlock,
		ViewBlock,
		nInputBlocks
	};
	// Convert text string to InputBlock
	InputBlock inputBlock(const char* s);
	// Convert InputBlock to text string
	const char* inputBlock(InputBlock id);
	// Print list of valid keywords for InputBlock specified
	void printValidKeywords(InputBlock block);

	// Axis Block Keyword Enum
	enum AxisKeyword
	{
		AutoScaleKeyword,
		AutoTicksKeyword,
		EndAxisKeyword,
		FirstTickKeyword,
		FractionalPositioningKeyword,
		GridLinesKeyword,
		GridLineMajorStyleKeyword,
		GridLineMinorStyleKeyword,
		InvertKeyword,
		LabelAnchorKeyword,
		LabelOrientationKeyword,
		LimitsKeyword,
		LogarithmicKeyword,
		MinorTicksKeyword,
		NumberFormatKeyword,
		PositionFractionalKeyword,
		PositionRealKeyword,
		StretchKeyword,
		TickDeltaKeyword,
		TickDirectionKeyword,
		TitleKeyword,
		TitleAnchorKeyword,
		TitleOrientationKeyword,
		VisibleAxisKeyword,
		nAxisKeywords
	};
	// Convert text string to AxisKeyword
	AxisKeyword axisKeyword(const char* s);
	// Convert AxisKeyword to text string
	const char* axisKeyword(AxisKeyword kwd);
	// Return minimum number of expected arguments
	int axisKeywordNArguments(AxisKeyword kwd);

	// Collection Block Keyword Enum
	enum CollectionKeyword
	{
		ColourAlphaControlKeyword,
		ColourAlphaFixedKeyword,
		ColourCustomGradientKeyword,
		ColourRGBGradientAKeyword,
		ColourRGBGradientBKeyword,
		ColourHSVGradientAKeyword,
		ColourHSVGradientBKeyword,
		ColourSingleKeyword,
		ColourSourceKeyword,
		DataDirectoryKeyword,
		DataSetDefinitionKeyword,
		EndCollectionKeyword,
		FitBlockKeyword,
		FitParametersBlockKeyword,
		GroupKeyword,
		InterpolateKeyword,
		InterpolateConstrainKeyword,
		InterpolateStepKeyword,
		LineStyleKeyword,
		ShininessKeyword,
		SliceBlockKeyword,
		StyleKeyword,
		TransformXKeyword,
		TransformYKeyword,
		TransformZKeyword,
		VisibleCollectionKeyword,
		nCollectionKeywords
	};
	// Convert text string to CollectionKeyword
	CollectionKeyword collectionKeyword(const char* s);
	// Convert CollectionKeyword to text string
	const char* collectionKeyword(CollectionKeyword kwd);
	// Return minimum number of expected arguments
	int collectionKeywordNArguments(CollectionKeyword kwd);

	// DataSet Block Keyword Enum
	enum DataSetKeyword
	{
		DataKeyword,
		EndDataSetKeyword,
		SourceKeyword,
		ZKeyword,
		nDataSetKeywords
	};
	// Convert text string to DataSetKeyword
	DataSetKeyword dataSetKeyword(const char* s);
	// Convert DataSetKeyword to text string
	const char* dataSetKeyword(DataSetKeyword kwd);
	// Return minimum number of expected arguments
	int dataSetKeywordNArguments(DataSetKeyword kwd);

	// FitParameters Block Keyword Enum
	enum FitParametersKeyword
	{
		EndFitParametersKeyword,
		EquationKeyword,
		FitResultsBlockKeyword,
		GlobalKeyword,
		LimitStrengthKeyword,
		OrthogonalKeyword,
		ReferenceKeyword,
		VariableKeyword,
		XRangeTypeKeyword,
		XRangeAbsoluteKeyword,
		XRangeIndexKeyword,
		XRangeIndexSingleKeyword,
		ZRangeTypeKeyword,
		ZRangeAbsoluteKeyword,
		ZRangeIndexKeyword,
		ZRangeIndexSingleKeyword,
		nFitParametersKeywords
	};
	// Convert text string to FitParametersKeyword
	FitParametersKeyword fitParametersKeyword(const char* s);
	// Convert FitParametersKeyword to text string
	const char* fitParametersKeyword(FitParametersKeyword kwd);
	// Return minimum number of expected arguments
	int fitParametersKeywordNArguments(FitParametersKeyword kwd);

	// FitResults Block Keyword Enum
	enum FitResultsKeyword
	{
		EndFitResultsKeyword,
		FittedValueKeyword,
		nFitResultsKeywords
	};
	// Convert text string to FitResultsKeyword
	FitResultsKeyword fitResultsKeyword(const char* s);
	// Convert FitResultsKeyword to text string
	const char* fitResultsKeyword(FitResultsKeyword kwd);
	// Return minimum number of expected arguments
	int fitResultsKeywordNArguments(FitResultsKeyword kwd);

	// Settings Block Keyword Enum
	enum SettingsKeyword
	{
		EndSettingsKeyword,
		ImageExportKeyword,
		nSettingsKeywords
	};
	// Convert text string to SettingsKeyword
	SettingsKeyword settingsKeyword(const char* s);
	// Convert SettingsKeyword to text string
	const char* settingsKeyword(SettingsKeyword kwd);
	// Return minimum number of expected arguments
	int settingsKeywordNArguments(SettingsKeyword kwd);

	// View Block Keyword Enum
	enum ViewKeyword
	{
		EndViewKeyword,
		GridKeyword,
		ViewPaneBlockKeyword,
		nViewKeywords
	};
	// Convert text string to ViewKeyword
	ViewKeyword viewKeyword(const char* s);
	// Convert ViewKeyword to text string
	const char* viewKeyword(ViewKeyword kwd);
	// Return minimum number of expected arguments
	int viewKeywordNArguments(ViewKeyword kwd);

	// ViewPane Block Keyword Enum
	enum ViewPaneKeyword
	{
		AutoPositionTitlesKeyword,
		AxisBlockKeyword,
		BoundingBoxKeyword,
		BoundingBoxPlaneYKeyword,
		EndViewPaneKeyword,
		FlatLabelsKeyword,
		GeometryKeyword,
		LabelPointSizeKeyword,
		PerspectiveKeyword,
		RoleKeyword,
		RoleTargetCollectionKeyword,
		RoleTargetPaneKeyword,
		RotationXKeyword,
		RotationYKeyword,
		RotationZKeyword,
		TitlePointSizeKeyword,
		TranslationKeyword,
		UseBestFlatViewKeyword,
		ViewTypeKeyword,
		nViewPaneKeywords
	};
	// Convert text string to ViewKeyword
	ViewPaneKeyword viewPaneKeyword(const char* s);
	// Convert ViewKeyword to text string
	const char* viewPaneKeyword(ViewPaneKeyword kwd);
	// Return minimum number of expected arguments
	int viewPaneKeywordNArguments(ViewPaneKeyword kwd);

	public:
	// Read AxisBlock keywords
	bool readAxisBlock(LineParser& parser, Axes& axes, int axis, bool strictBlockEnd = true);
	// Read CollectionBlock keywords
	bool readCollectionBlock(LineParser& parser, Collection* collection, bool strictBlockEnd = true);
	// Read DataSetBlock keywords
	bool readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection, bool strictBlockEnd = true);
	// Read FitParametersBlock keywords
	bool readFitParametersBlock(LineParser& parser, FitKernel* fitKernel, bool strictBlockEnd = true);
	// Read FitResultsBlock keywords
	bool readFitResultsBlock(LineParser& parser, DataSpaceRange* range, bool strictBlockEnd = true);
	// Read SettingsBlock keywords
	bool readSettingsBlock(LineParser& parser, bool strictBlockEnd = true);
	// Read ViewBlock keywords
	bool readViewBlock(LineParser& parser, bool strictBlockEnd = true);
	// Read ViewPaneBlock keywords
	bool readViewPaneBlock(LineParser& parser, ViewPane* pane, bool strictBlockEnd = true);
	// Write AxisBlock keywords
	bool writeAxisBlock(LineParser& parser, Axes& axes, int axis);
	// Write CollectionBlock keywords
	bool writeCollectionBlock(LineParser& parser, Collection* collection, Collection::CollectionType type = Collection::MasterCollection, int indentLevel = 0);
	// Write DataSetBlock keywords
	bool writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel = 0);
	// Write FitParametersBlock keywords
	bool writeFitParametersBlock(LineParser& parser, FitKernel* fitKernel, int indentLevel = 0);
	// Write FitResultsBlock keywords
	bool writeFitResultsBlock(LineParser& parser, DataSpaceRange* range, int rangeID = 0, int indentLevel = 0);
	// Write SettingsBlock keywords
	bool writeSettingsBlock(LineParser& parser);
	// Write ViewBlock keywords
	bool writeViewBlock(LineParser& parser);
	// Write ViewPaneBlock keywords
	bool writeViewPaneBlock(LineParser& parser, ViewPane* pane);
	
	public:
	// Current session file directory
	void setSessionFileDirectory(QDir dir);
	// Return current session file directory
	QDir sessionFileDirectory();
	// Set name of input file
	void setInputFile(const char* fileName);
	// Return name of input file
	const char* inputFile();
	// Set whether to enforce hard fail on session file error
	void setHardIOFail(bool hardFail);
	// Parse main input blocks through specified parser
	bool parseInputBlocks(LineParser& parser);
	// Load session from file specified
	bool loadSession(const char* fileName);
	// Save session input to file specified
	bool saveSession(const char* fileName);
	// Read session through parser specified
	bool readSession(LineParser& parser);
	// Write session through parser specified
	bool writeSession(LineParser& parser);


	/*
	 * Undo / Redo
	 */
	protected:
	// List of EditStateGroups
	List<EditStateGroup> editStateGroups_;
	// Current EditStateGroup being created
	EditStateGroup* currentEditStateGroup_;
	// Last EditState created in this group
	EditState* currentEditState_;
	// Target EditStateGroup for Undo
	EditStateGroup* undoEditStateGroup_;

	public:
	// Begin a new edit state group
	EditStateGroup* beginEditStateGroup(const char* format, ...);
	// Add edit state to group
	bool addEditState(ObjectInfo target, int quantity, int index = -1, int subIndex = -1);
	// Add edit state to group with basic integer data
	bool addEditState(ObjectInfo target, int quantity, int oldData, int newData, int index, int subIndex = -1);
	// Add edit state to group with basic double data
	bool addEditState(ObjectInfo target, int quantity, double oldData, double newData, int index = -1, int subIndex = -1);
	// Add edit state to group with basic const char* data
	bool addEditState(ObjectInfo target, int quantity, const char* oldData, const char* newData, int index = -1, int subIndex = -1);
	// Add edit state to group with LineStyle data
	bool addEditState(ObjectInfo target, int quantity, LineStyle oldData, LineStyle newData, int index = -1, int subIndex = -1);
	// Add data to current EditState (int)
	void addEditStateData(bool newData, const char* name, int value);
	// Add data to current EditState  (double)
	void addEditStateData(bool newData, const char* name, double value);
	// Add data to current EditState  (const char*)
	void addEditStateData(bool newData, const char* name, const char* value);
	// Add data to current EditState  (from Collection*)
	void addEditStateData(bool newData, const char* name, Collection* value);
	// Add data to current EditState  (from XYData*)
	void addEditStateData(bool newData, const char* name, XYData* value);
	// Add data to current EditState  (from LineStyle&)
	void addEditStateData(bool newData, const char* name, LineStyle& value);
	// End the new edit state group
	void endEditStateGroup();
	// Return current EditStateGroup being created
	EditStateGroup* currentEditStateGroup();
	// Return target EditStateGroup for undo
	EditStateGroup* undoEditStateGroup();
	// Return first in list of EditStateGroups
	EditStateGroup* editStateGroups();
	// Clear all edit state data
	void clearEditStates();
	// Perform undo
	bool undo();
	// Perform redo
	bool redo();
};

#endif
