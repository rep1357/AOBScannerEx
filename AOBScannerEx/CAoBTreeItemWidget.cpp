#include "stdafx.h"

#include "CAoBTreeItemWidget.hpp"
#include "CAoBTreeItem.hpp"
#include "CAoBTreeItemModel.hpp"
#include "CAoBTreeItemDelegate.hpp"
#include "CAoBTreeItemsDetailModel.hpp"
#include "CAoBTreeItemsDetailWidget.hpp"
#include "CMainWindow.hpp"
#include "CProcessDialog.hpp"
#include "CFindPatternWorker.hpp"

#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

CAoBTreeItemWidget::CAoBTreeItemWidget(QWidget* parent) : QScrollArea(parent)
{
	//
	this->m_filename = QString();
	this->m_moduleBuffer = NULL;
	this->m_moduleBaseAddress = NULL;
	this->m_dwSizeOfImage = 0;

	// model
	this->m_model = new CAoBTreeItemModel(this);
	this->m_model->setHeaderData(0, Qt::Horizontal, "", Qt::DisplayRole);

	// view
	this->m_view = new QTreeView(this);
	this->m_view->setModel(this->m_model);
	this->m_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	this->m_view->setExpandsOnDoubleClick(false);
	this->m_view->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
	this->m_view->setUpdatesEnabled(true);
	// stylesheet (grid line)
	QString treeViewStyle =
		"QTreeView { show-decoration-selected: 1; }"
		"QTreeView::item:has-children:open {background-color: lightgray;}"
		"QTreeView::item:!has-children { border: 0.5px ; border-style: solid ; border-color: lightgray ;}"
		"QTreeView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 lightgray, stop: 1 white); }"
		"QTreeView::item:selected:active { background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 lightgray, stop: 1 lightgray); color: black; }"
		"QTreeView::item:selected:!active { background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 lightgray, stop: 1 lightgray); color: black;}";
	this->m_view->setStyleSheet(treeViewStyle);
	// delegate
	CAoBTreeItemDelegate* itemDelegate = new CAoBTreeItemDelegate(this);
	itemDelegate->setComboBoxItems(std::move(get_method_items()));
	this->m_view->setItemDelegate(itemDelegate);

	// connect
	QObject::connect(this->m_view, &QTreeView::customContextMenuRequested, this, &CAoBTreeItemWidget::OnContextMenuRequested);
	QObject::connect(this->m_view, &QTreeView::clicked, this, &CAoBTreeItemWidget::OnTreeItemSingleClicked);
	QObject::connect(this->m_view, &QTreeView::doubleClicked, this, &CAoBTreeItemWidget::OnTreeItemDoubleClicked);
	QObject::connect(this->m_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &CAoBTreeItemWidget::OnSelectionCurrentChanged);

	// layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(this->m_view);
	mainLayout->setMargin(1);
	QWidget *innerWidget = new QWidget;
	innerWidget->setLayout(mainLayout);
	this->setWidget(innerWidget);
	this->setWidgetResizable(true);

	// 8 threads perhaps?
	qRegisterMetaType<FindPatternParam>("FindPatternParam");
	for (int i = 0; i < 8; i++)
	{
		QThread *workerThread = new QThread(this);
		CFindPatternWorker *worker = new CFindPatternWorker;
		worker->moveToThread(workerThread);
		QObject::connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
		QObject::connect(worker, &CFindPatternWorker::resultReady, this, &CAoBTreeItemWidget::OnHandleResults);
		workerThread->start();

		struct Worker workerStruct;
		workerStruct.thread = workerThread;
		workerStruct.worker = worker;
		workerStruct.requestCount = 0;
		this->m_workers.append(workerStruct);
	}
}
CAoBTreeItemWidget::~CAoBTreeItemWidget()
{
	for (auto a : this->m_workers)	
		a.thread->quit();
	for (auto a : this->m_workers)	
		a.thread->wait();	
}

