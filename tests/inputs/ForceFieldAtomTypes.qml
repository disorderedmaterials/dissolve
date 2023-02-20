import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Item {
    property variant dialogModel;
    property variant fullData: atList.currentItem.fullData.raw;
    ListView {
	id: atList;
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: prefix.left;
	anchors.bottom: overwrite.top;
	height: 400;
	model: dialogModel.atomTypes;
	delegate: Text {
	    property variant fullData: model;
	    text: description;
	    color: ListView.isCurrentItem ? "red" : "black";
	    MouseArea {
		anchors.fill: parent;
		onClicked: atList.currentIndex = index;
	    }
	}
    }
    Button {
	id: prefix;
	text: "Prefix";
	anchors.right: parent.right;
	anchors.top: parent.top;
    }
    Button {
	id: suffix;
	text: "suffix";
	anchors.right: parent.right;
	anchors.top: prefix.bottom;
    }
    CheckBox {
	id: overwrite
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	text: "Overwrite Parameters in existing atom types";
    }
}
