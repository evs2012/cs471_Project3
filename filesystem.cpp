#include "filesystem.h"
#include "ui_filesystem.h"

#include <QStringListModel>
#include <QList>
#include "i_node.h"
#include <QMessageBox>

QStringList * driveA_Strings;
QStringListModel * driveA_Model;
QStringList * driveB_Strings;
QStringListModel * driveB_Model;
QStringList * driveC_Strings;
QStringListModel * driveC_Model;
QStringList * UserDisplayStrings;
QStringListModel * UserDisplayModel;

QList<i_node> itemsInDriveA;
QList<i_node> itemsInDriveB;
QList<i_node> itemsInDriveC;
QList<i_node> flattenedDriveList;

FileSystem::FileSystem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileSystem)
{
    ui->setupUi(this);

    //link lists to models and models to list views
    driveA_Strings = new QStringList();
    driveB_Strings = new QStringList();
    driveC_Strings = new QStringList();
    UserDisplayStrings = new QStringList();

    driveA_Model = new QStringListModel(*driveA_Strings, NULL);
    driveB_Model = new QStringListModel(*driveB_Strings, NULL);
    driveC_Model = new QStringListModel(*driveC_Strings, NULL);
    UserDisplayModel = new QStringListModel(*UserDisplayStrings, NULL);

    ui->lstDriveA->setModel(driveA_Model);
    ui->lstDriveB->setModel(driveB_Model);
    ui->lstDriveC->setModel(driveC_Model);
    ui->lstUserDisplay->setModel(UserDisplayModel);


    //initialize the data from the file? or something
    itemsInDriveA.append(i_node("Folder1","/","no data",true));
    itemsInDriveA.append(i_node("Folder2","/","no data",true));
    itemsInDriveB.append(i_node("Folder3","/","no data",true));
    itemsInDriveB.append(i_node("Folder4","/","no data",true));
    itemsInDriveC.append(i_node("Folder5","/","no data",true));
    itemsInDriveC.append(i_node("Folder6","/","no data",true));

    itemsInDriveA[0].folderContents.append(i_node("FileA","txt","this is file A",false));
    itemsInDriveA[0].folderContents.append(i_node("Folder7","/","no data",true));
    itemsInDriveA[0].folderContents[1].folderContents.append(i_node("FileC","txt","this is file C",false));
    itemsInDriveA[0].folderContents.append(i_node("FileB","txt","this is file B",false));
    itemsInDriveA[1].folderContents.append(i_node("FileD","txt","this is file D",false));

    itemsInDriveB[0].folderContents.append(i_node("FileE","txt","this is file E",false));
    itemsInDriveB[0].folderContents.append(i_node("FileF","txt","this is file F",false));
    itemsInDriveB[1].folderContents.append(i_node("Folder8","/","no data",true));
    itemsInDriveB[1].folderContents[0].folderContents.append(i_node("FileG","txt","this is file G",false));
    itemsInDriveB[1].folderContents[0].folderContents.append(i_node("FileH","txt","this is file H",false));

    itemsInDriveC[0].folderContents.append(i_node("FileI","txt","this is file I",false));
    itemsInDriveC[0].folderContents.append(i_node("FileJ","txt","this is file J",false));
    itemsInDriveC[0].folderContents.append(i_node("FileK","txt","this is file K",false));
    itemsInDriveC[1].folderContents.append(i_node("Folder9","/","no data",true));
    itemsInDriveC[1].folderContents.append(i_node("Folder10","/","no data",true));
    itemsInDriveC[1].folderContents[0].folderContents.append(i_node("FileL","txt","this is file L",false));
    itemsInDriveC[1].folderContents[1].folderContents.append(i_node("FileM","txt","this is file M",false));



    UpdateLists();

}

FileSystem::~FileSystem()
{
    delete ui;
}

void FileSystem::UpdateLists()
{
    driveA_Strings->clear();
    driveB_Strings->clear();
    driveC_Strings->clear();
    UserDisplayStrings->clear();

    foreach (i_node n, itemsInDriveA) {
        AddItem(driveA_Strings, n, 1);
    }
    foreach (i_node n, itemsInDriveB) {
        AddItem(driveB_Strings, n, 1);
    }
    foreach (i_node n, itemsInDriveC) {
        AddItem(driveC_Strings, n, 1);
    }

    //Figure how to append everything to the user display

    driveA_Model->setStringList(*driveA_Strings);
    driveB_Model->setStringList(*driveB_Strings);
    driveC_Model->setStringList(*driveC_Strings);
    UserDisplayModel->setStringList(*UserDisplayStrings);
}

void FileSystem::AddItem(QStringList *&list, i_node &item, int level)
{
    QString Prefix = "";
    //level 1 is top
    switch (level) {
    case 2:
        Prefix = " |-> ";
        break;
    case 3:
        Prefix = " |   |-> ";
        break;
    case 4:
        Prefix = " |   |   |-> ";
        break;
    default:
        break;
    }
    list->append(Prefix + item.display());
    UserDisplayStrings->append(Prefix + item.display());
    flattenedDriveList.append(item);

    if (item.isFolder)
    {
        foreach (i_node n, item.folderContents) {
            AddItem(list, n, level+1);
        }
    }
}

