import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ColumnLayout {
    /* property variant fullData: ffList.currentItem.fullData; */
    Text { text: "Foo"; }
    ListView {
	id: atList;
	/* anchors.fill: parent; */
	model: atModel;
	delegate: Text {
	    property variant fullData: model;
	    text: display;
	    color: ListView.isCurrentItem ? "red" : "black";
	    MouseArea {
		anchors.fill: parent;
		onClicked: ffList.currentIndex = index;
	    }
	}
    }
    Text { text: "Bar"; }
}
