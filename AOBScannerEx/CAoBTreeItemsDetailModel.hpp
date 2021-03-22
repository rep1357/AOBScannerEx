#pragma once

QT_BEGIN_NAMESPACE
class CAoBTreeItemWidget;
class CAoBTreeItem;
class QTreeView;
QT_END_NAMESPACE

class CAoBTreeItemsDetailModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit CAoBTreeItemsDetailModel(CAoBTreeItemWidget* treeWidget, QObject* parent = nullptr);
	~CAoBTreeItemsDetailModel() = default;

	void setTreeSelectedIndex(const QModelIndex& index);
	CAoBTreeItemWidget* treeWidget() const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
	QModelIndex m_treeSelectedIndex;
	CAoBTreeItemWidget* m_treeWidget;
};