#include "stdafx.h"

#include "CFindPatternWorker.hpp"
#include "CAoBTreeItem.hpp"

#include "findpattern.hpp"

CFindPatternWorker::CFindPatternWorker(QObject *parent) : QObject(parent)
{
}
CFindPatternWorker::~CFindPatternWorker()
{
}

void CFindPatternWorker::doWork(const FindPatternParam& param)
{
	// params

	std::string AoB, AoB_FS;
	if (param.type() == METHOD_FUNCTION_START)
	{
		const QStringList patterns = param.arrayOfBytes().split(",");
		if (patterns.size() == 2)
		{
			AoB = patterns[0].toStdString();
			AoB_FS = patterns[1].toStdString();
		}
	}
	else
		AoB = param.arrayOfBytes().toStdString();

	const int order = param.order() > 0 ? param.order() : 1;

	const unsigned long long moduleBaseAddress = param.moduleBaseAddress();
	const unsigned long bufferSize = param.bufferSize();

	const void *bufferForLocal = param.moduleBuffer();
	const void *bufferEnd = reinterpret_cast<const char *>(bufferForLocal) + bufferSize;

	const void *searchBuffer = bufferForLocal;
	unsigned long remainingSize = bufferSize;
	const void *resultBuffer = nullptr;
	for (int n = 0; n < order; n++)
	{
		// n result
		if (n != 0)
		{
			searchBuffer = PBYTE(resultBuffer) + 1;
			remainingSize = ULONG_PTR(bufferEnd) - ULONG_PTR(searchBuffer);
		}

		resultBuffer = FindPatternA(searchBuffer, remainingSize, AoB.c_str());
		if (resultBuffer == NULL)
		{
			// fail
			emit this->resultReady(param.index(), 0);
			return;
		}
	}

	const unsigned long long delta = moduleBaseAddress - (unsigned long long)bufferForLocal;
	unsigned long long result = (unsigned long long)resultBuffer + param.offset();
	switch (param.type())
	{
		case METHOD_NORMAL:
		{
			// fix it up
			result = result + delta;
			break;
		}
		case METHOD_CALL:
		{
			// for x86 only?
			const int callOffset = *(unsigned long *)(result + 1);
			result = result + delta + callOffset + 5;
			break;
		}
		case METHOD_PTR1:
		{
			void *ptr = reinterpret_cast<char *>(result);
			if (bufferForLocal <= ptr && ptr < bufferEnd)
			{
				result = *(unsigned char *)(ptr);
			}
			else
			{
				// invalid ptr
				result = 0;
			}
			break;
		}
		case METHOD_PTR4:
		{
			void *ptr = reinterpret_cast<char *>(result);
			if (bufferForLocal <= ptr && ptr < bufferEnd)
			{
				result = *(unsigned long *)(ptr);
			}
			else
			{
				// invalid ptr
				result = 0;
			}
			break;
		}
		case METHOD_FUNCTION_START:
		{
			if (!AoB_FS.empty())
			{
				searchBuffer = resultBuffer;
				for (int i = AoB_FS.length(); i < param.offset(); i++)
				{
					resultBuffer = FindPatternA(PBYTE(searchBuffer) - i, AoB_FS.length(), AoB_FS.c_str());
					if (resultBuffer != NULL)
					{
						result = (unsigned long long)resultBuffer + delta;
						break;
					}
				}
			}
			else
			{
				result = 0;
			}
			break;
		}
		default:
		{
			result = 0;
			break;
		}
	}	

	emit this->resultReady(param.index(), result);
}