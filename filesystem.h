#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QMainWindow>
#include "i_node.h"

namespace Ui {
class FileSystem;
}

class FileSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileSystem(QWidget *parent = 0);
    ~FileSystem();

private slots:
    void on_btnAddFile_clicked();

    void on_btnDeleteFile_clicked();

    void on_btnPrintData_clicked();

private:
    Ui::FileSystem *ui;
    void UpdateLists();
    void AddItem(QStringList *&list, i_node &item, int level);
    void selectedItem(bool addItem);
};

#endif // FILESYSTEM_H
