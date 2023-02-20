import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ColumnLayout {
    property variant dialogModel;
    ListView {
	id: atList;
	width: 100;
	height: 100;
	/* anchors.fill: parent; */
	model: dialogModel.atomTypes;
	delegate: Text {
	    property variant fullData: model;
	    text: display;
	    color: ListView.isCurrentItem ? "red" : "black";
	    MouseArea {
		anchors.fill: parent;
		onClicked: atList.currentIndex = index;
	    }
	}
    }
}
