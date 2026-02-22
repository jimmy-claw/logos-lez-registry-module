import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * ProgramDetail — full metadata view for a selected registry program.
 * Shows all fields and provides a "Fetch IDL" button.
 */
Item {
    id: root

    // ── Data ─────────────────────────────────────────────────────────────────
    property string programName:    ""
    property string programVersion: ""
    property string programAuthor:  ""
    property string programDesc:    ""
    property string programId:      ""
    property string idlCid:         ""
    property string idlContent:     ""   // populated after "Fetch IDL"
    property bool   idlLoading:     false
    property string idlError:       ""

    // ── Signals ───────────────────────────────────────────────────────────────
    signal backRequested()
    signal fetchIdlRequested(string cid)

    // ── Layout ────────────────────────────────────────────────────────────────
    ColumnLayout {
        anchors { fill: parent; margins: 16 }
        spacing: 12

        // ── Toolbar ──────────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "← Back"
                flat: true
                onClicked: root.backRequested()
                contentItem: Text {
                    text: parent.text
                    color: "#7c6af5"
                    font.pixelSize: 13
                }
                background: Rectangle { color: "transparent" }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "Program Detail"
                color: "#888aaa"
                font.pixelSize: 13
            }
        }

        // ── Divider ───────────────────────────────────────────────────────────
        Rectangle { height: 1; color: "#3a3a4a"; Layout.fillWidth: true }

        // ── Scroll area for metadata ──────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 10

                // Name + version
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: root.programName || "(unnamed)"
                        color: "#e0e0ff"
                        font { pixelSize: 22; bold: true; family: "monospace" }
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Rectangle {
                        width:  versionText.implicitWidth + 12
                        height: 22
                        radius: 4
                        color:  "#2a2a3a"
                        border { color: "#5a5a7a"; width: 1 }
                        Text {
                            id: versionText
                            anchors.centerIn: parent
                            text: "v" + (root.programVersion || "?")
                            color: "#aaaacc"
                            font.pixelSize: 12
                        }
                    }
                }

                // Field helper component
                component MetaField : ColumnLayout {
                    property string label: ""
                    property string value: ""
                    property bool   mono:  false

                    visible: value.length > 0
                    spacing: 2
                    Layout.fillWidth: true

                    Text {
                        text: label
                        color: "#666688"
                        font { pixelSize: 11; capitalization: Font.AllUppercase }
                        letterSpacing: 0.8
                    }

                    TextEdit {
                        readOnly:   true
                        selectByMouse: true
                        text:       value
                        color:      "#ccccee"
                        font { pixelSize: 13; family: mono ? "monospace" : "" }
                        wrapMode:   TextEdit.Wrap
                        Layout.fillWidth: true
                        background: null
                    }
                }

                MetaField { label: "Author";     value: root.programAuthor;  mono: true }
                MetaField { label: "Program ID"; value: root.programId;      mono: true }
                MetaField { label: "IDL CID";    value: root.idlCid;         mono: true }
                MetaField { label: "Description"; value: root.programDesc }

                // ── Fetch IDL button ─────────────────────────────────────────
                RowLayout {
                    Layout.fillWidth: true
                    visible: root.idlCid.length > 0
                    spacing: 8

                    Button {
                        text: root.idlLoading ? "Fetching…" : "Fetch IDL"
                        enabled: !root.idlLoading && root.idlCid.length > 0
                        onClicked: root.fetchIdlRequested(root.idlCid)

                        contentItem: Text {
                            text: parent.text
                            color: parent.enabled ? "#7c6af5" : "#555566"
                            font.pixelSize: 13
                            horizontalAlignment: Text.AlignHCenter
                        }

                        background: Rectangle {
                            color:  parent.pressed ? "#3a3a5a" : (parent.hovered ? "#2a2a4a" : "#1e1e3a")
                            radius: 4
                            border { color: "#7c6af5"; width: 1 }
                        }
                    }

                    BusyIndicator {
                        running: root.idlLoading
                        width:   20; height: 20
                        visible: root.idlLoading
                    }
                }

                // IDL error
                Text {
                    visible: root.idlError.length > 0
                    text:    "Error: " + root.idlError
                    color:   "#ff6666"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                // IDL content viewer
                Rectangle {
                    visible: root.idlContent.length > 0
                    Layout.fillWidth: true
                    height: Math.min(idlText.implicitHeight + 16, 300)
                    color: "#111122"
                    radius: 4
                    border { color: "#3a3a5a"; width: 1 }
                    clip: true

                    ScrollView {
                        anchors { fill: parent; margins: 8 }
                        TextEdit {
                            id: idlText
                            readOnly:      true
                            selectByMouse: true
                            text:          root.idlContent
                            color:         "#aaffaa"
                            font { pixelSize: 11; family: "monospace" }
                            wrapMode:      TextEdit.NoWrap
                            background:    null
                        }
                    }
                }
            }
        }
    }
}
