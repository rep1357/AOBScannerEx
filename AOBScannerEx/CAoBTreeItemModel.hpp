#pragma once

#include "CSingleTreeItemModel.hpp"

QT_BEGIN_NAMESPACE
class CAoBTreeItem;
QT_END_NAMESPACE

class CAoBTreeItemModel : public CSingleTreeItemModel
{
	Q_OBJECT

public:
	explicit CAoBTreeItemModel(QWidget* parent = nullptr);
	~CAoBTreeItemModel() = default;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	CAoBTreeItem* getItem(int row, const QModelIndex& index) const;

	void update(const QModelIndex& index);

	QVariant dataVariant(const QModelIndex& index) const;

signals:
	void updateReady(int row); // => CAoBTreeItemsDetailModel::update

private:
	QVariant dataDisplayRole(const QModelIndex& index) const;
	QVariant dataBackgroundRole(const QModelIndex& index) const;
};