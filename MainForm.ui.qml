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
        x: 21
        y: 86
        width: 606
        height: 386
        model: ListModel {
        }
        delegate: Item {
            id: item
            x: 5
            width: 80
            height:80

            Row {
                id: row1

                Image {
                    id: imageItem
                    width: 75
                    height: 75
                    asynchronous: true
                    source: album.thumbImage == "" ? "images/album.png" : album.thumbImage
                }

                Column {
                    Row {

                        Text {
                            id: titleItem
                            text: album.title
                            font.bold: true
                        }
                        spacing: 20
                    }

                    Row {
                        Text {
                            id: categoryItem
                            text: album.category
                        }
                        Text {
                            id: subcategoryItem
                            text: album.subcategory
                        }
                        Text {
                            id: imageCountItem
                            text: album.imageCount == -1 ? "?" : album.imageCount
                        }
                        //layoutDirection: Qt.RightToLeft
                        spacing: 20
                    }

                    spacing: 2
                }
                spacing: 10
            }
        }
    }

}
