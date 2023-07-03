#ifndef CUSTOMTREEVIEW_H
#define CUSTOMTREEVIEW_H

#include <qtreeview.h>

class CustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CustomTreeView(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // CUSTOMTREEVIEW_H
