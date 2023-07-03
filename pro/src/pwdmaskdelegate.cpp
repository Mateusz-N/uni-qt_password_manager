#include "../headers/pwdmaskdelegate.h"

#include <QPainter>
#include <qapplication.h>
#include <qlineedit.h>

PwdMaskDelegate::PwdMaskDelegate(QChar passwordChar, bool hidePwd, QTreeView *treeView, QObject *parent) : QStyledItemDelegate(parent) {
    m_treeView = treeView;
    m_passwordChar = passwordChar;
    m_passwordIsHidden = hidePwd;
}

QWidget* PwdMaskDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    m_editorIndex = index;
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void PwdMaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    // Pobierz referencję do widgetu, do którego należy wybrany element
    const QWidget *widget = option.widget;

    // Utwórz nowy zestaw parametrów rysowania elementu na podstawie bieżącego
    QStyleOptionViewItem maskOption(option);

    // Wyłącz domyślne style zaznaczenia i najechania na element (zamiast podświetlania tekstu będzie podświetlana cała komórka)
    maskOption.state &= ~QStyle::State_Selected;
    maskOption.state &= ~QStyle::State_MouseOver;

    // Zainicjuj zestaw parametrów dla wybranych danych
    initStyleOption(&maskOption, index);
    painter->setBackground(QBrush(Qt::transparent));

    // Podmień tekst na ciąg m_passwordChar
    if(m_passwordIsHidden) maskOption.text = QString(maskOption.text.length(), m_passwordChar);

    // Rysuj element z nowymi parametrami
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &maskOption, painter, widget);

    // Niestandardowe rysowanie zaznaczonego elementu (zaznaczenie całej pojedynczej komórki)
    if (option.state & QStyle::State_Selected) {
        paintHighlight(painter, option, 64);
    }

    // Niestandardowe rysowanie najechanego elementu (zaznaczenie całej pojedynczej komórki)
    else if (option.state & QStyle::State_MouseOver) {
        paintHighlight(painter, option, 32);
    }

    // Sprawdź, czy otwarty jest edytor
    QWidget *activeEditor = qobject_cast<QWidget*>(m_treeView->indexWidget(m_editorIndex));
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(activeEditor);

    // Jeśli tak, zaktualizuj jego styl
    if(lineEdit) {
        lineEdit->setEchoMode(m_passwordIsHidden ? QLineEdit::Password : QLineEdit::Normal);
    }
}

void PwdMaskDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
    // Pobierz referencję do edytora typu QLineEdit
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);

    // Dopasuj rozmiar edytora
    if(lineEdit) {
        lineEdit->setGeometry(option.rect);
    }
}

void PwdMaskDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    // Pobierz referencję do edytora typu QLineEdit
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);

    // Odczytaj edytowane hasło
    QString pwd = index.model()->data(index).toString();

    // Podmień tekst na ciąg m_passwordChar
    if(lineEdit) {
        lineEdit->setText(pwd);
    }
}

void PwdMaskDelegate::paintHighlight(QPainter *painter, QStyleOptionViewItem option, int alpha) const {
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

bool PwdMaskDelegate::isPasswordHidden() {
    return m_passwordIsHidden;
}

void PwdMaskDelegate::setPasswordHidden(bool hidden) {
    m_passwordIsHidden = hidden;
}
