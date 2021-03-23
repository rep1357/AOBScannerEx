#include "stdafx.h"

#include "CMainWindow.hpp"
#include "CAoBTreeItemModel.hpp"
#include "CAoBTreeItemWidget.hpp"
#include "CAoBTreeItemsDetailModel.hpp"
#include "CAoBTreeItemsDetailWidget.hpp"

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent)
{
	this->setWindowTitle("Scanner");
	this->setMinimumSize(600, 400);

	// aob tree / detail view
	this->m_itemWidget = new CAoBTreeItemWidget(this);
	this->m_itemDetailWidget = new CAoBTreeItemsDetailWidget(this->m_itemWidget, this);

	// central widget
	this->m_splitter = new QSplitter(Qt::Horizontal, this);
	this->m_splitter->addWidget(this->m_itemWidget);
	this->m_splitter->addWidget(this->m_itemDetailWidget);
	this->m_splitter->setSizes({
		(this->width() / 4) * 1,
		(this->width() / 4) * 3 }
	);
	this->setCentralWidget(this->m_splitter);

	// menu
	QMenu* fileMenu = new QMenu(tr("File"));
	QMenu* toolsMenu = new QMenu(tr("Tools"));
	QMenu* helpMenu = new QMenu(tr("Help"));
	this->menuBar()->addMenu(fileMenu);
	this->menuBar()->addMenu(toolsMenu);
	this->menuBar()->addMenu(helpMenu);

	// add actions
	fileMenu->addAction(QIcon(":/images/Computer-64.png"), tr("&Open Process"), this->m_itemWidget, &CAoBTreeItemWidget::OnOpenProcess);
	fileMenu->addSeparator();
	fileMenu->addAction(QIcon::fromTheme("file-open", QIcon(":/images/Open Folder-64.png")), tr("&Load"), this->m_itemWidget, &CAoBTreeItemWidget::OnLoad, QKeySequence::Open);
	fileMenu->addAction(QIcon::fromTheme("file-save", QIcon(":/images/Save-64.png")), tr("&Save"), this->m_itemWidget, &CAoBTreeItemWidget::OnSave, QKeySequence::Save);
	fileMenu->addAction(QIcon::fromTheme("file-saveas", QIcon(":/images/Save as-64.png")), tr("&Save As..."), this->m_itemWidget, &CAoBTreeItemWidget::OnSaveAs, QKeySequence::SaveAs);
	fileMenu->addSeparator();
	fileMenu->addAction(QIcon::fromTheme("file-quit", QIcon(":/images/Close Window-64.png")), tr("&Quit"), this, SLOT(close()), Qt::CTRL + Qt::Key_Q);

	toolsMenu->addAction(QIcon(), tr("&Export To File"), this->m_itemWidget, &CAoBTreeItemWidget::OnExport);

	helpMenu->addAction(QIcon::fromTheme("help-about", QIcon(":/images/About-64.png")), tr("&About"), this, SLOT(OnAbout()));
	helpMenu->addAction(style()->standardIcon(QStyle::SP_TitleBarMenuButton), tr("About &Qt"), qApp, SLOT(aboutQt()));

	// connect
	QObject::connect(this->m_itemWidget, &CAoBTreeItemWidget::handleShowDetail, this->m_itemDetailWidget, &CAoBTreeItemsDetailWidget::OnShowDetail);
	QObject::connect(this->m_itemWidget, &CAoBTreeItemWidget::windowTitleChanged, this, &CMainWindow::setWindowTitle);
	QObject::connect(this->m_itemWidget->model(), &CAoBTreeItemModel::updateReady, this->m_itemDetailWidget->model(), &CAoBTreeItemsDetailModel::update);
	QObject::connect(this->m_itemDetailWidget->model(), &CAoBTreeItemsDetailModel::updateReady, this->m_itemWidget->model(), &CAoBTreeItemModel::update);

	// load
	this->loadSettings();
}

CMainWindow::~CMainWindow()
{
	this->saveSettings();
}

void CMainWindow::closeEvent(QCloseEvent* event)
{
	event->accept();
}

void CMainWindow::saveSettings()
{
	QSettings settings;
	settings.setValue("windowsize", this->size());
	settings.setValue("windowpos", this->pos());
	settings.setValue("splitleft", this->m_splitter->sizes()[0]);
	settings.setValue("splitright", this->m_splitter->sizes()[1]);
}
void CMainWindow::loadSettings()
{
	QSettings settings;

	QVariant size = settings.value("windowsize");
	if (size.isValid())
		this->resize(size.toSize());

	QVariant pos = settings.value("windowpos");
	if (pos.isValid())
		this->move(pos.toPoint());

	QVariant splitleft = settings.value("splitleft");
	QVariant splitright = settings.value("splitright");
	if (splitleft.isValid() && splitright.isValid())
		this->m_splitter->setSizes({ splitleft.toInt(), splitright.toInt() });
}

void CMainWindow::OnAbout()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(tr("About"));
	msgBox.setText(
		"<b>AobScannerEx</b><br>" \
		"<br>" \
		"Credit to vm-enter");
	msgBox.setTextFormat(Qt::RichText);
	msgBox.exec();
}
