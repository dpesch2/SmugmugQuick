function fillListViewModel(listViewModel, data, itemFunc ) {
    listViewModel.clear()
    for (var i = 0; i < data.length; i++) {
        console.log("list view update " + data[i])
        mainForm.listView.model.append(itemFunc(data[i]))
    }
}

function albumToListItem(album) {
    return {"id": album.id,
            "title": album.title,
            "thumbImage": album.thumbImage,
            "imageCount": album.imageCount,
            "category": album.category,
            "subcategory": album.subcategory
    }
}

function updateThumbImage(listViewModel, id, filename) {
    for (var i = 0; i < listViewModel.count; i++) {
        if( listViewModel.get(i).id == id ) {
            console.log("updateThumbImage " + i + " id=" + id)
            listViewModel.setProperty(i, "thumbImage", "file://" + filename )
        }
    }

}

function updateListViewModelItemProperty(listViewModel, id, propName, value, func) {
    for (var i = 0; i < listViewModel.count; i++) {
        if( listViewModel.get(i).id === id ) {
            listViewModel.setProperty(i, propName, func === undefined ? value : func(value) )
        }
    }
}

