#pragma once

QT_BEGIN_NAMESPACE
class CAoBTreeItemWidget;
class CAoBTreeItemsDetailWidget;
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CMainWindow(QWidget *parent = nullptr);
	~CMainWindow(); 

	CAoBTreeItemWidget* aobTree() const;
	CAoBTreeItemsDetailWidget* aobTreeItemsDetail() const;

private:
	void closeEvent(QCloseEvent* event);
	void saveSettings();
	void loadSettings();

private slots:
	void OnAbout();

private:
	CAoBTreeItemWidget* m_aobTree;
	CAoBTreeItemsDetailWidget* m_aobTreeItemsDetail;
	QSplitter* m_splitter;
};