import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Item {
    property variant dialogModel;
    property variant fullData: ffList.currentItem.fullData;
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
	Rectangle {
	    width: parent.width;
	    height: parent.height/2-5;
	    color: "white";
	    ListView {
		id: ffList;
		anchors.fill: parent;
		model: dialogModel.forcefields;
		delegate:
		Text {
		    property variant fullData: model;
		    text: name;
		    color: ListView.isCurrentItem ? "red" : "black";
		    MouseArea {
			anchors.fill: parent;
			onClicked: ffList.currentIndex = index;
		    }
		}
	    }
	}
	Rectangle {
	    width: parent.width;
	    height: parent.height/2-5;
	    anchors.bottom: parent.bottom;
	    color: "white";
	    TextArea {
		anchors.fill: parent;
		text: ffList.currentItem.fullData.description;
		wrapMode: TextEdit.Wrap;
	    }
	}
    }
}
