#include "stdafx.h"

#include "CProcessDialog.hpp"
#include <TlHelp32.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

CProcessDialog::CProcessDialog(QWidget *parent) : QDialog(parent)
{
	this->m_listWidget = new QListWidget(this);
	this->m_listWidget->setSortingEnabled(false);
	this->m_listWidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

	QPushButton *buttonOpen = new QPushButton("Open", this);
	QPushButton *buttonCancel = new QPushButton("Cancel", this);
	QPushButton *buttonRefresh = new QPushButton("Refresh", this);

	QHBoxLayout *hboxLayout = new QHBoxLayout;
	hboxLayout->addWidget(buttonOpen);
	hboxLayout->addWidget(buttonCancel);
	hboxLayout->addWidget(buttonRefresh);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(this->m_listWidget, 0, 0);
	mainLayout->addLayout(hboxLayout, 1, 0);
	this->setLayout(mainLayout);

	QObject::connect(buttonOpen, &QPushButton::clicked, this, &CProcessDialog::OnOpenProcess);
	QObject::connect(buttonCancel, &QPushButton::clicked, this, &CProcessDialog::OnCancel);
	QObject::connect(buttonRefresh, &QPushButton::clicked, this, &CProcessDialog::OnRefresh);
	QObject::connect(this->m_listWidget, &QListWidget::doubleClicked, this, &CProcessDialog::OnDoubleClicked);

	// refresh the items
	this->OnRefresh();
}

unsigned long CProcessDialog::GetSelectedProcessId() const
{
	return this->m_dwProcessId;
}

void CProcessDialog::OnOpenProcess(bool checked)
{
	foreach(const QListWidgetItem * item, this->m_listWidget->selectedItems())
	{
		// single selection
		this->m_dwProcessId = item->data(Qt::UserRole).toInt();
		this->accept();
		return;
	}
}

void CProcessDialog::OnCancel(bool checked)
{
	this->reject();
}

BOOL CProcessDialog::AddProcessItems(QListWidget *listWidget)
{
	// Take a snapshot of all processes in the system.
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//
		return FALSE;
	}

	// Set the size of the structure before using it.
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return FALSE;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	QFileIconProvider iconProvider;
	do
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess != NULL)
		{
			// pid-pname
			CHAR szFilename[MAX_PATH];
			if (GetModuleFileNameExA(hProcess, NULL, szFilename, MAX_PATH))
			{
				QString text = QString("%1-%2").
					arg(QString::number(pe32.th32ProcessID, 16).rightJustified(8, '0')).
					arg(QString::fromWCharArray(pe32.szExeFile));
				QListWidgetItem *item = new QListWidgetItem(iconProvider.icon(QFileInfo(szFilename)), text, listWidget);
				uint uProcessID = pe32.th32ProcessID;
				item->setData(Qt::UserRole, uProcessID);

				// add item
				listWidget->addItem(item);
			}
			CloseHandle(hProcess);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return TRUE;
}

void CProcessDialog::OnRefresh(bool checked)
{
	// clear all items
	this->m_listWidget->clear();
	this->AddProcessItems(this->m_listWidget);
	this->m_listWidget->scrollToBottom();
}

void CProcessDialog::OnDoubleClicked(const QModelIndex &index)
{
	if (!index.isValid())
		return;

	this->m_dwProcessId = index.data(Qt::UserRole).toInt();
	this->accept();
}