import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

MenuItem {
    id: root

    required property var canvasDimensions
    readonly property double headerHeight: 20
    required property string key
    readonly property Popup plotPopup: Popup {
        id: plotContainer

        readonly property var canvasDimensions: root.canvasDimensions
        readonly property double headerHeight: root.headerHeight
        readonly property bool interacting: root.plotView.mouseArea.containsMouse

        contentItem: root.plotView
        height: 1.5 * canvasDimensions["dy"]
        width: 1.5 * canvasDimensions["dx"]

        onInteractingChanged: if (!interacting && !root.hovered) {
            close();
        }
    }
    required property Item plotView
    required property var renderableData

    hoverEnabled: true
    text: key

    onHoveredChanged: {
        if (hovered) {
            const p = parent.mapToItem(parent, parent.width, 0);
            plotPopup.x = p.x;
            plotPopup.y = p.y;
            plotPopup.open();
        } else if (!plotPopup.interacting)
            plotPopup.close();
    }
}
