import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

MenuItem {
    id: renderableItem

    required property var canvasDimensions
    readonly property double headerHeight: 20
    required property string key
    required property Item plot
    readonly property Popup plotPopup: Popup {
        id: plotContainer

        readonly property var canvasDimensions: renderableItem.canvasDimensions
        readonly property double headerHeight: renderableItem.headerHeight
        readonly property bool hovered: plotContainerHoverHandler.hovered

        contentItem: renderableItem.plot
        height: 1.5 * canvasDimensions["dy"]
        width: 1.5 * canvasDimensions["dx"]

        HoverHandler {
            id: plotContainerHoverHandler

            onHoveredChanged: {
                if (hovered)
                    renderableItem.sharedHoverTimer.stop();
                else
                    renderableItem.sharedHoverTimer.restart();
            }
        }
    }
    required property var renderableData
    readonly property Timer sharedHoverTimer: Timer {
        readonly property Popup popup: renderableItem.plotPopup

        interval: 150
        repeat: false

        onTriggered: {
            const noHover = !renderableItem.hovered && !popup.hovered;
            if (noHover)
                popup.close();
        }
    }

    hoverEnabled: true
    text: key

    onHoveredChanged: {
        if (hovered) {
            renderableItem.sharedHoverTimer.stop();
            const p = renderableItem.mapToItem(renderableItem, renderableItem.width, 0);
            plotPopup.x = p.x;
            plotPopup.y = p.y;
            plotPopup.open();
        } else
            renderableItem.sharedHoverTimer.restart();
    }
}