void CAoBTreeItemWidget::OnContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = this->m_view->indexAt(pos);
	QMenu *menu = new QMenu(this);
	QAction *action;

	// scan
	action = menu->addAction("Scan", this, SLOT(OnScan()));
	action->setData(index);
	action->setEnabled(this->isGroupItemIndex(index));
	menu->addAction(action);

	// scan all and separator :^)
	action = menu->addAction(QIcon(":/images/Search-64.png"), "Scan All", this, SLOT(OnScanAll()));
	action->setEnabled(this->m_model->rowCount() > 0);
	menu->addAction(action);
	menu->addSeparator();

	// insert row
	action = menu->addAction(QIcon(":/images/Add Row-64.png"), "Insert Row", this, SLOT(OnInsertRow()));
	menu->addAction(action);

	// insert row item
	action = menu->addAction(QIcon(":/images/Add Row-64.png"), "Insert Row Item", this, SLOT(OnInsertRowItem()));
	action->setData(index);
	action->setEnabled(this->isGroupIndex(index));
	menu->addAction(action);

	// remove row
	action = menu->addAction(QIcon(":/images/Delete Row-64.png"), "Remove Row", this, SLOT(OnRemoveRow()));
	action->setData(index);
	action->setEnabled(this->isGroupIndex(index));
	menu->addAction(action);

	// remove row item
	action = menu->addAction(QIcon(":/images/Delete Row-64.png"), "Remove Row Item", this, SLOT(OnRemoveRowItem()));
	action->setData(index);
	action->setEnabled(this->isGroupItemIndex(index));
	menu->addAction(action);

	menu->popup(this->m_view->viewport()->mapToGlobal(pos));
}

void CAoBTreeItemWidget::OnTreeItemSingleClicked(const QModelIndex& index)
{
	if (index.isValid() && index.parent() == this->m_view->rootIndex())
	{
		emit this->handleShowDetail(index);
		this->m_detailIndex = index;
	}
}

void CAoBTreeItemWidget::OnTreeItemDoubleClicked(const QModelIndex& index)
{
	if (!index.isValid())	
		return;

	CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	switch (item->type()) // ITEM_TYPE
	{
		case TYPE_GROUP_ITEM_IGNORE:
		{
			this->m_model->setData(index, !item->ignore());
			return;
		}
		case TYPE_GROUP_ITEM_COMMENT:
		{
			this->m_model->setData(index, !item->comment());
			return;
		}
		case TYPE_GROUP_ITEM_SEARCHED:
		{
			this->m_model->setData(index, !item->searched());
			return;
		}
		default:
			return;
	}
}

void CAoBTreeItemWidget::OnSelectionCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (current.isValid() && current.parent() == this->m_view->rootIndex())
	{
		emit this->handleShowDetail(current);
		this->m_detailIndex = current;
		QObject::disconnect(this->m_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &CAoBTreeItemWidget::OnSelectionCurrentChanged);
	}
}

void CAoBTreeItemWidget::OnLoad()
{
	const QString filename = QFileDialog::getOpenFileName(this, QString(), QSettings().value("inifile").toString(), "Xml files (*.xml)");
	if (filename.isEmpty())
		return;

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, QString(), file.errorString(), QMessageBox::Ok);
		return;
	}

	QXmlStreamReader reader(&file);
	while (!reader.atEnd() && !reader.hasError())
	{
		QXmlStreamReader::TokenType token = reader.readNext();
		if (token == QXmlStreamReader::StartElement)
		{
			if (reader.name() == "scanner")
			{
				this->readFromXml(reader);
				break;
			}
		}
	}

	file.close();

	this->m_filename = filename;
}
void CAoBTreeItemWidget::OnSave()
{
	if (this->m_filename.isEmpty())
	{
		// if no file exists, try to save as...
		this->OnSaveAs();
		return;
	}

	QFile file(this->m_filename);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, QString(), file.errorString(), QMessageBox::Ok);
		return;
	}

	QXmlStreamWriter writer(&file);
	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(4);
	writer.writeStartDocument();
	// write elements
	writer.writeStartElement("scanner");
	this->writeXmlElement(writer);
	writer.writeEndElement();
	writer.writeEndDocument();

	file.close();

	QMessageBox::information(this, QString(), tr("Done."), QMessageBox::Ok);
}
void CAoBTreeItemWidget::OnSaveAs()
{
	const QString filename = QFileDialog::getSaveFileName(this, QString(), QSettings().value("inifile").toString(), "Xml files (*.xml)");
	if (filename.isEmpty())
		return;

	this->m_filename = filename;
	this->OnSave();
}

