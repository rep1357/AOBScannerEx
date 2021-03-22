#pragma once

#include "CSingleTreeItem.hpp"

enum //ITEM_TYPE
{
	TYPE_GROUP = 100,
	TYPE_GROUP_ITEM,
};

enum //ITEM_GROUP_TYPE
{
	// CAoBTreeItem::setType/type
	TYPE_GROUP_ITEM_PATTERN  = 0,
	TYPE_GROUP_ITEM_METHOD	 = 1,
	TYPE_GROUP_ITEM_ORDER	 = 2,
	TYPE_GROUP_ITEM_OFFSET	 = 3,
	TYPE_GROUP_ITEM_IGNORE	 = 4,
	TYPE_GROUP_ITEM_COMMENT  = 5,
	TYPE_GROUP_ITEM_SEARCHED = 6,
	TYPE_GROUP_ITEM_RESULT	 = 7,
	//
	TYPE_GROUP_ITEM_COUNT	 = 8,
};

enum SEARCH_METHOD
{
	METHOD_NORMAL,
	METHOD_CALL,
	METHOD_PTR1,
	METHOD_PTR4,
	METHOD_FUNCTION_START,
};

//
QString get_bool_string(bool b);
QString get_method_string(int method);

//
QList<QPair<QString, QVariant>> get_method_items();

class CAoBTreeItem : public CSingleTreeItem
{
public:
	explicit CAoBTreeItem(CAoBTreeItem* parent = nullptr);
	~CAoBTreeItem() = default;
	
public:
	// ITEM_TYPE
	void setType(int group);
	int type() const;

	void setPattern(const QString& pattern);
	QString pattern() const;

	// SEARCH_METHOD
	void setMethod(int type);
	int method() const;

	void setOrder(int order);
	int order() const;

	void setOffset(int offset);
	int offset() const;

	void setIgnore(bool ignoreFlag);
	bool ignore() const;

	void setComment(bool commentFlag);
	bool comment() const;

	void setSearched(bool searched);
	bool searched() const;

	void setResult(unsigned long long result);
	unsigned long long result() const;
};