import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ListView {
    id: control;

    signal selected();

    delegate:
    Text {
	property variant fullData: model;
	text: name;
	color: ListView.isCurrentItem ? palette.active.base : palette.active.text;
	MouseArea {
	    height: parent.height;
	    width: control.width;
	    onClicked: {
		control.currentIndex = index;
		control.focus = true;
	    }
	    onDoubleClicked: {
		control.currentIndex = index;
		selected();
	    }
	}
    }
    highlight:
    Rectangle {
	color: palette.active.highlight;
    }
}