BOOL GetProcessModuleInfo(HANDLE hProcess, LPVOID *lplpBaseOfDll, LPDWORD lpdwSizeOfImage)
{
	if (lplpBaseOfDll == NULL || lpdwSizeOfImage == NULL)
		return FALSE;

	PROCESS_BASIC_INFORMATION pbi;
	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL)))
	{
		PEB peb;
		if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(PEB), NULL))
			return FALSE;

		// process module
		HMODULE hModule = (HMODULE)peb.Reserved3[1];

		// manual
		IMAGE_DOS_HEADER imageDosHeader;
		IMAGE_NT_HEADERS imageNtHeaders;
		LONG e_lfanew;

		// read dos header
		if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(hModule), &imageDosHeader, sizeof(IMAGE_DOS_HEADER), NULL))
			return FALSE;

		if (imageDosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		{
			// wrong signature
			return FALSE;
		}

		e_lfanew = imageDosHeader.e_lfanew;
		if (e_lfanew >= 0 && e_lfanew < 0x10000000)
		{
			if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(PBYTE(hModule) + e_lfanew),
				&imageNtHeaders, sizeof(IMAGE_NT_HEADERS), NULL))
			{
				return FALSE;
			}

			if (imageNtHeaders.Signature == IMAGE_NT_SIGNATURE)
			{
				*lplpBaseOfDll = hModule;
				*lpdwSizeOfImage = imageNtHeaders.OptionalHeader.SizeOfCode;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CAoBTreeItemWidget::OnOpenProcess()
{
	CProcessDialog processDialog;
	if (processDialog.exec() == QDialog::Rejected)
	{
		qDebug() << "Rejected";
		return;
	}

	DWORD dwProcessId = processDialog.GetSelectedProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		QMessageBox::information(this, QString(), QString("OpenProcess failed."), QMessageBox::Ok);
		return;
	}

	LPVOID lpBaseOfDll;
	DWORD dwSizeOfImage;
	if (!GetProcessModuleInfo(hProcess, &lpBaseOfDll, &dwSizeOfImage))
	{
		CloseHandle(hProcess);
		QMessageBox::information(this, QString(), QString("GetProcessModuleInfo failed."), QMessageBox::Ok);
		return;
	}

	if (this->m_moduleBuffer)
		VirtualFree(this->m_moduleBuffer, 0, MEM_RELEASE);

	this->m_moduleBuffer = VirtualAlloc(NULL, dwSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	this->m_moduleBaseAddress = lpBaseOfDll;
	this->m_dwSizeOfImage = dwSizeOfImage;
	if (!this->m_moduleBuffer)
	{
		CloseHandle(hProcess);
		return;
	}

	if (!ReadProcessMemory(hProcess, this->m_moduleBaseAddress, this->m_moduleBuffer, this->m_dwSizeOfImage, NULL))
	{
		VirtualFree(this->m_moduleBuffer, 0, MEM_RELEASE);
		this->m_moduleBuffer = NULL;
		CloseHandle(hProcess);
		return;
	}

	CloseHandle(hProcess);

	this->setWindowTitle(QString("Scanner - %1").arg(QString::number(dwProcessId)));
}

void CAoBTreeItemWidget::OnExport()
{

}

void CAoBTreeItemWidget::OnScan()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!this->isGroupItemIndex(index))
		return;

	if (this->m_moduleBuffer == NULL)
	{
		// open process if not
		this->OnOpenProcess();
		if (this->m_moduleBuffer == NULL)
			return;
	}

	const QString pattern = this->treeItem(0, index)->pattern();
	if (pattern.isEmpty())
		return;

	this->scan(index);
}
void CAoBTreeItemWidget::OnScanAll()
{
	if (this->m_moduleBuffer == NULL)
	{
		// open process if not
		this->OnOpenProcess();
		if (this->m_moduleBuffer == NULL)
			return;
	}

	this->scanAll();
}

