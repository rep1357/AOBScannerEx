#pragma once

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

class CComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	explicit CComboBoxDelegate(const QList<QPair<QString, QVariant>> &items, QObject *parent = 0);

	// editing
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

	void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

protected:
	bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

private:
	QList<QPair<QString, QVariant>> m_lItem;
};