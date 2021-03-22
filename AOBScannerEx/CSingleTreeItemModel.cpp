#include "stdafx.h"

#include "CSingleTreeItemModel.hpp"

CSingleTreeItemModel::CSingleTreeItemModel(QObject *parent) : QAbstractItemModel(parent)
{
	this->m_rootItem = new CSingleTreeItem;
}
CSingleTreeItemModel::~CSingleTreeItemModel()
{
	delete this->m_rootItem;
}

int CSingleTreeItemModel::rowCount(const QModelIndex &parent) const
{
	const CSingleTreeItem *parentItem;

	if (!parent.isValid())
		parentItem = this->m_rootItem;
	else
		parentItem = static_cast<CSingleTreeItem *>(parent.internalPointer());

	return parentItem->childCount();
}

int CSingleTreeItemModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

QVariant CSingleTreeItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	const CSingleTreeItem *item = static_cast<CSingleTreeItem *>(index.internalPointer());
	switch (role)
	{
		case Qt::DisplayRole: return item->name();
#if _Q_NO_CUSTOMTREEMODEL
		case Qt::DecorationRole: return item->icon();
		case Qt::BackgroundRole: return item->backColor();
		case Qt::ForegroundRole: return item->textColor();
#endif
		default: break;
	}
	return QVariant();
}

bool CSingleTreeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	CSingleTreeItem *item = static_cast<CSingleTreeItem *>(index.internalPointer());
	if (role == Qt::EditRole)
	{
		item->setName(value.toString());
		this->dataChanged(index, index);
		return true;
	}
	return false;
}

QModelIndex CSingleTreeItemModel::index(int row, int column, const QModelIndex &parent) const
{
	CSingleTreeItem *parentItem;
	if (!parent.isValid())
		parentItem = this->m_rootItem;
	else
		parentItem = static_cast<CSingleTreeItem *>(parent.internalPointer());

	CSingleTreeItem *childItem = parentItem->child(row);
	if (childItem != nullptr)
		return this->createIndex(row, column, childItem);

	return QModelIndex();
}

QModelIndex CSingleTreeItemModel::parent(const QModelIndex &child) const
{
	if (!child.isValid())
		return QModelIndex();

	const CSingleTreeItem *childItem = static_cast<CSingleTreeItem *>(child.internalPointer());
	if (!childItem)
		return QModelIndex();

	CSingleTreeItem *parentItem = childItem->parent();
	if (parentItem == this->m_rootItem)
		return QModelIndex();

	return this->createIndex(parentItem->row(), 0, parentItem);
}

bool CSingleTreeItemModel::setHeaderData(int section, Qt::Orientation orientation,
	const QVariant &value, int role)
{
	if (QAbstractItemModel::setHeaderData(section, orientation, value, role))
	{
		emit this->headerDataChanged(orientation, section, section);
		return true;
	}
	return false;
}

QVariant CSingleTreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

bool CSingleTreeItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
	CSingleTreeItem *parentItem;
	if (parent.isValid())
		parentItem = static_cast<CSingleTreeItem *>(parent.internalPointer());
	else
		parentItem = this->m_rootItem;

	this->beginInsertRows(parent, row, row + count - 1);
	parentItem->insertChild(new CSingleTreeItem(parentItem));
	this->endInsertRows();

	return true;
}

bool CSingleTreeItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
	CSingleTreeItem *parentItem;
	if (!parent.isValid())
		parentItem = this->m_rootItem;
	else
		parentItem = static_cast<CSingleTreeItem *>(parent.internalPointer());

	this->beginRemoveRows(parent, row, row + count - 1);
	parentItem->removeChild(row);
	this->endRemoveRows();

	return true;
}

void CSingleTreeItemModel::moveItem(const QModelIndex &item, MoveDirection direction)
{
	if (!item.isValid())
		return;

	CSingleTreeItem *node = static_cast<CSingleTreeItem *>(item.internalPointer());
	int targetItemPos = node->row();
	if (item.parent().isValid())
		node = static_cast<CSingleTreeItem *>(item.parent().internalPointer());
	else
		node = this->m_rootItem;

	if (direction == MoveDirection::Up && targetItemPos > 0)
	{
		this->beginMoveRows(item.parent(), targetItemPos, targetItemPos, item.parent(),
			targetItemPos - 1);
		node->swapChildren(targetItemPos, targetItemPos - 1);
		this->endMoveRows();
	}
	else if (direction == MoveDirection::Down && targetItemPos < node->childCount() - 1)
	{
		this->beginMoveRows(item.parent(), targetItemPos + 1, targetItemPos + 1,
			item.parent(), targetItemPos);
		node->swapChildren(targetItemPos, targetItemPos + 1);
		this->endMoveRows();
	}
}

void CSingleTreeItemModel::sort(int column, Qt::SortOrder order)
{
	Q_UNUSED(column);

	this->layoutAboutToBeChanged();
	//if (order == Qt::AscendingOrder)
	//    this->sortItems(this->m_rootItem->children(), TreeItem::itemCompareAsc);
	//else
	//    this->sortItems(this->m_rootItem->children(), TreeItem::itemCompareDes);
	this->layoutChanged();
}

Qt::DropActions CSingleTreeItemModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::ItemFlags CSingleTreeItemModel::flags(const QModelIndex &index) const
{
	if (index.isValid())
		return (QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled);
	return Qt::ItemIsDropEnabled;
}

CSingleTreeItem* CSingleTreeItemModel::rootItem() const
{
	return this->m_rootItem;
}

void CSingleTreeItemModel::resetRoot(const QModelIndex &index)
{
	this->layoutAboutToBeChanged();
	this->beginRemoveRows(index, 0, this->rowCount(index));

	// delete real data
	// delete this->m_rootItem;
	// this->m_rootItem = new CSingleTreeItem;
	this->m_rootItem->deleteAll();

	this->endRemoveRows();
	this->layoutChanged();
}