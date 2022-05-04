import QtQuick 2
import QtQuick.Controls 2
import DataManagerSimulationModel 1.0
import SortFilterProxyModel 1.0
import Qt.labs.platform


GroupBox {
    id: databox

    required property DataManagerSimulationModel model;

    title: "Available Data"

    SortFilterProxyModel {
	id: sims
	source: model
	filterString: filterEdit.text
    }

    Image {
	anchors.top: parent.top
	anchors.right: filterEdit.left
	anchors.bottom: filterEdit.bottom
	width: height
	source: "qrc:/general/icons/general_filter.svg"
    }
    TextField {
	id: filterEdit
	anchors.top: parent.top
	anchors.right: parent.right
	ToolTip.text: "Filter templates by name / description"
	ToolTip.visible: hovered
    }
    HorizontalHeaderView {
	id: simTableHeader
	syncView: simTable
	anchors.top: filterEdit.bottom
	anchors.left: simTable.left
    }
    TableView {
	id: simTable
	anchors.top: simTableHeader.bottom
	anchors.left: parent.left
	anchors.right: parent.right
	anchors.bottom: parent.bottom
	clip: true
	model: sims
	delegate: Rectangle{
	    implicitWidth: simLabel.width + 5
	    implicitHeight: simLabel.height + 5
	    border.width: 1;
	    color: palette.base
	    Label {
		id: simLabel
		text: display
	    }
	}
    }
}
