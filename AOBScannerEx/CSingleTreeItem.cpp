#include "stdafx.h"

#include "CSingleTreeItem.hpp"

CSingleTreeItem::CSingleTreeItem(CSingleTreeItem *parent)
	: m_parent(parent)
	, m_name("")
#if _Q_NO_CUSTOMTREEMODEL
	, m_backColor(QColor())
	, m_textColor(QColor())
#endif
{
	if (!parent)
		this->setDepth(0);
	else
		this->setDepth(parent->depth() + 1);
}
CSingleTreeItem::CSingleTreeItem(const CSingleTreeItem &other)
{
	this->m_parent = other.parent();
	this->m_depth = other.depth();
	this->m_name = other.name();
#if _Q_NO_CUSTOMTREEMODEL
	this->m_icon = other.icon();
	this->m_backColor = other.backColor();
	this->m_textColor = other.textColor();
#endif
	for (auto it = other.children().begin(); it != other.children().end(); it++)
	{
		const CSingleTreeItem* i = *it;
		CSingleTreeItem* newItem;
		if (i->childCount())
			newItem = this->copy(*i, this->m_parent);
		else
		{
			newItem = new CSingleTreeItem(this->m_parent);
			newItem->setDepth(i->depth());
			newItem->setName(i->name());
#if _Q_NO_CUSTOMTREEMODEL
			newItem->setIcon(i->icon());
			newItem->setBackColor(i->backColor());
			newItem->setTextColor(i->textColor());
#endif
		}
		this->insertChild(newItem);
	}
}
CSingleTreeItem::~CSingleTreeItem()
{
	this->deleteAll();
}
CSingleTreeItem& CSingleTreeItem::operator=(const CSingleTreeItem &other)
{
	if (this != &other)
	{
		this->m_parent = other.parent();
		this->m_depth = other.depth();
		this->m_name = other.name();
#if _Q_NO_CUSTOMTREEMODEL
		this->m_icon = other.icon();
		this->m_backColor = other.backColor();
		this->m_textColor = other.textColor();
#endif
		this->deleteAll();
		this->m_items = other.children();
	}
	return *this;
}

int CSingleTreeItem::row() const
{
	if (!this->m_parent)
		return 0;

	return this->m_parent->childPos(const_cast<CSingleTreeItem *>(this));
}
int CSingleTreeItem::childCount() const
{
	return this->m_items.size();
}
int CSingleTreeItem::childPos(CSingleTreeItem* item) const
{
	return this->m_items.indexOf(item);
}
void CSingleTreeItem::insertChild(CSingleTreeItem* item)
{
	this->m_items.append(item);
}
void CSingleTreeItem::removeChild(int row)
{
	auto it = this->m_items.begin() + row;
	if (it != this->m_items.end())
	{
		CSingleTreeItem *item = *it;
		delete item;
		this->m_items.erase(it);
	}
}

void CSingleTreeItem::swapChildren(int targetRow, int destRow)
{
	Q_ASSERT(targetRow >= 0 && targetRow < this->m_items.count());
	Q_ASSERT(destRow >= 0 && destRow < this->m_items.count());
	qSwap(this->m_items[targetRow], this->m_items[destRow]);
}

CSingleTreeItem* CSingleTreeItem::copy(const CSingleTreeItem &child, CSingleTreeItem* const parent) const &
{
	CSingleTreeItem* item = new CSingleTreeItem(parent);
	item->setDepth(child.depth());
	item->setName(child.name());
#if _Q_NO_CUSTOMTREEMODEL
	item->setIcon(child.icon());
	item->setBackColor(child.backColor());
	item->setTextColor(child.textColor());
#endif
	for (auto it = child.children().begin(); it != child.children().end(); it++)
	{
		const CSingleTreeItem* i = *it;
		CSingleTreeItem* newItem;
		if (i->childCount() > 0)
			newItem = this->copy(*i, item);
		else
		{
			newItem = new CSingleTreeItem(item);
			newItem->setDepth(i->depth());
			newItem->setName(i->name());
#if _Q_NO_CUSTOMTREEMODEL
			newItem->setIcon(i->icon());
			newItem->setBackColor(i->backColor());
			newItem->setTextColor(i->textColor());
#endif
		}
		item->insertChild(newItem);
	}
	return item;
}

CSingleTreeItem *CSingleTreeItem::parent() const
{
	return this->m_parent;
}
CSingleTreeItem *CSingleTreeItem::child(int row) const
{
	if (row < 0 || this->m_items.size() <= row)
		return nullptr;

	return this->m_items[row];
}
int CSingleTreeItem::depth() const
{
	return this->m_depth;
}
QString CSingleTreeItem::name() const
{
	return this->m_name;
}
#if _Q_NO_CUSTOMTREEMODEL
QIcon CSingleTreeItem::icon() const
{
	return this->m_icon;
}
QColor CSingleTreeItem::backColor() const
{
	return this->m_backColor;
}
QColor CSingleTreeItem::textColor() const
{
	return this->m_textColor;
}
#endif
QList<CSingleTreeItem *> CSingleTreeItem::children() const &
{
	return this->m_items;
}

void CSingleTreeItem::setParent(CSingleTreeItem *parent)
{
	this->m_parent = parent;
}
void CSingleTreeItem::setDepth(int depth)
{
	this->m_depth = depth;
}
void CSingleTreeItem::setName(const QString &name)
{
	this->m_name = name;
}
#if _Q_NO_CUSTOMTREEMODEL
void CSingleTreeItem::setIcon(const QIcon &icon)
{
	this->m_icon = icon;
}
void CSingleTreeItem::setIcon(const QString &filename, const QSize &s, Qt::AspectRatioMode aspectMode, Qt::TransformationMode mode)
{
	this->m_icon = QIcon(QPixmap(filename).scaled(s, aspectMode, mode));
}
void CSingleTreeItem::setIcon(const QPixmap &pixmap, const QSize &s, Qt::AspectRatioMode aspectMode, Qt::TransformationMode mode)
{
	this->m_icon = QIcon(pixmap.scaled(s, aspectMode, mode));
}
void CSingleTreeItem::setBackColor(const QColor &color)
{
	this->m_backColor = color;
}
void CSingleTreeItem::setTextColor(const QColor &color)
{
	this->m_textColor = color;
}
#endif

void CSingleTreeItem::setUserData(int i, const QVariant& data)
{
	this->m_userData[i] = data;
}
QVariant CSingleTreeItem::userData(int i) const
{
	return this->m_userData[i];
}

void CSingleTreeItem::deleteAll()
{
	//printf("called\n");
	if (this->m_items.size() > 0)
		qDeleteAll(this->m_items);

	this->m_items.clear();
}