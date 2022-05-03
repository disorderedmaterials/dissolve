import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 2
import QtQuick.Window 2
import DataManagerSimulationModel 1.0
import SortFilterProxyModel 1.0

Rectangle {
    id: main;
    required property DataManagerSimulationModel sim;
    anchors.fill: parent
    color: red

    SortFilterProxyModel {
	id: sims
	source: sim
	filterString: filterEdit.text
    }

    GroupBox {
	id: databox
	anchors.top: parent.top
	anchors.bottom: parent.bottom
	anchors.left: parent.left
	anchors.right: refbox.left
	title: "Available Data"
	Image {
	    anchors.top: parent.top
	    anchors.right: filterEdit.left
	    anchors.bottom: filterEdit.bottom
	    width: height
	    source: "qrc:/general/icons/general_filter.svg"
	}
	TextField {
	    id: filterEdit
	    anchors.top: parent.top
	    anchors.right: parent.right
	    ToolTip.text: "Filter templates by name / description"
	    ToolTip.visible: hovered
	}
	TableView {
	    anchors.top: filterEdit.bottom
	    anchors.left: parent.left
	    anchors.right: parent.right
	    anchors.bottom: parent.bottom
	    clip: true
	    model: sims
	    delegate: Rectangle{
		implicitWidth: simLabel.width + 5
		implicitHeight: simLabel.height + 5
		border.width: 1;
		color: "white";
		Label {
		    id: simLabel
		    text: display
		}
	    }
	}
    }
    GroupBox {
	id: refbox
	anchors.top: parent.top
	anchors.bottom: parent.bottom
	anchors.right: parent.right
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
