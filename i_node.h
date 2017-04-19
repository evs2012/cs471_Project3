#ifndef I_NODE_H
#define I_NODE_H
#include <QString>
#include <QList>

class i_node
{
public:
    i_node(){}
    i_node(QString _name, QString _ext, QString _data, bool _isFolder);
    QString display();

    QString filename;
    QString extension;
    QString data;

    bool isFolder;
    bool markForDeletion;
    QList<i_node> folderContents;

    bool operator== (const i_node &n);

    int getUID();
private:
    int UID;
    static int newUID;
};

#endif // I_NODE_H
