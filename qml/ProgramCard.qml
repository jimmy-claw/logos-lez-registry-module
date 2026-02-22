import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * ProgramCard — delegate component for a single registry program entry.
 * Shows name, version, author snippet, and description preview.
 */
Rectangle {
    id: root

    // ── Data ─────────────────────────────────────────────────────────────────
    property string programName:    ""
    property string programVersion: ""
    property string programAuthor:  ""
    property string programDesc:    ""
    property string programId:      ""
    property string idlCid:         ""

    // ── Signals ───────────────────────────────────────────────────────────────
    signal clicked()

    // ── Layout ────────────────────────────────────────────────────────────────
    height:  cardColumn.implicitHeight + 24
    radius:  Theme.spacing.radiusLarge
    color:   mouseArea.containsMouse ? Theme.palette.backgroundSecondary : Theme.palette.backgroundTertiary

    border {
        color: mouseArea.containsMouse ? Theme.palette.primary : Theme.palette.borderSecondary
        width: 1
    }

    Behavior on color { ColorAnimation { duration: 120 } }
    Behavior on border.color { ColorAnimation { duration: 120 } }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }

    ColumnLayout {
        id: cardColumn
        anchors {
            left:   parent.left
            right:  parent.right
            top:    parent.top
            margins: Theme.spacing.medium
        }
        spacing: Theme.spacing.tiny

        // Name + Version row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacing.small

            Text {
                text: root.programName || "(unnamed)"
                color: Theme.palette.text
                font { pixelSize: 15; bold: true; family: "monospace" }
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Text {
                text: "v" + (root.programVersion || "?")
                color: Theme.palette.textSecondary
                font.pixelSize: 12
            }
        }

        // Author
        Text {
            visible: root.programAuthor.length > 0
            text: "by " + (root.programAuthor.length > 16
                           ? root.programAuthor.substring(0, 8) + "…" + root.programAuthor.slice(-6)
                           : root.programAuthor)
            color: Theme.palette.primary
            font.pixelSize: 11
        }

        // Description snippet
        Text {
            visible: root.programDesc.length > 0
            text: root.programDesc.length > 120
                  ? root.programDesc.substring(0, 120) + "…"
                  : root.programDesc
            color: Theme.palette.textSecondary
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // Tags / IDL indicator
        RowLayout {
            spacing: 6
            visible: root.idlCid.length > 0

            Rectangle {
                width: idlLabel.implicitWidth + 10
                height: 18
                radius: Theme.spacing.tiny
                color: "#1a2e1a"
                border { color: Theme.palette.success; width: 1 }
                Text {
                    id: idlLabel
                    anchors.centerIn: parent
                    text: "IDL"
                    color: Theme.palette.success
                    font.pixelSize: 10
                }
            }
        }
    }
}
