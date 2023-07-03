#include "../headers/customtreeview.h"

#include <QStandardItemModel>
#include <QDragEnterEvent>
#include <QMimeData>
#include <qabstractitemmodel.h>

CustomTreeView::CustomTreeView(QWidget *parent) : QTreeView(parent) {

    // Awaryjny model domyślny (docelowo zostanie użyty inny)
    QStandardItemModel *model = new QStandardItemModel(this);
    setModel(model);
}

void CustomTreeView::dropEvent(QDropEvent *event) {

    // Upewnienie, że zdarzenie ma poprawny format MIME
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {

        // Ustaw akcję na przeniesienie
        event->setDropAction(Qt::MoveAction);
        event->accept();

        // Pobierz indeksy elementów źródłowego i docelowego
        QModelIndexList selectedItemIndexList = selectedIndexes();
        if(selectedItemIndexList.isEmpty()) {
            return; // Nie przesuwaj jeśli nic nie zaznaczono
        }
        QModelIndex sourceIndex = selectedItemIndexList.first();
        QModelIndex targetIndex = indexAt(event->position().toPoint());

        // Zapobież upuszczeniu na inny element (wstawieniu elementu źródłowego jako jego dziecka)
        if (sourceIndex.isValid() && targetIndex.isValid()) {

            // Pobierz numery wierszy źródłowego i docelowego
            int sourceRow = sourceIndex.row();
            int targetRow = targetIndex.row();

            // Z poziomu model() nie można wywołać ->takeRow(), dlatego tworzona jest zmienna pomocnicza
            QStandardItemModel* standardModel = qobject_cast<QStandardItemModel*>(model());

            // Przenieś wiersz (dosłownie, weź zaznaczony i wstaw go w docelowe miejsce)
            QList<QStandardItem*> item = standardModel->takeRow(sourceRow);
            standardModel->insertRow(targetRow, item);

            // Zaktualizuj zaznaczenie
            clearSelection();
        }
    } else {
        event->ignore();
    }
}

// Obowiązkowe redefinicje metod; nic specjalnego się tu nie dzieje
void CustomTreeView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void CustomTreeView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        event->ignore();
    }
}
