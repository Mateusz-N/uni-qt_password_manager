#include "../headers/plaintextdelegate.h"

#include <qapplication.h>
#include <qlineedit.h>

PlainTextDelegate::PlainTextDelegate(QObject *)
{

}

void PlainTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // Pobierz referencję do widgetu, do którego należy wybrany element
    const QWidget *widget = option.widget;

    // Utwórz nowy zestaw parametrów rysowania elementu na podstawie bieżącego
    QStyleOptionViewItem plainTextOption(option);

    // Wyłącz domyślne style zaznaczenia i najechania na element (zamiast podświetlania tekstu będzie podświetlana cała komórka)
    plainTextOption.state &= ~QStyle::State_Selected;
    plainTextOption.state &= ~QStyle::State_MouseOver;

    // Zainicjuj zestaw parametrów dla wybranych danych
    initStyleOption(&plainTextOption, index);
    painter->setBackground(QBrush(Qt::transparent));

    // Rysuj element z nowymi parametrami
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &plainTextOption, painter, widget);

    // Niestandardowe rysowanie zaznaczonego elementu (zaznaczenie całej pojedynczej komórki)
    if (option.state & QStyle::State_Selected) {
        paintHighlight(painter, option, 64);
    }

    // Niestandardowe rysowanie najechanego elementu (zaznaczenie całej pojedynczej komórki)
    else if (option.state & QStyle::State_MouseOver) {
        paintHighlight(painter, option, 32);
    }
}

void PlainTextDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
    // Pobierz referencję do edytora typu QLineEdit
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);

    // Dopasuj rozmiar edytora
    if(lineEdit) {
        lineEdit->setGeometry(option.rect);
    }
}

void PlainTextDelegate::paintHighlight(QPainter *painter, QStyleOptionViewItem option, int alpha) const {
    // Pobierz prostokąt okupowany przez komórkę
    QRect rect = option.rect;

    // Ustaw kolor i przezroczystość zaznaczenia (domyślny Windows 10)
    QColor highlightColor = option.palette.color(QPalette::Highlight);
    highlightColor.setAlpha(alpha);

    // Tło
    painter->setBrush(highlightColor);

    // Obramowanie (brak)
    painter->setPen(Qt::NoPen);

    // Zastosowanie zmian
    painter->drawRect(rect);
}
