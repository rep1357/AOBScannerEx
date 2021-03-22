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

public slots:
	void OnShowDetail(const QModelIndex& current);

private:
	CAoBTreeItemsDetailModel* m_model;
	QTableView* m_view;
};