import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * RegisterForm — form to register a new program in the LEZ registry.
 * Exposes an onSubmit signal with the JSON args payload.
 */
Item {
    id: root

    property bool   submitting: false
    property string submitError: ""
    property string submitResult: ""

    signal submitRequested(string argsJson)
    signal cancelled()

    function reset() {
        nameField.text = ""
        versionField.text = ""
        descField.text = ""
        programIdField.text = ""
        idlCidField.text = ""
        walletPathField.text = ""
        submitError  = ""
        submitResult = ""
    }

    ColumnLayout {
        anchors { fill: parent; margins: 16 }
        spacing: 10

        // ── Toolbar ──────────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "Register Program"
                color: "#e0e0ff"
                font { pixelSize: 18; bold: true }
                Layout.fillWidth: true
            }

            Button {
                text: "✕ Cancel"
                flat: true
                onClicked: root.cancelled()
                contentItem: Text {
                    text: parent.text
                    color: "#888aaa"
                    font.pixelSize: 13
                }
                background: Rectangle { color: "transparent" }
            }
        }

        Rectangle { height: 1; color: "#3a3a4a"; Layout.fillWidth: true }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 10

                // ── Field helper ─────────────────────────────────────────────
                component FormField : ColumnLayout {
                    property alias  label:       fieldLabel.text
                    property alias  placeholder: inputField.placeholderText
                    property alias  text:        inputField.text
                    property bool   required:    false

                    spacing: 2
                    Layout.fillWidth: true

                    RowLayout {
                        Text {
                            id: fieldLabel
                            color: "#888aaa"
                            font { pixelSize: 11; capitalization: Font.AllUppercase }
                        }
                        Text {
                            visible: required
                            text: " *"
                            color: "#ff6666"
                            font.pixelSize: 11
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 34
                        color: "#111122"
                        radius: 4
                        border { color: inputField.activeFocus ? "#7c6af5" : "#3a3a4a"; width: 1 }

                        TextField {
                            id: inputField
                            anchors { fill: parent; leftMargin: 8; rightMargin: 8 }
                            background: null
                            color: "#e0e0ff"
                            placeholderTextColor: "#555566"
                            font.pixelSize: 13
                        }
                    }
                }

                FormField {
                    id: nameField
                    label: "Name"
                    placeholder: "e.g. lez-multisig"
                    required: true
                }

                FormField {
                    id: versionField
                    label: "Version"
                    placeholder: "e.g. 0.1.0"
                    required: true
                }

                FormField {
                    id: programIdField
                    label: "Program ID"
                    placeholder: "hex or comma-separated u32s"
                    required: true
                }

                // Description (multi-line)
                ColumnLayout {
                    spacing: 2
                    Layout.fillWidth: true

                    Text {
                        text: "DESCRIPTION"
                        color: "#888aaa"
                        font { pixelSize: 11; capitalization: Font.AllUppercase }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 80
                        color: "#111122"
                        radius: 4
                        border { color: descField.activeFocus ? "#7c6af5" : "#3a3a4a"; width: 1 }

                        ScrollView {
                            anchors { fill: parent; margins: 6 }
                            TextArea {
                                id: descField
                                background: null
                                color: "#e0e0ff"
                                placeholderText: "Short description of the program…"
                                placeholderTextColor: "#555566"
                                font.pixelSize: 13
                                wrapMode: TextArea.Wrap
                            }
                        }
                    }
                }

                FormField {
                    id: idlCidField
                    label: "IDL CID (optional)"
                    placeholder: "bafy..."
                }

                FormField {
                    id: walletPathField
                    label: "Wallet Path"
                    placeholder: "/path/to/wallet.json"
                    required: true
                }

                // ── Result / Error ────────────────────────────────────────────
                Text {
                    visible: root.submitError.length > 0
                    text: "Error: " + root.submitError
                    color: "#ff6666"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Text {
                    visible: root.submitResult.length > 0
                    text: "✓ " + root.submitResult
                    color: "#6ad06a"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                // ── Submit button ─────────────────────────────────────────────
                RowLayout {
                    Layout.fillWidth: true
                    Item { Layout.fillWidth: true }

                    Button {
                        text: root.submitting ? "Registering…" : "Register Program"
                        enabled: !root.submitting
                               && nameField.text.length > 0
                               && versionField.text.length > 0
                               && programIdField.text.length > 0
                               && walletPathField.text.length > 0

                        onClicked: {
                            var args = {
                                name:        nameField.text,
                                version:     versionField.text,
                                program_id:  programIdField.text,
                                description: descField.text,
                                wallet_path: walletPathField.text
                            }
                            if (idlCidField.text.length > 0)
                                args.idl_cid = idlCidField.text
                            root.submitRequested(JSON.stringify(args))
                        }

                        contentItem: Text {
                            text: parent.text
                            color: parent.enabled ? "#ffffff" : "#555566"
                            font { pixelSize: 13; bold: true }
                            horizontalAlignment: Text.AlignHCenter
                        }

                        background: Rectangle {
                            color:  parent.enabled
                                    ? (parent.pressed ? "#5a4ad5" : (parent.hovered ? "#6a5ae5" : "#7c6af5"))
                                    : "#2a2a3a"
                            radius: 4
                        }
                    }
                }
            }
        }
    }
}
