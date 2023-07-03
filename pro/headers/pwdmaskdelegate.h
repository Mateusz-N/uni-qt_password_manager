#ifndef PWDMASKDELEGATE_H
#define PWDMASKDELEGATE_H

#include <qstyleditemdelegate.h>
#include <qtreeview.h>

/* Delegat nadpisujący domyślnego delegata, służący do zakrywania hasła */
class PwdMaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PwdMaskDelegate(QChar passwordChar, bool hidePwd, QTreeView *treeView, QObject *parent = 0);

    // Pobiera indeks otwartego edytora i ustawia edytor domyślnymi ustawieniami
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    // Zakrywa/odkrywa hasła w trybie wyświetlania i ustawia niestandardowe rysowanie podświetlenia elementu
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // Dopasowuje rozmiar edytora
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // Zakrywa/odkrywa hasła w trybie edycji
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    // Getter stanu ukrycia hasła
    bool isPasswordHidden();

    void setPasswordHidden(bool);

private:
    // Rysuje niestandardowe podświetlenie elementu
    void paintHighlight(QPainter *painter, QStyleOptionViewItem option, int alpha) const;

    mutable QModelIndex m_editorIndex;
    QChar m_passwordChar;
    bool m_passwordIsHidden;
    QTreeView *m_treeView;
};
#endif // PWDMASKDELEGATE_H
