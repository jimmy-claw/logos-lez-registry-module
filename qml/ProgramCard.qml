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
    radius:  6
    color:   mouseArea.containsMouse ? "#2a2a3a" : "#1e1e2e"

    border {
        color: mouseArea.containsMouse ? "#7c6af5" : "#3a3a4a"
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
            margins: 12
        }
        spacing: 4

        // Name + Version row
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: root.programName || "(unnamed)"
                color: "#e0e0ff"
                font { pixelSize: 15; bold: true; family: "monospace" }
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Text {
                text: "v" + (root.programVersion || "?")
                color: "#888aaa"
                font.pixelSize: 12
            }
        }

        // Author
        Text {
            visible: root.programAuthor.length > 0
            text: "by " + (root.programAuthor.length > 16
                           ? root.programAuthor.substring(0, 8) + "…" + root.programAuthor.slice(-6)
                           : root.programAuthor)
            color: "#7c6af5"
            font.pixelSize: 11
        }

        // Description snippet
        Text {
            visible: root.programDesc.length > 0
            text: root.programDesc.length > 120
                  ? root.programDesc.substring(0, 120) + "…"
                  : root.programDesc
            color: "#aaaacc"
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
                radius: 4
                color: "#2a3a2a"
                border { color: "#4a8a4a"; width: 1 }
                Text {
                    id: idlLabel
                    anchors.centerIn: parent
                    text: "IDL"
                    color: "#6ad06a"
                    font.pixelSize: 10
                }
            }
        }
    }
}
