import QtQuick 2
import QtQuick.Controls 2
import DataManagerReferencePointModel 1.0
import Qt.labs.platform

GroupBox {
    title: "Reference Points"
    Label {
	anchors.right: suffixField.left
    }
    TextField {
	id: suffixField
	anchors.right: removeButton.left
    }
    Button {
	id: removeButton
	icon.source: "qrc:/general/icons/general_remove.svg"
	anchors.right: openButton.left
	enabled: false
	text: "Remove"
    }
    Button {
	id: openButton
	anchors.top: parent.top
	anchors.right: createButton.left
	icon.source: "qrc:/menu/icons/menu_open.svg"
	text: "Open"
	onClicked: openDialog.open()
    }
    Button {
	id: createButton
	anchors.top: parent.top
	anchors.right: parent.right
	icon.source: "qrc:/menu/icons/menu_new.svg"
	text: "Create"
	onClicked: saveDialog.open()
    }
    HorizontalHeaderView {
	id: refTableHeader
	syncView: refTable
	anchors.top: createButton.bottom
	anchors.left: refTable.left
    }
    TableView {
	id: refTable
	anchors.right: parent.right
	anchors.left: parent.left
	anchors.bottom: parent.bottom
	anchors.top: refTableHeader.bottom
	clip: true
	model: ref
	delegate: Rectangle{
	    id: refItem
	    implicitWidth: refLabel.width + 5
	    implicitHeight: refLabel.height + 5
	    border.width: 1;
	    color: "white"
	    Label {
		id: refLabel
		text: display
		MouseArea {
		    anchors.fill: parent
		    onClicked: function() {
			console.log(row);
			console.log(column);
			console.log(selModel.selectedIndexes);
			selModel.select(ref.index(row, 0), ItemSelectionModel.ClearAndSelect);
			refTable.forceLayout();
		    }
		}
	    }
	    Connections {
		target: selModel
		function onSelectionChanged(selected, deselected) { refItem.color = selModel.isSelected(ref.index(row, 0)) ? "cyan" : "white";}

	    }
	}
    }
    ItemSelectionModel {
	id: selModel
	model: ref
    }
    FileDialog {
	id: openDialog
	fileMode: FileDialog.OpenFile
	title: "Please choose a file"
	nameFilters: ["Restart files (*.restart)", "All files (*)"]
	/* folder: shortcuts.home */
	onAccepted: ref.addFile(suffixField.text, file)
    }
    FileDialog {
	id: saveDialog
	fileMode: FileDialog.SaveFile
	title: "Please choose a file"
	defaultSuffix: "restart"
	nameFilters: ["Restart files (*.restart)", "All files (*)"]
	/* folder: shortcuts.home */
	onAccepted: function() {
	    ref.saveRestart(file)
	    ref.addFile(suffixField.text, file)
	}
    }
}
