#pragma once

class CSingleTreeItem
{
public:
	explicit CSingleTreeItem(CSingleTreeItem *parent = nullptr);
	explicit CSingleTreeItem(const CSingleTreeItem &other);
	virtual ~CSingleTreeItem();

	// operator
	CSingleTreeItem &operator=(const CSingleTreeItem &other);

public:
	void deleteAll();

	int row() const;
	int childCount() const;
	int childPos(CSingleTreeItem* item) const;
	void insertChild(CSingleTreeItem* item);
	void removeChild(int row);
	void swapChildren(int targetRow, int destRow);
	CSingleTreeItem *copy(const CSingleTreeItem &child, CSingleTreeItem* const parent) const &;

	CSingleTreeItem *parent() const;
	CSingleTreeItem *child(int row) const;
	int depth() const;
	QList<CSingleTreeItem *> children() const &;

	void setName(const QString &name);
#if _Q_NO_CUSTOMTREEMODEL
	void setIcon(const QIcon &icon);
	void setIcon(const QString &filename, const QSize &s, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
		Qt::TransformationMode mode = Qt::FastTransformation);
	void setIcon(const QPixmap &pixmap, const QSize &s, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
		Qt::TransformationMode mode = Qt::FastTransformation);
	void setBackColor(const QColor &color);
	void setTextColor(const QColor &color);

	QIcon icon() const;
	QColor backColor() const;
	QColor textColor() const;
#endif
	QString name() const;

	void setUserData(int i, const QVariant& data);
	QVariant userData(int i) const;

private:
	void setParent(CSingleTreeItem *parent);
	void setDepth(int depth);

private:
	QList<CSingleTreeItem*> m_items;
	CSingleTreeItem* m_parent;
	QString	m_name;
	QMap<int, QVariant> m_userData;
#if _Q_NO_CUSTOMTREEMODEL
	QIcon	m_icon;
	QColor	m_backColor;
	QColor	m_textColor;
#endif
	int		m_depth;
};