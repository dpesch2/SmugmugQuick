import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Backend 1.0 // C++ backend
import "util.js" as Util

ApplicationWindow {
    title: qsTr("Hello World")
    width: 640
    height: 480
    visible: true

    id: mainPage

    property string selectedAlbumId: ""
    property string selectedImageId: ""

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    MainForm {
        id: mainForm
        anchors.fill: parent
        loadButton.onClicked: {
            model.username = "from-where-i-stand"
        }
        dumpButton.onClicked: model.dump()

        usernameTextfield.text: model.username

    }

    MessageDialog {
        id: messageDialog
        title: qsTr("Error")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }

    Settings {
        id: settings
     }

    Model {
        id: model

        onAlbumsLoaded: {
            Util.fillListViewModel(mainForm.listView.model, model.albums, function(album) { return {"album": album } })
        }

        onImageLoaded: {
            console.log("onImageLoaded");
        }

        onExtraImageInfoLoaded: {
            console.log("exif loaded");

        }

        onImageSavedToShared: {
            console.log(qsTr("Saving finished to ") + fullPath);
        }

        onError: {
            messageDialog.show(errorMsg);
        }
    }

}
