#pragma once

#include "CSingleTreeItem.hpp"

class CSingleTreeItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit CSingleTreeItemModel(QObject *parent = nullptr);
	CSingleTreeItemModel(const CSingleTreeItemModel &other) = default;
	~CSingleTreeItemModel() Q_DECL_OVERRIDE;

	// operator
	CSingleTreeItemModel &operator=(const CSingleTreeItemModel &other) = default;

	// enum
	enum class MoveDirection
	{
		Up,
		Down,
	};

public:
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	virtual QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

	virtual bool setHeaderData(int section, Qt::Orientation orientation,
		const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
	virtual QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	virtual Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;
	//void sortItems(QList<QSharedPointer<TreeItem>> &items, bool(*compareFunc)(const TreeItem *, const TreeItem *));
	void moveItem(const QModelIndex &item, MoveDirection direction);

	CSingleTreeItem* rootItem() const;
	void resetRoot(const QModelIndex &index);

protected:
	CSingleTreeItem* m_rootItem;
};