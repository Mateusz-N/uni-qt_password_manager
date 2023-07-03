#ifndef PLAINTEXTDELEGATE_H
#define PLAINTEXTDELEGATE_H

#include <qpainter.h>
#include <qstyleditemdelegate.h>

class PlainTextDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PlainTextDelegate(QObject *parent = 0);

    // Ustawia niestandardowe rysowanie podświetlenia elementu
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // Dopasowuje rozmiar edytora
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    // Rysuje niestandardowe podświetlenie elementu
    void paintHighlight(QPainter *painter, QStyleOptionViewItem option, int alpha) const;
};

#endif // PLAINTEXTDELEGATE_H
