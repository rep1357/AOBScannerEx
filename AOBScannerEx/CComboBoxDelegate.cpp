#include "stdafx.h"

#include "CComboBoxDelegate.hpp"
#include <qcombobox.h>
#include <qevent.h>

CComboBoxDelegate::CComboBoxDelegate(const QList<QPair<QString, QVariant>> &items, QObject *parent) : QItemDelegate(parent)
{
	this->m_lItem = items;
}

QWidget *CComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QComboBox *comboBox = new QComboBox(parent);
	foreach(auto pair, this->m_lItem)
		comboBox->addItem(pair.first, pair.second);
	comboBox->installEventFilter(const_cast<CComboBoxDelegate*>(this));
	return comboBox;
}

void CComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	const QString text = index.data().toString();
	const QVariant userData = index.data(Qt::UserRole + 1);
	QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
	for (int i = 0; i < this->m_lItem.size(); i++)
	{
		if (this->m_lItem.at(i).first.compare(text) == 0
			&& this->m_lItem.at(i).second.compare(userData))
		{
			comboBox->setCurrentIndex(i);
			break;
		}
	}
}

void CComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
	auto pair = this->m_lItem.at(comboBox->currentIndex());
	model->setData(index, pair.first, Qt::EditRole);
	model->setData(index, pair.second, Qt::UserRole + 1);
}

void CComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

bool CComboBoxDelegate::eventFilter(QObject *object, QEvent *event)
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
	return QItemDelegate::eventFilter(object, event);
}