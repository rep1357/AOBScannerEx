#pragma once

// QSingleton
template <class T>
class QSingleton
{
	// QSingleton_impl
	class QSingleton_impl
	{
		friend class QSingleton<T>;

		static T *m_instance;

		QSingleton_impl()
		{
			this->m_instance = new T;
		}

		~QSingleton_impl()
		{
			this->_deleteInstance();
		}

		T *data()
		{
			return this->m_instance;
		}

		operator T *()
		{
			return this->m_instance;
		}

		void _deleteInstance(QObject *)
		{
			if (this->m_instance)
			{
				this->m_instance->deleteLater();
				this->m_instance = nullptr;
			}
		}

		void _deleteInstance(void *)
		{
			if (this->m_instance)
			{
				delete this->m_instance;
				this->m_instance = nullptr;
			}
		}

		void _deleteInstance()
		{
			this->_deleteInstance((T *)nullptr);
		}
	};

public:
	static T *getInstance()
	{
		static QSingleton_impl s_instance;
		return s_instance;
	}

protected:
	QSingleton()
	{
	}

	~QSingleton()
	{
	}

private:
	QSingleton(const QSingleton &) = delete;
	QSingleton &operator=(const QSingleton &) = delete;
	QSingleton(QSingleton &&) = delete;
	QSingleton &operator=(QSingleton &&) = delete;
};

// impl
template <class T>
T *QSingleton<T>::QSingleton_impl::m_instance = nullptr;