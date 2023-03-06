import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Item {
    id: control;
    property variant dialogModel;
    property variant fullData: ffList.currentItem.fullData;
    signal selected();
    GroupBox {
	title: "Select Forcefield";
	anchors.top: parent.top;
	height: parent.height/2;
	width: parent.width;
	TextField {
	    id: ffFilter;
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
	    color: palette.active.base;
	    PrettyListView {
		id: ffList;
		anchors.fill: parent;
		clip: true;
		focus: true;
		model: dialogModel.forcefields;
		onSelected: control.selected();
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
	    color: palette.active.base;
	    TextArea {
		readOnly: true;
		anchors.fill: parent;
		text: ffList.currentItem.fullData.description;
		wrapMode: TextEdit.Wrap;
	    }
	}
    }
    Component.onCompleted: ffList.forceActiveFocus();
}
