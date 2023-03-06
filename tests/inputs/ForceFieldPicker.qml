import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Item {
    property variant dialogModel;
    property variant fullData: ffList.currentItem.fullData;
    GroupBox {
	title: "Select Forcefield";
	anchors.top: parent.top;
	height: parent.height/2;
	width: parent.width;
	TextField {
	    id: ffFilter;
	    focus: true;
	    anchors.top: parent.top;
	    anchors.right: parent.right;
	    placeholderText: qsTr("Filter");
	    onTextEdited: dialogModel.forcefields.setFilterFixedString(text);

	}
	Image {
	    anchors.top: ffFilter.top;
	    anchors.bottom: ffFilter.bottom;
	    anchors.right: ffFilter.left;
	    source: "qrc:/general/icons/general_filter.svg";
	    fillMode: Image.PreserveAspectFit;
	}
	Rectangle {
	    width: parent.width;
	    anchors.top: ffFilter.bottom;
	    anchors.bottom: parent.bottom;
	    color: "white";
	    ListView {
		id: ffList;
		anchors.fill: parent;
		clip: true;
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
    }
    GroupBox {
	title: "Forcefield Description";
	width: parent.width;
	height: parent.height/2-5;
	anchors.bottom: parent.bottom;
	Rectangle {
	    anchors.fill:parent;
	    color: "white";
	    TextArea {
		anchors.fill: parent;
		text: ffList.currentItem.fullData.description;
		wrapMode: TextEdit.Wrap;
	    }
	}
    }
}