void CAoBTreeItemWidget::OnInsertRow()
{
	const QString name = QInputDialog::getText(this, QString(), tr("Enter name :"));
	if (name.isEmpty())
		return;

	const QModelIndex& index = this->m_view->rootIndex();
	const int row = this->m_model->rowCount(index);
	if (this->m_model->insertRow(row, index))
	{
		const QModelIndex& child = this->m_model->index(row, 0, index);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName(name);
		item->setType(TYPE_GROUP);
	}
}
void CAoBTreeItemWidget::OnInsertRowItem()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!this->isGroupIndex(index))
		return;

	const QString name = QInputDialog::getText(this, QString(), tr("Enter item name :"));
	if (name.isEmpty())
		return;

	const int row = this->m_model->rowCount(index);
	if (this->m_model->insertRow(row, index))
	{
		const QModelIndex& child = this->m_model->index(row, 0, index);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName(name);
		item->setType(TYPE_GROUP_ITEM);
		this->insertRowItems(child);
		//
		this->updateDetailWidget();
	}
}

void CAoBTreeItemWidget::OnRemoveRow()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!this->isGroupIndex(index))
		return;

	if (!this->m_model->removeRow(index.row(), index.parent()))
		return;

	this->updateDetailWidget();
}
void CAoBTreeItemWidget::OnRemoveRowItem()
{
	const QModelIndex &index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	if (!this->isGroupItemIndex(index))
		return;

	if (!this->m_model->removeRow(index.row(), index.parent()))
		return;

	this->updateDetailWidget();	
}

void CAoBTreeItemWidget::scan(const QModelIndex& index)
{
	if (!this->isGroupItemIndex(index))
		return;

	if (this->treeItem(0, index)->pattern().isEmpty())
	{
		// probably separater if no patterns
		return;
	}

	// sort by request count
	qSort(this->m_workers.begin(), this->m_workers.end(), [](struct Worker &w1, struct Worker &w2) -> bool
	{
		return w1.requestCount < w2.requestCount;
	});

	// Pattern	: 0
	// Method	: 1
	// Order	: 2
	// Offset	: 3
	// Ignore	: 4
	// Comment	: 5
	// Searched	: 6
	// Result	: 7
	FindPatternParam param(this->m_model->index(7, 0, index), (unsigned long long)this->m_moduleBaseAddress, this->m_moduleBuffer, this->m_dwSizeOfImage,
		this->treeItem(0, index)->pattern(),
		this->treeItem(1, index)->method(),
		this->treeItem(2, index)->order(),
		this->treeItem(3, index)->offset());

	auto& worker = this->m_workers[0];
	QMetaObject::invokeMethod(worker.worker, "doWork", Qt::QueuedConnection, Q_ARG(const FindPatternParam&, param));
	worker.requestCount++;
}
void CAoBTreeItemWidget::scanAll()
{
	const QModelIndex& rootIndex = this->m_view->rootIndex();
	const int rowGroups = this->m_model->rowCount(rootIndex);
	for (int i = 0; i < rowGroups; i++)
	{
		const QModelIndex& groupChild = this->m_model->index(i, 0, rootIndex);
		const int rowGroupItems = this->m_model->rowCount(groupChild);
		for (int j = 0; j < rowGroupItems; j++)
		{
			const QModelIndex& groupItemChild = this->m_model->index(j, 0, groupChild);
			if (this->isGroupItemIndex(groupItemChild))
			{
				this->scan(groupItemChild);
			}
		}
	}
}

