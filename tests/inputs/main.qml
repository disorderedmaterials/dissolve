import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Rectangle {
    visible: true;
    anchors.fill: parent;

    StackView {
	id: stack;
	initialItem: pickForcefield;
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: parent.right;
	anchors.bottom: nextButton.top;
    }

    ListModel {
	id: pictures;

	ListElement { imagePath: "1.jpg"; imageName: "flower" }
	ListElement { imagePath: "2.jpg"; imageName: "house" }
	ListElement { imagePath: "3.jpg"; imageName: "water" }

    }

    Component {
	id: pickForcefield

	Item {
	    Text {
		anchors.top: parent.top
		anchors.right: ffFilter.left;
		text: "Filter";
	    }
	    TextField {
		id: ffFilter;
		focus: true;
		anchors.top: parent.top;
		anchors.right: parent.right;
		placeholderText: qsTr("Filter");
	    }
	    Item {
		anchors.top: ffFilter.bottom;
		anchors.bottom: parent.bottom;
		width: parent.width;
		ListView {
		    width: parent.width;
		    height: parent.height/2;
		    id: ffList;
		    model: pictures;
		    delegate: Text {
			text: imageName;
		    }
		}
	    }
	}
    }

    Component {
	id: dummy

	Rectangle {
	    color: "red";
	    anchors.fill: parent;
	}
    }

    Button {
	text: "Cancel";
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
    }
    Button {
	id: nextButton;
	text: "Next";
	anchors.bottom: parent.bottom;
	anchors.right: parent.right;
	onClicked: stack.push(dummy);
    }
    Button {
	text: "Back";
	anchors.bottom: parent.bottom;
	anchors.right: nextButton.left;
	enabled: stack.depth > 1;
	onClicked: stack.pop();
    }
}
