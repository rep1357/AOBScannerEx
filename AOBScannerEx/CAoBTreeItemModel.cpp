#include "stdafx.h"

#include "CAoBTreeItemModel.hpp"
#include "CAoBTreeItem.hpp"

CAoBTreeItemModel::CAoBTreeItemModel(QWidget* parent) : CSingleTreeItemModel(parent)
{
	
}

QVariant CAoBTreeItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole)
		return this->dataDisplayRole(index);
	else if (role == Qt::ForegroundRole)
		return QColor(Qt::black);
	else if (role == Qt::BackgroundRole)
		return this->dataBackgroundRole(index);

	return QVariant();
}

bool CAoBTreeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	CAoBTreeItem *item = static_cast<CAoBTreeItem *>(index.internalPointer());
	if (role == Qt::EditRole)
	{
		switch (item->type())
		{
			case TYPE_GROUP:
			{
				const QString& name = value.toString();
				if (name.isEmpty())
					return false;

				item->setName(name);
				emit this->dataChanged(index, index);
				return true;
			}
			case TYPE_GROUP_ITEM:
			{
				const QString& name = value.toString();
				if (name.isEmpty())
					return false;

				item->setName(name);
				emit this->dataChanged(index, index);
				emit this->updateReady(index.row());
				return true;
			}
			case TYPE_GROUP_ITEM_PATTERN:
			{
				const QString& pattern = value.toString();
				if (pattern.isEmpty())
					return false;

				item->setPattern(pattern);
				emit this->dataChanged(index, index);
				emit this->updateReady(index.parent().row());
				return true;
			}
			case TYPE_GROUP_ITEM_METHOD:
			{
				const QString methodString = value.toString();
				return false;
			}
			case TYPE_GROUP_ITEM_ORDER:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok || i < 1)
					return false;

				item->setOrder(i);
				emit this->dataChanged(index, index);
				emit this->updateReady(index.parent().row());
				return true;
			}
			case TYPE_GROUP_ITEM_OFFSET:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok)
					return false;

				item->setOffset(i);
				emit this->dataChanged(index, index);	
				emit this->updateReady(index.parent().row());
				return true;
			}
			case TYPE_GROUP_ITEM_IGNORE:
			{
				item->setIgnore(value.toBool());
				emit this->dataChanged(index, index);
				emit this->dataChanged(index.parent(), index.parent());
				emit this->updateReady(index.parent().row());
				return true;
			}
			case TYPE_GROUP_ITEM_COMMENT:
			{
				item->setComment(value.toBool());
				emit this->dataChanged(index, index);
				emit this->dataChanged(index.parent(), index.parent());
				emit this->updateReady(index.parent().row());
				return true;
			}
			case TYPE_GROUP_ITEM_SEARCHED:
			{
				item->setSearched(value.toBool());
				emit this->dataChanged(index, index);
				emit this->dataChanged(index.parent(), index.parent());
				emit this->updateReady(index.parent().row());
				return true;
			}
			case TYPE_GROUP_ITEM_RESULT:
			{
				bool ok;
				qulonglong i = value.toString().toULongLong(&ok, value.toString().startsWith("0x") ? 16 : 10);
				if (!ok)
					return false;

				// searched = true
				this->setData(this->index(6, 0, index.parent()), true);

				item->setResult(i);
				emit this->dataChanged(index, index);
				emit this->dataChanged(index.parent(), index.parent());
				emit this->updateReady(index.parent().row());
				return true;
			}
		}
	}
	else if (role == Qt::UserRole + 1)
	{
		if (item->type() == TYPE_GROUP_ITEM_METHOD)
		{
			bool ok;
			int i = value.toInt(&ok);
			if (!ok)
				return false;

			item->setMethod(i);
			emit this->dataChanged(index, index);
			return true;
		}
	}
	return false;
}

