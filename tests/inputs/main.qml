import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Rectangle {
    id: root;
    visible: true;
    anchors.fill: parent;
    color: "#EEEEEE";
    signal qmlCancel();

    StackLayout {
	id: stack;
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: parent.right;
	anchors.bottom: nextButton.top;
	currentIndex: 0;

	ForceFieldPicker {
	    id: ffList;
	}

	ForceFieldAssign {
	}

	ForceFieldAtomTypes {
	}
    }

    Button {
	text: "Cancel";
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	onClicked: root.qmlCancel();
    }
    Button {
	id: nextButton;
	text: "Next";
	anchors.bottom: parent.bottom;
	anchors.right: parent.right;
	onClicked: stack.currentIndex += 1;
    }
    Button {
	text: "Back";
	anchors.bottom: parent.bottom;
	anchors.right: nextButton.left;
	enabled: stack.currentIndex > 0;
	onClicked: stack.currentIndex -= 1;
    }
}
