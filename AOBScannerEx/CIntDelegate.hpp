#pragma once

class CIntDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	explicit CIntDelegate(int min = 0, int max = 99, QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
	int m_max, m_min;
};