import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property variant dialogModel;

    Rectangle {
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: prefixButton.left;
	anchors.bottom: overwrite.top;
	height: 400;
	color: palette.active.base;
	IconListView {
	    id: atList;
	    anchors.fill: parent;
	    model: dialogModel.atomTypes;
	}
    }
    Button {
	id: prefixButton;
	text: "Prefix";
	anchors.right: parent.right;
	anchors.top: parent.top;
	onClicked: prefixDialog.open();
	enabled: atList.currentIndex >= 0;
    }

    Dialog {
	id: prefixDialog
	title: "Prefix Dialog"

	standardButtons: Dialog.Ok | Dialog.Cancel
	onAccepted: dialogModel.atomTypes.addPrefix(atList.currentIndex , prefixField.text);

	TextField {
	    id: prefixField;
	    placeholderText: "Prefix";
	}

    }

    Button {
	id: suffixButton;
	text: "Suffix";
	anchors.right: parent.right;
	anchors.top: prefixButton.bottom;
	onClicked: suffixDialog.open();
	enabled: atList.currentIndex >= 0;
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
	anchors.bottom: indicator.top;
	anchors.left: parent.left;
	text: "Overwrite Parameters in existing atom types";
    }

    Image {
	id: indicatorImage;
	source: dialogModel.atomTypesIndicator == 0 ? "qrc:/general/icons/general_true.svg" : "qrc:/general/icons/general_warn.svg";
	anchors.top: indicator.top;
	anchors.bottom: indicator.bottom;
	anchors.left: parent.left;
	fillMode: Image.PreserveAspectFit;
    }
    Text {
	id: indicator;
	text: dialogModel.atomTypesIndicator == 0 ? "There are no naming conflicts with the assigned atom types" : (dialogModel.atomTypesIndicator  + " assigned atom types conflict with existing types");
	anchors.bottom: parent.bottom;
	anchors.left: indicatorImage.right;
	anchors.right: parent.right;
	wrapMode: Text.Wrap;
    }

    Connections {
	target: dialogModel.atomTypes;
	function onDataChanged(topLeft, bottomRight) {
	    dialogModel.atomTypesIndicatorChanged();
	}
    }

    Binding {
	target: dialogModel;
	property: "overwriteParametersCheck";
	value: overwrite.checked;
    }

    Component.onCompleted: atList.forceActiveFocus();
}