Qt::ItemFlags CAoBTreeItemModel::flags(const QModelIndex &index) const
{
	auto flags = CSingleTreeItemModel::flags(index);
	if (index.isValid())
	{
		const CAoBTreeItem *item = static_cast<CAoBTreeItem *>(index.internalPointer());
		switch (item->type())
		{
			case TYPE_GROUP:
			case TYPE_GROUP_ITEM:
			case TYPE_GROUP_ITEM_PATTERN:
			case TYPE_GROUP_ITEM_METHOD:
			case TYPE_GROUP_ITEM_ORDER:
			case TYPE_GROUP_ITEM_OFFSET:
			case TYPE_GROUP_ITEM_RESULT:
				flags |= Qt::ItemIsEditable;
				break;
			//case TYPE_GROUP_ITEM_IGNORE:
			//case TYPE_GROUP_ITEM_COMMENT:
			//case TYPE_GROUP_ITEM_SEARCHED:
			default:
				break;
		}
	}
	return flags;
}

QVariant CAoBTreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
		return QVariant("Scanner");

	return QVariant();
}

CAoBTreeItem* CAoBTreeItemModel::getItem(int row, const QModelIndex& index) const
{
	return static_cast<CAoBTreeItem*>(this->index(row, 0, index).internalPointer());
}

QVariant CAoBTreeItemModel::dataVariant(const QModelIndex& index) const
{
	const CAoBTreeItem *item = static_cast<CAoBTreeItem *>(index.internalPointer());
	switch (item->type())
	{
		case TYPE_GROUP:
		case TYPE_GROUP_ITEM:
			return item->name();
		case TYPE_GROUP_ITEM_PATTERN:
			return item->pattern();
		case TYPE_GROUP_ITEM_METHOD:
			return get_method_string(item->method());
		case TYPE_GROUP_ITEM_ORDER:
			return QString::number(item->order());
		case TYPE_GROUP_ITEM_OFFSET:
			return QString::number(item->offset());
		case TYPE_GROUP_ITEM_IGNORE:
			return get_bool_string(item->ignore());
		case TYPE_GROUP_ITEM_COMMENT:
			return get_bool_string(item->comment());
		case TYPE_GROUP_ITEM_SEARCHED:
			return get_bool_string(item->searched());
		case TYPE_GROUP_ITEM_RESULT:
			return QString("0x%1").arg(QString::number(item->result(), 16).rightJustified(8, '0').toUpper());
	}
	return QVariant();
}

QVariant CAoBTreeItemModel::dataDisplayRole(const QModelIndex& index) const
{
	const CAoBTreeItem *item = static_cast<CAoBTreeItem *>(index.internalPointer());
	switch (item->type())
	{
		case TYPE_GROUP:
		case TYPE_GROUP_ITEM:
			return item->name();
		case TYPE_GROUP_ITEM_PATTERN:
		case TYPE_GROUP_ITEM_METHOD:
		case TYPE_GROUP_ITEM_ORDER:
		case TYPE_GROUP_ITEM_OFFSET:
		case TYPE_GROUP_ITEM_IGNORE:
		case TYPE_GROUP_ITEM_COMMENT:
		case TYPE_GROUP_ITEM_SEARCHED:
		case TYPE_GROUP_ITEM_RESULT:
			return item->name() + this->dataVariant(index).toString();
	}
	return QVariant();
}

QVariant CAoBTreeItemModel::dataBackgroundRole(const QModelIndex& index) const
{
	if (CAoBTreeItem::isGroupItemIndex(index))
	{
		// カウント数のチェックが無いとぬるぽで死ぬ
		if (this->rowCount(index) == TYPE_GROUP_ITEM_COUNT)
		{
			if (this->getItem(6, index)->searched() && this->getItem(7, index)->result() == 0)
			{
				return QColor(Qt::red);
			}
			else
			{
				if (this->getItem(4, index)->ignore())
					return QColor(Qt::yellow);

				if (this->getItem(5, index)->comment())
					return QColor(Qt::green);

				return QColor(Qt::transparent);
			}
		}
	}

	return QColor(Qt::white);
}

void CAoBTreeItemModel::update(const QModelIndex& index)
{
	emit this->dataChanged(index, index);
}