import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 2
import QtQuick.Window 2
import DataManagerReferencePointModel 1.0
import Qt.labs.qmlmodels 1.0

Item {
    id: main;
    required property DataManagerReferencePointModel ref;

    TableModel {
	id: myModel
	TableModelColumn { display: "name" }
	TableModelColumn { display: "color" }

	rows: [
	    {
		"name": "cat",
		"color": "black"
	    },
	    {
		"name": "dog",
		"color": "brown"
	    },
	    {
		"name": "bird",
		"color": "white"
	    }
	]
    }

    Dialog {
	title: "Data Manager"
	standardButtons: Dialog.Close
	onRejected: main.Window.window.close()
	visible: true
	RowLayout {
	    GroupBox {
		title: "Available Data"
		ColumnLayout {
		    RowLayout {
			Image {
			    Layout.preferredWidth: 20
			    Layout.preferredHeight: 20
			    source: "qrc:/general/icons/general_filter.svg"
			}
			TextField {
			    id: filterEdit
			    ToolTip.text: "Filter templates by name / description"
			    ToolTip.visible: hovered
			}
		    }
		    TableView {
			Layout.preferredWidth: 200
			Layout.preferredHeight: 200
			columnSpacing: 1
			rowSpacing: 1
			clip: true
			model: myModel; //ref;

			delegate: DelegateChooser {
			    DelegateChoice {
				column: 0
				delegate: TextField {
				    width: 100
				    text: display
				    color: "blue"
				}
			    }
			    DelegateChoice {
				column: 1
				delegate: TextField {
				    width: 100
				    text: display
				    color: "green"
				}
			    }
			}
		    }
		}
	    }
	    GroupBox {
		title: "Reference Points"
		RowLayout {
		    Button {
			icon.source: "qrc:/general/icons/general_remove.svg"
			enabled: false
			text: "Remove"
		    }
		    Button {
			icon.source: "qrc:/menu/icons/menu_open.svg"
			text: "Open"
		    }
		    Button {
			icon.source: "qrc:/menu/icons/menu_new.svg"
			text: "Create"
		    }
		}
	    }
	}
    }
}
