#include "stdafx.h"

#include "CIntDelegate.hpp"

CIntDelegate::CIntDelegate(int min, int max, QObject *parent) : QItemDelegate(parent)
{
	this->m_min = min;
	this->m_max = max;
}

QWidget *CIntDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QLineEdit *lineEdit = new QLineEdit(parent);
	QIntValidator *validator = new QIntValidator(this->m_min, this->m_max, lineEdit);
	lineEdit->setValidator(validator);
	return lineEdit;
}
