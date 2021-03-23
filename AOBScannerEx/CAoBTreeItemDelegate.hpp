#pragma once

QT_BEGIN_NAMESPACE
class CComboBoxDelegate;
class CIntDelegate;
class QEvent;
QT_END_NAMESPACE

class CAoBTreeItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit CAoBTreeItemDelegate(QObject *parent = 0);

	void setMethodDelegate(CComboBoxDelegate* delegate);
	void setOrderDelegate(CIntDelegate* delegate);
	void setOffsetDelegate(CIntDelegate* delegate);

	// editing
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

	void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

protected:
	bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

private:
	QList<QPair<QString, QVariant>> m_comboBoxItems;
	CComboBoxDelegate* m_methodDelegate;
	CIntDelegate* m_orderDelegate;
	CIntDelegate* m_offsetDelegate;
};