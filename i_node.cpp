#include "i_node.h"

i_node::i_node(QString _name, QString _ext, QString _data, bool _isFolder)
{
    this->filename = _name;
    this->extension = _ext;
    this->data = _data;
    this->isFolder = _isFolder;
    this->UID = newUID++;
    this->markForDeletion = false;
}

QString i_node::display()
{
    if (this->isFolder)
    {
           return this->filename + this->extension;
    }
    else
    {
        return this->filename + "." + this->extension;
    }
}

int i_node::getUID()
{
    return this->UID;
}

bool i_node::operator== (const i_node &n)
{
    return (n.UID == this->UID);
}

int i_node::newUID = 0;
