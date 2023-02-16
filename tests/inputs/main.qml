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
			color: ListView.isCurrentItem ? "red" : "black";
			MouseArea {
			    anchors.fill: parent;
			    onClicked: ffList.currentIndex = index;
			}
		    }
		}
		Text {
		    width: parent.width;
		    height: parent.height/2;
		    anchors.top: ffList.bottom;
		    text: pictures.get(ffList.currentIndex).imagePath;
		}
	    }
	}

	Rectangle {
	    color: "#EEEEEE";
	    Text {
		text: pictures.get(ffList.currentIndex).imageName;
		color: "red";
	    }
	}
    }

    ListModel {
	id: pictures;

	ListElement { imagePath: "1.jpg"; imageName: "flower" }
	ListElement { imagePath: "2.jpg"; imageName: "house" }
	ListElement { imagePath: "3.jpg"; imageName: "water" }

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
