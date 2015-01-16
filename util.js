function fillListViewModel(listViewModel, data, itemFunc ) {
    listViewModel.clear()
    for (var i = 0; i < data.length; i++) {
        console.log("list view update " + data[i])
        mainForm.listView.model.append(itemFunc(data[i]))
    }
}