void CAoBTreeItemWidget::OnHandleResults(const QModelIndex& index, unsigned long long result)
{
	CFindPatternWorker *worker = qobject_cast<CFindPatternWorker *>(this->sender());
	for (auto &workerStruct : this->m_workers)
	{
		if (workerStruct.worker == worker)
		{
			workerStruct.requestCount = std::max<>(0, workerStruct.requestCount - 1);
			break;
		}
	}

	this->m_model->setData(index, result);
}

CAoBTreeItemModel* CAoBTreeItemWidget::model() const
{
	return this->m_model;
}

QTreeView* CAoBTreeItemWidget::view() const
{
	return this->m_view;
}

CAoBTreeItem* CAoBTreeItemWidget::treeItem(int row, const QModelIndex& index) const
{
	return this->m_model->getItem(row, index);
}

void CAoBTreeItemWidget::updateDetailWidget()
{
	if (this->m_detailIndex.isValid())
		emit this->m_view->clicked(this->m_detailIndex);
}

bool CAoBTreeItemWidget::isGroupIndex(const QModelIndex& index)
{
	if (!index.isValid())
		return false;
	
	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() != TYPE_GROUP)
		return false;

	return true;
}
bool CAoBTreeItemWidget::isGroupItemIndex(const QModelIndex& index)
{
	if (!index.isValid())
		return false;

	const CAoBTreeItem* item = static_cast<CAoBTreeItem*>(index.internalPointer());
	if (item->type() != TYPE_GROUP_ITEM)
		return false;

	return true;
}

bool CAoBTreeItemWidget::insertRowItems(const QModelIndex& parent)
{
	if (!this->isGroupItemIndex(parent))
		return false;

	// insert items
	return (this->insertGroupItemPattern(parent).isValid()	// pattern
		&& this->insertGroupItemMethod(parent).isValid()	// method
		&& this->insertGroupItemOrder(parent).isValid()		// order
		&& this->insertGroupItemOffset(parent).isValid()	// offset
		&& this->insertGroupItemIgnore(parent).isValid()	// ignore
		&& this->insertGroupItemComment(parent).isValid()	// comment
		&& this->insertGroupItemSearched(parent).isValid()	// searched
		&& this->insertGroupItemResult(parent).isValid()	// result
		);
}

