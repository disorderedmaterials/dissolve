import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property variant dialogModel;
    property variant fullData: atList.currentItem.fullData.raw;
    ListView {
	id: atList;
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: prefixButton.left;
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
	id: prefixButton;
	text: "Prefix";
	anchors.right: parent.right;
	anchors.top: parent.top;
    }
    Button {
	id: suffixButton;
	text: "Suffix";
	anchors.right: parent.right;
	anchors.top: prefixButton.bottom;
	onClicked: suffixDialog.open();
    }

    Dialog {
	id: suffixDialog
	title: "Suffix Dialog"

	standardButtons: Dialog.Ok | Dialog.Cancel
	onAccepted: dialogModel.atomTypes.addSuffix(atList.currentIndex , suffixField.text);

	TextField {
	    id: suffixField;
	    placeholderText: "Suffix";
	}

    }

    CheckBox {
	id: overwrite
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	text: "Overwrite Parameters in existing atom types";
    }
}
