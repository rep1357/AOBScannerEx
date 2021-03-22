#pragma once

QT_BEGIN_NAMESPACE
class CMainWindow;
class CAoBTreeItem;
class CAoBTreeItemModel;
class CFindPatternWorker;
class QTreeView;
QT_END_NAMESPACE

class CAoBTreeItemWidget : public QScrollArea
{
	friend class CMainWindow;

	Q_OBJECT

	struct Worker
	{
		QThread *thread;
		CFindPatternWorker *worker;
		int requestCount;
	};

public:
	explicit CAoBTreeItemWidget(QWidget* parent = nullptr);
	~CAoBTreeItemWidget();

	CAoBTreeItemModel* model() const;
	QTreeView* view() const;

	CAoBTreeItem* treeItem(int row, const QModelIndex& index) const;

private:
	void updateDetailWidget();

	bool isGroupIndex(const QModelIndex& index);
	bool isGroupItemIndex(const QModelIndex& index);

	bool insertRowItems(const QModelIndex& parent);

	// settings
	void writeXmlElement(QXmlStreamWriter& writer);
	void readFromXml(QXmlStreamReader& reader);
	void readXmlGroup(QXmlStreamReader& reader, const QModelIndex& parent);
	void readXmlGroupItem(QXmlStreamReader& reader, const QModelIndex& parent);
	//
	QModelIndex insertGroup(const QString& groupName);
	QModelIndex insertGroupItem(const QString& groupItemName, const QModelIndex& parent);
	//
	QModelIndex insertGroupItemPattern(const QModelIndex& parent, const QString& pattern = QString());
	QModelIndex insertGroupItemMethod(const QModelIndex& parent, int method = 0);
	QModelIndex insertGroupItemOrder(const QModelIndex& parent, int order = 1);
	QModelIndex insertGroupItemOffset(const QModelIndex& parent, int offset = 0);
	QModelIndex insertGroupItemIgnore(const QModelIndex& parent, bool ignore = false);
	QModelIndex insertGroupItemComment(const QModelIndex& parent, bool comment = false);
	QModelIndex insertGroupItemSearched(const QModelIndex& parent, bool searched = false);
	QModelIndex insertGroupItemResult(const QModelIndex& parent, unsigned long long result = 0);

signals:
	// CMainWindow::m_aobTreeItemDetail -> CAoBTreeItemDetailWidget::OnShowDetail (defined at CMainWindow constructor)
	void handleShowDetail(const QModelIndex& index);

private slots:
	void OnContextMenuRequested(const QPoint& pos);
	void OnTreeItemSingleClicked(const QModelIndex& index);
	void OnTreeItemDoubleClicked(const QModelIndex& index);
	void OnSelectionCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
	void OnLoad();
	void OnSave();
	void OnSaveAs();
	void OnOpenProcess();
	void OnExport();
	void OnScan();
	void OnScanAll();
	void OnInsertRow();
	void OnInsertRowItem();
	void OnRemoveRow();
	void OnRemoveRowItem();

private:
	void scan(const QModelIndex& index);
	void scanAll();

public slots:
	void OnHandleResults(const QModelIndex& index, unsigned long long result);

private:
	CAoBTreeItemModel* m_model;
	QTreeView* m_view;
	QModelIndex m_detailIndex;

	QString m_filename;

	LPVOID m_moduleBuffer;
	LPVOID m_moduleBaseAddress;
	DWORD m_dwSizeOfImage;

	//
	QList<struct Worker> m_workers;
};