void CAoBTreeItemWidget::writeXmlElement(QXmlStreamWriter& writer)
{
	writer.writeStartElement("groups");
	const QModelIndex& rootIndex = this->m_view->rootIndex();
	const int rowGroups = this->m_model->rowCount(rootIndex);
	for (int i = 0; i < rowGroups; i++)
	{
		const QModelIndex& groupChild = this->m_model->index(i, 0, rootIndex);
		const CAoBTreeItem* groupItem = static_cast<CAoBTreeItem*>(groupChild.internalPointer());
		writer.writeTextElement("group", groupItem->name());

		writer.writeStartElement("groupitems");
		const int rowGroupItems = this->m_model->rowCount(groupChild);
		for (int j = 0; j < rowGroupItems; j++)
		{
			const QModelIndex& groupItemChild = this->m_model->index(j, 0, groupChild);
			const CAoBTreeItem* groupItemItem = static_cast<CAoBTreeItem*>(groupItemChild.internalPointer());
			writer.writeTextElement("groupitem", groupItemItem->name());

			// Pattern	: 0
			// Method	: 1
			// Order	: 2
			// Offset	: 3
			// Ignore	: 4
			// Comment	: 5
			// Searched	: 6
			// Result	: 7
			writer.writeStartElement("groupitemsitem");
			writer.writeTextElement("pattern", this->treeItem(0, groupItemChild)->pattern());
			writer.writeTextElement("method", QString::number(this->treeItem(1, groupItemChild)->method()));
			writer.writeTextElement("order", QString::number(this->treeItem(2, groupItemChild)->order()));
			writer.writeTextElement("offset", QString::number(this->treeItem(3, groupItemChild)->offset()));
			writer.writeTextElement("ignore", QString::number(this->treeItem(4, groupItemChild)->ignore()));
			writer.writeTextElement("comment", QString::number(this->treeItem(5, groupItemChild)->comment()));
			writer.writeTextElement("searched", QString::number(this->treeItem(6, groupItemChild)->searched()));
			writer.writeTextElement("result", QString::number(this->treeItem(7, groupItemChild)->result(), 16));
			writer.writeEndElement();
		}
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
void CAoBTreeItemWidget::readXmlGroupItem(QXmlStreamReader& reader, const QModelIndex& parent)
{
	if (this->insertRowItems(parent))
	{
		while (!reader.atEnd() && !reader.hasError())
		{
			QXmlStreamReader::TokenType token = reader.readNext();
			if (reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "groupitemsitem")
			{
				// element groupitemsitem end
				break;
			}
			if (token == QXmlStreamReader::StartElement)
			{
				// Pattern	: 0
				// Method	: 1
				// Order	: 2
				// Offset	: 3
				// Ignore	: 4
				// Comment	: 5
				// Searched	: 6
				// Result	: 7
				if (reader.name() == "pattern")
					this->treeItem(0, parent)->setPattern(reader.readElementText());
				else if (reader.name() == "method")
					this->treeItem(1, parent)->setMethod(reader.readElementText().toInt());
				else if (reader.name() == "order")
					this->treeItem(2, parent)->setOrder(reader.readElementText().toInt());
				else if (reader.name() == "offset")
					this->treeItem(3, parent)->setOffset(reader.readElementText().toInt());
				else if (reader.name() == "ignore")
					this->treeItem(4, parent)->setIgnore(reader.readElementText().toInt());
				else if (reader.name() == "comment")
					this->treeItem(5, parent)->setComment(reader.readElementText().toInt());
				else if (reader.name() == "searched")
					this->treeItem(6, parent)->setSearched(reader.readElementText().toInt());
				else if (reader.name() == "result")
					this->treeItem(7, parent)->setResult(reader.readElementText().toULongLong(nullptr, 16));
			}
		}
	}
}
void CAoBTreeItemWidget::readXmlGroup(QXmlStreamReader& reader, const QModelIndex& parent)
{
	while (!reader.atEnd() && !reader.hasError())
	{
		QXmlStreamReader::TokenType token = reader.readNext();
		if (reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "groupitems")
		{
			// element groupitems end
			break;
		}
		if (token == QXmlStreamReader::StartElement)
		{
			if (reader.name() == "groupitem")
			{
				const QModelIndex& groupItemChild = this->insertGroupItem(reader.readElementText(), parent);
				if (groupItemChild.isValid())
				{
					// next
					this->m_view->setCurrentIndex(groupItemChild);

					// read group item
					this->readXmlGroupItem(reader, groupItemChild);

					// prev
					this->m_view->setCurrentIndex(this->m_view->selectionModel()->currentIndex().parent());
				}
			}
		}
	}
}
void CAoBTreeItemWidget::readFromXml(QXmlStreamReader& reader)
{
	// reset
	this->m_model->resetRoot(this->m_view->rootIndex());

	// update
	this->m_model->layoutAboutToBeChanged();
	while (!reader.atEnd() && !reader.hasError())
	{
		QXmlStreamReader::TokenType token = reader.readNext();
		if (reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "groups")
		{
			// element groups end
			break;
		}
		if (token == QXmlStreamReader::StartElement)
		{
			if (reader.name() == "group")
			{
				const QModelIndex& groupChild = this->insertGroup(reader.readElementText());
				if (groupChild.isValid())
				{
					// next
					this->m_view->setCurrentIndex(groupChild);

					// read group
					this->readXmlGroup(reader, groupChild);

					// prev
					this->m_view->setCurrentIndex(this->m_view->selectionModel()->currentIndex().parent());
				}
			}			
		}
	}
	this->m_model->layoutChanged();
}

QModelIndex CAoBTreeItemWidget::insertGroup(const QString& groupName)
{
	const QModelIndex& rootIndex = this->m_view->rootIndex();
	const int row = this->m_model->rowCount(rootIndex);
	if (this->m_model->insertRow(row, rootIndex))
	{
		const QModelIndex& child = this->m_model->index(row, 0, rootIndex);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName(groupName);
		item->setType(TYPE_GROUP);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItem(const QString& groupItemName, const QModelIndex& parent)
{
	const int row = this->m_model->rowCount(parent);
	if (this->m_model->insertRow(row, parent))
	{
		const QModelIndex& child = this->m_model->index(row, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName(groupItemName);
		item->setType(TYPE_GROUP_ITEM);
		return child;
	}
	return QModelIndex();
}

QModelIndex CAoBTreeItemWidget::insertGroupItemPattern(const QModelIndex& parent, const QString& pattern)
{
	// pattern
	if (this->m_model->insertRow(0, parent))
	{
		const QModelIndex& child = this->m_model->index(0, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Pattern: ");
		item->setType(TYPE_GROUP_ITEM_PATTERN);
		item->setPattern(pattern);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemMethod(const QModelIndex& parent, int method)
{
	// method
	if (this->m_model->insertRow(1, parent))
	{
		const QModelIndex& child = this->m_model->index(1, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Method: ");
		item->setType(TYPE_GROUP_ITEM_METHOD);
		item->setMethod(method);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemOrder(const QModelIndex& parent, int order)
{
	// order
	if (this->m_model->insertRow(2, parent))
	{
		const QModelIndex& child = this->m_model->index(2, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Order: ");
		item->setType(TYPE_GROUP_ITEM_ORDER);
		item->setOrder(order);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemOffset(const QModelIndex& parent, int offset)
{
	// offset
	if (this->m_model->insertRow(3, parent))
	{
		const QModelIndex& child = this->m_model->index(3, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Offset: ");
		item->setType(TYPE_GROUP_ITEM_OFFSET);
		item->setOffset(offset);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemIgnore(const QModelIndex& parent, bool ignore)
{
	// ignore
	if (this->m_model->insertRow(4, parent))
	{
		const QModelIndex& child = this->m_model->index(4, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Ignore: ");
		item->setType(TYPE_GROUP_ITEM_IGNORE);
		item->setIgnore(ignore);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemComment(const QModelIndex& parent, bool comment)
{
	// comment
	if (this->m_model->insertRow(5, parent))
	{
		const QModelIndex& child = this->m_model->index(5, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Comment: ");
		item->setType(TYPE_GROUP_ITEM_COMMENT);
		item->setComment(comment);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemSearched(const QModelIndex& parent, bool searched)
{
	// searched
	if (this->m_model->insertRow(6, parent))
	{
		const QModelIndex& child = this->m_model->index(6, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Searched: ");
		item->setType(TYPE_GROUP_ITEM_SEARCHED);
		item->setSearched(searched);
		return child;
	}
	return QModelIndex();
}
QModelIndex CAoBTreeItemWidget::insertGroupItemResult(const QModelIndex& parent, unsigned long long result)
{
	// result
	if (this->m_model->insertRow(7, parent))
	{
		const QModelIndex& child = this->m_model->index(7, 0, parent);
		CAoBTreeItem* item = static_cast<CAoBTreeItem*>(child.internalPointer());
		item->setName("Result: ");
		item->setType(TYPE_GROUP_ITEM_RESULT);
		item->setResult(result);
		return child;
	}
	return QModelIndex();
}
