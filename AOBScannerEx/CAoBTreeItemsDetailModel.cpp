#include "stdafx.h"

#include "CAoBTreeItemsDetailModel.hpp"
#include "CAoBTreeItemWidget.hpp"
#include "CAoBTreeItemModel.hpp"
#include "CAoBTreeItem.hpp"


CAoBTreeItemsDetailModel::CAoBTreeItemsDetailModel(CAoBTreeItemWidget* treeWidget, QObject* parent) : QAbstractTableModel(parent)
{
	this->m_treeWidget = treeWidget;
}

void CAoBTreeItemsDetailModel::setTreeSelectedIndex(const QModelIndex& index)
{
	this->m_treeSelectedIndex = index;
}
QModelIndex CAoBTreeItemsDetailModel::treeSelectedIndex() const
{
	return this->m_treeSelectedIndex;
}
CAoBTreeItemWidget* CAoBTreeItemsDetailModel::treeWidget() const
{
	return this->m_treeWidget;
}
QModelIndex CAoBTreeItemsDetailModel::treeWidgetIndex(int row) const
{
	if (!this->m_treeSelectedIndex.isValid())
		return QModelIndex();

	if (this->rowCount() <= row)
		return QModelIndex();

	return this->m_treeWidget->model()->index(row, 0, this->m_treeSelectedIndex);
}

int CAoBTreeItemsDetailModel::rowCount(const QModelIndex &parent) const
{
	if (this->m_treeSelectedIndex.isValid())
		return this->m_treeWidget->model()->rowCount(this->m_treeSelectedIndex);
	return 0;
}
int CAoBTreeItemsDetailModel::columnCount(const QModelIndex &parent) const
{
	return 6;
}

QVariant CAoBTreeItemsDetailModel::data(const QModelIndex &index, int role) const
{
	if (!this->m_treeSelectedIndex.isValid())
		return QVariant();

	const int row = index.row();
	if (this->rowCount() <= row)
		return QVariant();

	// Pattern	: 0
	// Method	: 1
	// Order	: 2
	// Offset	: 3
	// Ignore	: 4
	// Comment	: 5
	// Searched	: 6
	// Result	: 7
	const QModelIndex& child = this->m_treeWidget->model()->index(row, 0, this->m_treeSelectedIndex);
	if (this->m_treeWidget->model()->rowCount(child) != TYPE_GROUP_ITEM_COUNT)
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{		
		switch (index.column())
		{
			case 0: return static_cast<CAoBTreeItem*>(child.internalPointer())->name();
			case 1: return get_method_string(this->m_treeWidget->treeItem(1, child)->method());
			case 2: return this->m_treeWidget->treeItem(2, child)->order();
			case 3: return this->m_treeWidget->treeItem(3, child)->offset();
			case 4: return this->m_treeWidget->treeItem(0, child)->pattern();
			// x86 only?????????ASDFASFASFASFSADFASFDASDFASDF
			case 5: return QString("0x%1").arg(QString::number(this->m_treeWidget->treeItem(7, child)->result(), 16).rightJustified(8, '0').toUpper());
		}
	}
	else if (role == Qt::BackgroundColorRole)
	{
		bool ignore = this->m_treeWidget->treeItem(4, child)->ignore();
		bool comment = this->m_treeWidget->treeItem(5, child)->comment();
		bool searched = this->m_treeWidget->treeItem(6, child)->searched();
		unsigned long long result = this->m_treeWidget->treeItem(7, child)->result();

		if (searched && result == 0)
			return QColor(Qt::red);
		else
		{
			if (ignore)
				return QColor(Qt::yellow);
			if (comment)
				return QColor(Qt::green);
			return QColor(Qt::transparent);
		}
	}
	return QVariant();
}

bool CAoBTreeItemsDetailModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!this->m_treeSelectedIndex.isValid())
		return false;

	const int row = index.row();
	if (this->rowCount() <= row)
		return false;

	// Pattern	: 0
	// Method	: 1
	// Order	: 2
	// Offset	: 3
	// Ignore	: 4
	// Comment	: 5
	// Searched	: 6
	// Result	: 7
	const QModelIndex& child = this->m_treeWidget->model()->index(row, 0, this->m_treeSelectedIndex);
	if (this->m_treeWidget->model()->rowCount(child) != TYPE_GROUP_ITEM_COUNT)
		return false;

	if (role == Qt::EditRole)
	{
		switch (index.column())
		{
			case 0:
			{
				CAoBTreeItem::getItem(child)->setName(value.toString());
				emit this->dataChanged(index, index);
				emit this->updateReady(child);
				return true;
			}
			case 1:
			{
				const QString typeString = value.toString();
				return false;
			}
			case 2:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok || i < 0)
					return false;

				const QModelIndex& order = this->m_treeWidget->model()->index(2, 0, child);
				CAoBTreeItem::getItem(order)->setOrder(i);
				emit this->dataChanged(index, index);
				emit this->updateReady(order);
				return true;
			}
			case 3:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok)
					return false;

				const QModelIndex& offset = this->m_treeWidget->model()->index(3, 0, child);
				CAoBTreeItem::getItem(offset)->setOffset(i);
				emit this->dataChanged(index, index);
				emit this->updateReady(offset);
				return true;
			}
			case 4:
			{
				const QModelIndex& pattern = this->m_treeWidget->model()->index(0, 0, child);
				CAoBTreeItem::getItem(pattern)->setPattern(value.toString());
				emit this->dataChanged(index, index);
				emit this->updateReady(pattern);
				return true;
			}
			case 5:
			{
				bool ok;
				qulonglong i = value.toString().toULongLong(&ok, value.toString().startsWith("0x") ? 16 : 10);
				if (!ok)
					return false;

				const QModelIndex& result = this->m_treeWidget->model()->index(7, 0, child);
				const QModelIndex& searched = this->m_treeWidget->model()->index(6, 0, child);
				CAoBTreeItem::getItem(result)->setResult(i);
				CAoBTreeItem::getItem(searched)->setSearched(true);

				// update entire row
				emit this->dataChanged(this->index(row, 0), this->index(row, this->columnCount() - 1));
				// update tree items
				emit this->updateReady(result);
				emit this->updateReady(searched);
				emit this->updateReady(child);
				return true;
			}
		}
	}
	else if (role == Qt::UserRole + 1)
	{
		switch (index.column())
		{
			case 1:
			{
				bool ok;
				int i = value.toInt(&ok);
				if (!ok)
					return false;

				const QModelIndex& method = this->m_treeWidget->model()->index(1, 0, child);
				CAoBTreeItem::getItem(method)->setMethod(i);
				emit this->dataChanged(index, index);
				emit this->updateReady(method);
				return true;
			}
		}
	}

	return false;
}

QVariant CAoBTreeItemsDetailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
			case 0: return QString("Name");
			case 1: return QString("Method");
			case 2: return QString("Order");
			case 3: return QString("Offset");
			case 4: return QString("AoB");
			case 5: return QString("Result");
		}
	}
	else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		return QString::number(section);
	}

	return QVariant();
}

Qt::ItemFlags CAoBTreeItemsDetailModel::flags(const QModelIndex &index) const
{
	// don't allow to edit column5(result)
	auto flags = QAbstractTableModel::flags(index);
	//if (index.column() != 5)
	flags |= Qt::ItemIsEditable;
	return flags;
}

void CAoBTreeItemsDetailModel::update(int row)
{
	if (this->rowCount() <= row)
		return;

	// update entire row
	emit this->dataChanged(this->index(row, 0), this->index(row, this->columnCount() - 1));
}
void CAoBTreeItemsDetailModel::updateAll()
{
	const int row = this->rowCount();
	for (int i = 0; i < row; i++)
	{
		// update entire row
		emit this->dataChanged(this->index(i, 0), this->index(i, this->columnCount() - 1));
	}
}