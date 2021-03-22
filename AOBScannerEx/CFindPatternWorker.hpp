#pragma once

class FindPatternParam
{
public:
	FindPatternParam() {}
	FindPatternParam(const QModelIndex& index, unsigned long long baseAddr, void *buffer, unsigned long size,
		const QString &aobString, int type, int order, int offset)
	{
		this->m_index = index;
		this->m_moduleBaseAddress = baseAddr;
		this->m_moduleBuffer = buffer;
		this->m_bufferSize = size;
		this->m_arrayOfBytes = aobString;
		this->m_type = type;
		this->m_order = order;
		this->m_offset = offset;
	}
	~FindPatternParam() {}

	QModelIndex index() const { return this->m_index; }

	unsigned long long moduleBaseAddress() const { return this->m_moduleBaseAddress; }
	void *moduleBuffer() const { return this->m_moduleBuffer; }

	QString arrayOfBytes() const { return this->m_arrayOfBytes; }
	unsigned long bufferSize() const { return this->m_bufferSize; }

	int type() const { return this->m_type; }
	int offset() const { return this->m_offset; }
	int order() const { return this->m_order; }

private:
	//
	QModelIndex m_index;

	// xD!
	unsigned long long m_moduleBaseAddress;		// HMODULE
	void *m_moduleBuffer;			// for local
	unsigned long m_bufferSize;		// from NTHeader

	// input
	QString m_arrayOfBytes;
	int m_type, m_offset, m_order;
};

Q_DECLARE_METATYPE(FindPatternParam);

class CFindPatternWorker : public QObject
{
	Q_OBJECT

public:
	CFindPatternWorker(QObject *parent = nullptr);
	~CFindPatternWorker();

public slots:
	void doWork(const FindPatternParam& param);

signals:
	void resultReady(const QModelIndex& index, unsigned long long result);
};
