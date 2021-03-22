#include "stdafx.h"

#include "CAoBTreeItemDelegate.hpp"
#include "CAoBTreeItem.hpp"

CAoBTreeItemDelegate::CAoBTreeItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void CAoBTreeItemDelegate::setComboBoxItems(const QList<QPair<QString, QVariant>>& comboBoxItems)
{
	this->m_comboBoxItems = comboBoxItems;
}

QWidget *CAoBTreeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
	{
		QComboBox *comboBox = new QComboBox(parent);
		foreach(auto pair, this->m_comboBoxItems)
			comboBox->addItem(pair.first, pair.second);
		comboBox->installEventFilter(const_cast<CAoBTreeItemDelegate*>(this));
		return comboBox;
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void CAoBTreeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
	{
		const QString text = index.data().toString();
		const QVariant userData = index.data(Qt::UserRole + 1);
		QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
		for (int i = 0; i < this->m_comboBoxItems.size(); i++)
		{
			if (this->m_comboBoxItems.at(i).first.compare(text) == 0
				&& this->m_comboBoxItems.at(i).second.compare(userData))
			{
				comboBox->setCurrentIndex(i);
				break;
			}
		}
		return;
	}
	return QStyledItemDelegate::setEditorData(editor, index);
}

void CAoBTreeItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
	{
		QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
		auto pair = this->m_comboBoxItems.at(comboBox->currentIndex());
		model->setData(index, pair.first, Qt::EditRole);
		model->setData(index, pair.second, Qt::UserRole + 1);
		return;
	}
	return QStyledItemDelegate::setModelData(editor, model, index);
}

void CAoBTreeItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() == TYPE_GROUP_ITEM_METHOD)
	{
		editor->setGeometry(option.rect);
		return;
	}
	return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool CAoBTreeItemDelegate::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::FocusIn)
	{
		QComboBox *comboBox = qobject_cast<QComboBox*>(object);
		if (comboBox)
		{
			comboBox->showPopup();
			comboBox->removeEventFilter(this);
		}
	}
	return QStyledItemDelegate::eventFilter(object, event);
}