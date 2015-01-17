import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    width: 640
    height: 480

    property alias loadButton: loadButton
    property alias dumpButton: dumpButton
    property alias usernameTextfield: usernameTextfield
    property alias listView : listView1

    RowLayout {
        anchors.centerIn: parent
    }

    Button {
        id: loadButton
        x: 13
        y: 42
        text: qsTr("Load")
    }

    TextField {
        id: usernameTextfield
        x: 13
        y: 17
        width: 619
        height: 19
        placeholderText: qsTr("Text Field")
    }

    Button {
        id: dumpButton
        x: 99
        y: 42
        text: qsTr("Dump")
    }

    ListView {
        id: listView1
        clip: true
        x: 21
        y: 86
        width: 606
        height: 386
        //highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        model: ListModel {
        }
        delegate: AlbumListItem {}

    }

}
