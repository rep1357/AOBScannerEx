#include "stdafx.h"

#include "CAoBTreeItemsDetailWidget.hpp"
#include "CAoBTreeItemsDetailModel.hpp"
#include "CAoBTreeItem.hpp"
#include "CAoBTreeItemModel.hpp"
#include "CAoBTreeItemWidget.hpp"

#include "CIntDelegate.hpp"
#include "CComboBoxDelegate.hpp"

CAoBTreeItemsDetailWidget::CAoBTreeItemsDetailWidget(CAoBTreeItemWidget* treeWidget, QWidget* parent) : QScrollArea(parent)
{
	this->m_treeWidget = treeWidget;
	this->m_model = new CAoBTreeItemsDetailModel(treeWidget, this);

	this->m_view = new QTableView(this);
	this->m_view->setModel(this->m_model);
	this->m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->m_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
	this->m_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	// delegates
	this->m_view->setItemDelegateForColumn(1, new CComboBoxDelegate(std::move(get_method_items()), this)); // doesn't have rvalue impl tho
	this->m_view->setItemDelegateForColumn(2, new CIntDelegate(1, 100, this));
	this->m_view->setItemDelegateForColumn(3, new CIntDelegate(-1000, 1000, this));

	// movable
	this->m_view->verticalHeader()->setSectionsMovable(true);
	this->m_view->verticalHeader()->setDragEnabled(true);
	this->m_view->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
	this->m_view->verticalHeader()->setDropIndicatorShown(true);
	this->m_view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

	// connect
	QObject::connect(this->m_view, &QTableView::customContextMenuRequested, this, &CAoBTreeItemsDetailWidget::OnContextMenuRequested);

	// layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(this->m_view);
	mainLayout->setMargin(1);
	QWidget *innerWidget = new QWidget;
	innerWidget->setLayout(mainLayout);
	this->setWidget(innerWidget);
	this->setWidgetResizable(true);

	// load
	QSettings settings;
	// table view settings
	for (int i = 0; i < this->m_view->horizontalHeader()->count(); i++)
	{
		QVariant width = settings.value(QString("tableview/column/%1/width").arg(i));
		if (width.isValid())
			this->m_view->setColumnWidth(i, width.toInt());
	}
}

CAoBTreeItemsDetailWidget::~CAoBTreeItemsDetailWidget()
{
	QSettings settings;
	for (int i = 0; i < this->m_view->horizontalHeader()->count(); i++)
		settings.setValue(QString("tableview/column/%1/width").arg(i), this->m_view->columnWidth(i));
}

CAoBTreeItemsDetailModel * CAoBTreeItemsDetailWidget::model() const
{
	return this->m_model;
}

void CAoBTreeItemsDetailWidget::OnContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = this->m_view->indexAt(pos);
	QMenu *menu = new QMenu(this);
	QAction *action;

	const QModelIndex& treeSelectedIndex = this->m_model->treeSelectedIndex();
	const QModelIndex& treeWidgetIndex = this->m_model->treeWidgetIndex(index.row());
	//if (!child.isValid())
	//	return;

	// scan
	action = menu->addAction("Scan", this->m_treeWidget, &CAoBTreeItemWidget::OnScan);
	action->setData(treeWidgetIndex);
	action->setEnabled(CAoBTreeItem::isGroupItemIndex(treeWidgetIndex));
	menu->addAction(action);

	// scan all and separator :^)
	action = menu->addAction(QIcon(":/images/Search-64.png"), "Scan All", this->m_treeWidget, &CAoBTreeItemWidget::OnScanAll);
	action->setEnabled(this->m_model->rowCount() > 0);
	menu->addAction(action);
	menu->addSeparator();

	// insert row item
	action = menu->addAction(QIcon(":/images/Add Row-64.png"), "Insert Row Item", this->m_treeWidget, &CAoBTreeItemWidget::OnInsertRowItem);
	action->setData(treeSelectedIndex);
	action->setEnabled(CAoBTreeItem::isGroupIndex(treeSelectedIndex));
	menu->addAction(action);

	// remove row item
	action = menu->addAction(QIcon(":/images/Delete Row-64.png"), "Remove Row Item", this->m_treeWidget, &CAoBTreeItemWidget::OnRemoveRowItem);
	action->setData(treeWidgetIndex);
	action->setEnabled(CAoBTreeItem::isGroupItemIndex(treeWidgetIndex));
	menu->addAction(action);

	menu->popup(this->m_view->viewport()->mapToGlobal(pos));
}

void CAoBTreeItemsDetailWidget::OnShowDetail(const QModelIndex& index)
{	
	this->m_model->layoutAboutToBeChanged();

	const int row = this->m_treeWidget->model()->rowCount(index);
	this->m_model->removeRows(0, this->m_model->rowCount());
	this->m_model->insertRows(0, row);
	this->m_model->setTreeSelectedIndex(index);
	this->m_model->updateAll();

	this->m_model->layoutChanged();
}
