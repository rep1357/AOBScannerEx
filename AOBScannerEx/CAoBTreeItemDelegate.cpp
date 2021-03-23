#include "stdafx.h"

#include "CAoBTreeItemDelegate.hpp"
#include "CAoBTreeItem.hpp"
#include "CComboBoxDelegate.hpp"
#include "CIntDelegate.hpp"

CAoBTreeItemDelegate::CAoBTreeItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
	this->m_methodDelegate = nullptr;
	this->m_orderDelegate = nullptr;
	this->m_offsetDelegate = nullptr;
}
void CAoBTreeItemDelegate::setMethodDelegate(CComboBoxDelegate* delegate)
{
	this->m_methodDelegate = delegate;
}
void CAoBTreeItemDelegate::setOrderDelegate(CIntDelegate* delegate)
{
	this->m_orderDelegate = delegate;
}
void CAoBTreeItemDelegate::setOffsetDelegate(CIntDelegate* delegate)
{
	this->m_offsetDelegate = delegate;
}

QWidget *CAoBTreeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
		return this->m_methodDelegate->createEditor(parent, option, index);
	else if (item->type() == TYPE_GROUP_ITEM_ORDER)
		return this->m_orderDelegate->createEditor(parent, option, index);
	else if (item->type() == TYPE_GROUP_ITEM_OFFSET)
		return this->m_offsetDelegate->createEditor(parent, option, index);

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void CAoBTreeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
		return this->m_methodDelegate->setEditorData(editor, index);

	return QStyledItemDelegate::setEditorData(editor, index);
}

void CAoBTreeItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
		return this->m_methodDelegate->setModelData(editor, model, index);

	return QStyledItemDelegate::setModelData(editor, model, index);
}

void CAoBTreeItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
		return this->m_methodDelegate->updateEditorGeometry(editor, option, index);

	return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool CAoBTreeItemDelegate::eventFilter(QObject *object, QEvent *event)
{
	return QStyledItemDelegate::eventFilter(object, event);
}