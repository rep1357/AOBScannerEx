#pragma once

class CProcessDialog : public QDialog
{
	Q_OBJECT

public:
	CProcessDialog(QWidget *parent = nullptr);
	unsigned long GetSelectedProcessId() const;

private:
	BOOL AddProcessItems(QListWidget *listWidget);

private slots:
	void OnOpenProcess(bool checked = false);
	void OnCancel(bool checked = false);
	void OnRefresh(bool checked = false);
	void OnDoubleClicked(const QModelIndex &index);

private:
	QListWidget *m_listWidget;
	DWORD m_dwProcessId;
};