void FileSystem::on_btnAddFile_clicked()
{
    if (ui->newFileName->text() != "" && ui->newFileExtension->text() != "" && ui->newFileData->text() != "")
    {
        selectedItem(true);
        ui->newFileName->setText("");
        ui->newFileExtension->setText("");
        ui->newFileData->setText("");
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Needs filename, extension, and unique data.");
        msgBox.exec();

    }
    UpdateLists();
}

void FileSystem::on_btnDeleteFile_clicked()
{
    selectedItem(false);
    UpdateLists();
}

//if add item is false the selected item will be deleted
void FileSystem::selectedItem(bool addItem)
{
    QMessageBox msgBox;
    //go through the 3 lists and their sublists looking for the file, when found hop to parent list and remove it.
    QModelIndexList ListOfItemsOnForm = ui->lstUserDisplay->selectionModel()->selectedIndexes();

    bool inA = false;
    bool inB = false;
    bool inC = false;

    int n, m, l;


    foreach (const QModelIndex &selectedIndex, ListOfItemsOnForm)
    {
        // check it against the items in blocked list, if found move it to ready list
        /// Drive A
        n = 0;
        while(n < itemsInDriveA.size())
        {
            //top level
            if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveA[n].filename))
            {
                if (itemsInDriveA[n].isFolder)
                {
                    if (addItem)
                    {
                        //need to add the item to this folder
                        itemsInDriveA[n].folderContents.append(i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                    }
                    else
                    {
                        msgBox.setText("Cannot delete directory.");
                        msgBox.exec();
                    }
                }
                else
                {
                    //this is the item, remove it and break
                    inA = true;

                    if (addItem)
                    {
                        //a file was clicked so just add underneath it
                        itemsInDriveA.insert(n+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                    }
                    else
                    {
                        itemsInDriveA.removeAt(n);
                    }
                    break;
                }
            }
            //second level
            if (itemsInDriveA[n].isFolder)
            {
                m = 0;
                while(m < itemsInDriveA[n].folderContents.size())
                {
                    if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveA[n].folderContents[m].filename))
                    {
                        if (itemsInDriveA[n].folderContents[m].isFolder)
                        {
                            if (addItem)
                            {
                                //need to add the item to this folder
                                itemsInDriveA[n].folderContents[m].folderContents.append(i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                            }
                            else
                            {
                                msgBox.setText("Cannot delete directory.");
                                msgBox.exec();
                            }
                        }
                        else
                        {
                            //this is the item, remove it and break
                            inA = true;

                            if (addItem)
                            {
                                //a file was clicked so just add underneath it
                                itemsInDriveA[n].folderContents.insert(m+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                            }
                            else
                            {
                                itemsInDriveA[n].folderContents.removeAt(m);
                            }
                            break;
                        }
                    }
                    // third level
                    if (itemsInDriveA[n].folderContents[m].isFolder)
                    {
                        l = 0;
                        while(l < itemsInDriveA[n].folderContents[m].folderContents.size())
                        {
                            if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveA[n].folderContents[m].folderContents[l].filename))
                            {
                                //third level will not be a folder
                                    //this is the item, remove it and break
                                    inA = true;

                                    if (addItem)
                                    {
                                        //a file was clicked so just add underneath it
                                        itemsInDriveA[n].folderContents[m].folderContents.insert(l+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                                    }
                                    else
                                    {
                                        itemsInDriveA[n].folderContents[m].folderContents.removeAt(l);
                                    }
                                    break;
                            }
                            l++;
                        }
                    }
                    m++;
                }
            }
            n++;
        }
        if (inA)
        {
            break;
        }

        ///Drive B
        n = 0;
        while(n < itemsInDriveB.size())
        {
            //top level
            if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveB[n].filename))
            {
                if (itemsInDriveB[n].isFolder)
                {
                    if (addItem)
                    {
                        //need to add the item to this folder
                        itemsInDriveB[n].folderContents.append(i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                    }
                    else
                    {
                        msgBox.setText("Cannot delete directory.");
                        msgBox.exec();
                    }
                }
                else
                {
                    //this is the item, remove it and break
                    inB = true;

                    if (addItem)
                    {
                        //a file was clicked so just add underneath it
                        itemsInDriveB.insert(n+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                    }
                    else
                    {
                        itemsInDriveB.removeAt(n);
                    }
                    break;
                }
            }
            //second level
            if (itemsInDriveB[n].isFolder)
            {
                m = 0;
                while(m < itemsInDriveB[n].folderContents.size())
                {
                    if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveB[n].folderContents[m].filename))
                    {
                        if (itemsInDriveB[n].folderContents[m].isFolder)
                        {
                            if (addItem)
                            {
                                //need to add the item to this folder
                                itemsInDriveB[n].folderContents[m].folderContents.append(i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                            }
                            else
                            {
                                msgBox.setText("Cannot delete directory.");
                                msgBox.exec();
                            }
                        }
                        else
                        {
                            //this is the item, remove it and break
                            inB = true;

                            if (addItem)
                            {
                                //a file was clicked so just add underneath it
                                itemsInDriveB[n].folderContents.insert(m+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                            }
                            else
                            {
                                itemsInDriveB[n].folderContents.removeAt(m);
                            }
                            break;
                        }
                    }
                    // third level
                    if (itemsInDriveB[n].folderContents[m].isFolder)
                    {
                        l = 0;
                        while(l < itemsInDriveB[n].folderContents[m].folderContents.size())
                        {
                            if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveB[n].folderContents[m].folderContents[l].filename))
                            {
                                //third level will not be a folder
                                    //this is the item, remove it and break
                                    inB = true;

                                    if (addItem)
                                    {
                                        //a file was clicked so just add underneath it
                                        itemsInDriveB[n].folderContents[m].folderContents.insert(l+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                                    }
                                    else
                                    {
                                        itemsInDriveB[n].folderContents[m].folderContents.removeAt(l);
                                    }
                                    break;
                            }
                            l++;
                        }
                    }
                    m++;
                }
            }
            n++;
        }
        if (inB)
        {
            break;
        }

        ///Drive C
        n = 0;
        while(n < itemsInDriveC.size())
        {
            //top level
            if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveC[n].filename))
            {
                if (itemsInDriveC[n].isFolder)
                {
                    if (addItem)
                    {
                        //need to add the item to this folder
                        itemsInDriveC[n].folderContents.append(i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                    }
                    else
                    {
                        msgBox.setText("Cannot delete directory.");
                        msgBox.exec();
                    }
                }
                else
                {
                    //this is the item, remove it and break
                    inC = true;

                    if (addItem)
                    {
                        //a file was clicked so just add underneath it
                        itemsInDriveC.insert(n+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                    }
                    else
                    {
                        itemsInDriveC.removeAt(n);
                    }
                    break;
                }
            }
            //second level
            if (itemsInDriveC[n].isFolder)
            {
                m = 0;
                while(m < itemsInDriveC[n].folderContents.size())
                {
                    if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveC[n].folderContents[m].filename))
                    {
                        if (itemsInDriveC[n].folderContents[m].isFolder)
                        {
                            if (addItem)
                            {
                                //need to add the item to this folder
                                itemsInDriveC[n].folderContents[m].folderContents.append(i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                            }
                            else
                            {
                                msgBox.setText("Cannot delete directory.");
                                msgBox.exec();
                            }
                        }
                        else
                        {
                            //this is the item, remove it and break
                            inC = true;

                            if (addItem)
                            {
                                //a file was clicked so just add underneath it
                                itemsInDriveC[n].folderContents.insert(m+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                            }
                            else
                            {
                                itemsInDriveC[n].folderContents.removeAt(m);
                            }
                            break;
                        }
                    }
                    // third level
                    if (itemsInDriveC[n].folderContents[m].isFolder)
                    {
                        l = 0;
                        while(l < itemsInDriveC[n].folderContents[m].folderContents.size())
                        {
                            if (selectedIndex.data(Qt::DisplayRole).toString().contains(itemsInDriveC[n].folderContents[m].folderContents[l].filename))
                            {
                                //third level will not be a folder
                                    //this is the item, remove it and break
                                    inC = true;

                                    if (addItem)
                                    {
                                        //a file was clicked so just add underneath it
                                        itemsInDriveC[n].folderContents[m].folderContents.insert(l+1, i_node(ui->newFileName->text() ,ui->newFileExtension->text(),ui->newFileData->text(),false));
                                    }
                                    else
                                    {
                                        itemsInDriveC[n].folderContents[m].folderContents.removeAt(l);
                                    }
                                    break;
                            }
                            l++;
                        }
                    }
                    m++;
                }
            }
            n++;
        }
        if (inC)
        {
            break;
        }
    }
}

void FileSystem::on_btnPrintData_clicked()
{
    //go through the list to see if I can find the selected file or folder, all folders will be created with the caption no data so treat them the same, the hard part is flattening the list
    // make list of selected items from listview on form: Should only be one, I am not sure how to multi select
    QModelIndexList ListOfItemsOnForm = ui->lstUserDisplay->selectionModel()->selectedIndexes();

    foreach (const QModelIndex &selectedIndex, ListOfItemsOnForm)
    {
        // check it against the items in the flattened list and grab the needed info
        foreach (i_node n, flattenedDriveList)
        {
            if (selectedIndex.data(Qt::DisplayRole).toString().contains(n.filename))
            {
                //this is the item, display it
                ui->lineEdit->setText(n.data);
            }
        }
    }
}
