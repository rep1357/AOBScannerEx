#pragma once

QT_BEGIN_NAMESPACE
class CAoBTreeItemWidget;
class CAoBTreeItemsDetailModel;
class QTableView;
QT_END_NAMESPACE;

class CAoBTreeItemsDetailWidget : public QScrollArea
{
	Q_OBJECT

public:
	explicit CAoBTreeItemsDetailWidget(CAoBTreeItemWidget* treeWidget, QWidget* parent = nullptr);
	~CAoBTreeItemsDetailWidget(); 

	CAoBTreeItemsDetailModel* model() const;

public slots:
	void OnShowDetail(const QModelIndex& current);

private slots:
	void OnContextMenuRequested(const QPoint& pos);

private:
	CAoBTreeItemWidget* m_treeWidget;
	CAoBTreeItemsDetailModel* m_model;
	QTableView* m_view;
};