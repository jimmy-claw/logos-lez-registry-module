import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * LezRegistryView — main registry browser panel.
 *
 * Expects the following context properties from C++:
 *   - lezRegistryModel  : ProgramListModel*
 *   - lezRegistryModule : LezRegistryModule*
 */
Item {
    id: root

    // ── State ─────────────────────────────────────────────────────────────────
    property int    currentView: 0    // 0=list, 1=detail, 2=register
    property var    selectedProgram: null

    // IDL fetch state
    property string idlContent:  ""
    property bool   idlLoading:  false
    property string idlError:    ""

    // ── Root background ────────────────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: Theme.palette.background
    }

    // ── Header ─────────────────────────────────────────────────────────────────
    Rectangle {
        id: header
        anchors { left: parent.left; right: parent.right; top: parent.top }
        height: 56
        color: Theme.palette.backgroundSecondary
        border { color: Theme.palette.borderSecondary; width: 0 }

        RowLayout {
            anchors { fill: parent; leftMargin: Theme.spacing.large; rightMargin: Theme.spacing.large }
            spacing: 10

            // Logo / Title
            Text {
                text: "⬡ LEZ Registry"
                color: Theme.palette.primary
                font { pixelSize: 18; bold: true; family: "monospace" }
            }

            // Live count badge
            Rectangle {
                visible: lezRegistryModel.count > 0
                width:  countText.implicitWidth + Theme.spacing.medium
                height: 20
                radius: 10
                color: Theme.palette.backgroundSecondary

                Text {
                    id: countText
                    anchors.centerIn: parent
                    text: lezRegistryModel.count + " programs"
                    color: Theme.palette.textSecondary
                    font.pixelSize: 11
                }
            }

            Item { Layout.fillWidth: true }

            // Loading indicator
            BusyIndicator {
                running: lezRegistryModel.loading
                width:   20; height: 20
                visible: lezRegistryModel.loading
            }

            // Refresh button
            Button {
                text: "↺"
                visible: root.currentView === 0
                enabled: !lezRegistryModel.loading
                onClicked: lezRegistryModel.refresh()

                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? Theme.palette.primary : Theme.palette.textTertiary
                    font.pixelSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: parent.hovered ? Theme.palette.backgroundSecondary : "transparent"
                    radius: Theme.spacing.tiny
                }
                width: 32; height: 32
            }

            // Register button
            Button {
                text: "+ Register"
                visible: root.currentView === 0
                onClicked: root.currentView = 2

                contentItem: Text {
                    text: parent.text
                    color: Theme.palette.primary
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                }
                background: Rectangle {
                    color:  parent.hovered ? Theme.palette.backgroundSecondary : "transparent"
                    radius: Theme.spacing.tiny
                    border { color: Theme.palette.primary; width: 1 }
                }
                height: 30
            }
        }
    }

    // ── Body (stack of views) ──────────────────────────────────────────────────
    Item {
        anchors {
            left:   parent.left
            right:  parent.right
            top:    header.bottom
            bottom: parent.bottom
            margins: 0
        }

        // ── VIEW 0: Program List ──────────────────────────────────────────────
        Item {
            anchors.fill: parent
            visible: root.currentView === 0

            // Error banner
            Rectangle {
                id: errorBanner
                anchors { left: parent.left; right: parent.right; top: parent.top }
                height: errorBanner.visible ? errorText.implicitHeight + Theme.spacing.large : 0
                visible: lezRegistryModel.error.length > 0
                color: "#2a1111"
                border { color: Theme.palette.error; width: 1 }

                Text {
                    id: errorText
                    anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; margins: 10 }
                    text: "⚠ " + lezRegistryModel.error
                    color: Theme.palette.error
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                }
            }

            // Empty state
            Column {
                anchors.centerIn: parent
                spacing: Theme.spacing.medium
                visible: !lezRegistryModel.loading && lezRegistryModel.count === 0 && lezRegistryModel.error.length === 0

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "⬡"
                    color: Theme.palette.textTertiary
                    font.pixelSize: 48
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "No programs registered"
                    color: Theme.palette.textTertiary
                    font.pixelSize: 15
                }
                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Refresh"
                    onClicked: lezRegistryModel.refresh()
                    contentItem: Text {
                        text: parent.text
                        color: Theme.palette.primary
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                    }
                    background: Rectangle {
                        color: parent.hovered ? Theme.palette.backgroundSecondary : "transparent"
                        radius: Theme.spacing.tiny
                        border { color: Theme.palette.primary; width: 1 }
                    }
                }
            }

            ListView {
                id: programList
                anchors {
                    left:   parent.left
                    right:  parent.right
                    top:    errorBanner.bottom
                    bottom: parent.bottom
                    margins: Theme.spacing.medium
                }
                spacing: Theme.spacing.small
                clip: true
                model: lezRegistryModel

                ScrollBar.vertical: ScrollBar {}

                delegate: ProgramCard {
                    width: programList.width

                    programName:    model.name        || ""
                    programVersion: model.version     || ""
                    programAuthor:  model.author      || ""
                    programDesc:    model.description || ""
                    programId:      model.programId   || ""
                    idlCid:         model.idlCid      || ""

                    onClicked: {
                        root.selectedProgram = {
                            name:        model.name        || "",
                            version:     model.version     || "",
                            author:      model.author      || "",
                            description: model.description || "",
                            programId:   model.programId   || "",
                            idlCid:      model.idlCid      || ""
                        }
                        root.idlContent = ""
                        root.idlError   = ""
                        root.currentView = 1
                    }
                }
            }
        }

        // ── VIEW 1: Program Detail ────────────────────────────────────────────
        ProgramDetail {
            anchors.fill: parent
            visible: root.currentView === 1

            programName:    root.selectedProgram ? root.selectedProgram.name        : ""
            programVersion: root.selectedProgram ? root.selectedProgram.version     : ""
            programAuthor:  root.selectedProgram ? root.selectedProgram.author      : ""
            programDesc:    root.selectedProgram ? root.selectedProgram.description : ""
            programId:      root.selectedProgram ? root.selectedProgram.programId   : ""
            idlCid:         root.selectedProgram ? root.selectedProgram.idlCid      : ""
            idlContent:     root.idlContent
            idlLoading:     root.idlLoading
            idlError:       root.idlError

            onBackRequested: {
                root.currentView = 0
                root.idlContent  = ""
                root.idlError    = ""
            }

            onFetchIdlRequested: function(cid) {
                root.idlLoading = true
                root.idlError   = ""
                root.idlContent = ""
                lezRegistryModule.fetchIdlAsync(cid)
            }
        }

        // ── VIEW 2: Register Form ─────────────────────────────────────────────
        RegisterForm {
            anchors.fill: parent
            visible: root.currentView === 2
            submitting: false

            onCancelled: root.currentView = 0

            onSubmitRequested: function(argsJson) {
                submitting = true
                submitError  = ""
                submitResult = ""
                lezRegistryModule.registerAsync(argsJson)
            }
        }
    }

    // ── Connections from module signals ───────────────────────────────────────
    Connections {
        target: lezRegistryModule

        function onIdlFetched(content, error) {
            root.idlLoading = false
            if (error.length > 0) {
                root.idlError   = error
                root.idlContent = ""
            } else {
                root.idlContent = content
                root.idlError   = ""
            }
        }

        function onProgramRegistered() {
            // Return to list and refresh
            root.currentView = 0
            lezRegistryModel.refresh()
        }
    }

    // ── Auto-load on startup ──────────────────────────────────────────────────
    Component.onCompleted: {
        lezRegistryModel.refresh()
    }
}
