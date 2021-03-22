#include "stdafx.h"

#include "CAoBTreeItem.hpp"

QString get_bool_string(bool b)
{
	return b ? "true" : "false";
}

QString get_method_string(int method)
{
	switch (method)
	{
		case METHOD_NORMAL: return "NORMAL";
		case METHOD_CALL: return "CALL";
		case METHOD_PTR1: return "PTR1";
		case METHOD_PTR4: return "PTR4";
		case METHOD_FUNCTION_START: return "FUNCTION START";
	}
	return QString();
}

QList<QPair<QString, QVariant>> get_method_items()
{
	QList<QPair<QString, QVariant>> items;
	items << QPair<QString, QVariant>("NORMAL", METHOD_NORMAL)
		<< QPair<QString, QVariant>("CALL", METHOD_CALL)
		<< QPair<QString, QVariant>("PTR1", METHOD_PTR1)
		<< QPair<QString, QVariant>("PTR4", METHOD_PTR4)
		<< QPair<QString, QVariant>("FUNCTION START", METHOD_FUNCTION_START);
	return items;
}

CAoBTreeItem::CAoBTreeItem(CAoBTreeItem* parent) : CSingleTreeItem(parent)
{
}

void CAoBTreeItem::setType(int type)
{
	this->setUserData(0, type);
}
int CAoBTreeItem::type() const
{
	return this->userData(0).toInt();
}

void CAoBTreeItem::setPattern(const QString& pattern)
{
	this->setUserData(1, pattern);
}
QString CAoBTreeItem::pattern() const
{
	return this->userData(1).toString();
}

void CAoBTreeItem::setMethod(int method)
{
	this->setUserData(2, method);
}
int CAoBTreeItem::method() const
{
	return this->userData(2).toInt();
}

void CAoBTreeItem::setOrder(int order)
{
	this->setUserData(3, order);
}
int CAoBTreeItem::order() const
{
	return this->userData(3).toInt();
}

void CAoBTreeItem::setOffset(int offset)
{
	this->setUserData(4, offset);
}
int CAoBTreeItem::offset() const
{
	return this->userData(4).toInt();
}

void CAoBTreeItem::setIgnore(bool ignore)
{
	this->setUserData(5, ignore);
}
bool CAoBTreeItem::ignore() const
{
	return this->userData(5).toBool();
}

void CAoBTreeItem::setComment(bool comment)
{
	this->setUserData(6, comment);
}
bool CAoBTreeItem::comment() const
{
	return this->userData(6).toBool();
}

void CAoBTreeItem::setSearched(bool searched)
{
	this->setUserData(7, searched);
}
bool CAoBTreeItem::searched() const
{
	return this->userData(7).toBool();
}

void CAoBTreeItem::setResult(unsigned long long result)
{
	this->setUserData(8, result);
}
unsigned long long CAoBTreeItem::result() const
{
	return this->userData(8).toULongLong();
}