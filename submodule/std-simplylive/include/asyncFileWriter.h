#pragma once
#include <future>
#include "FrameList.h"
#include "asyncThread.h"

class AsyncFileWriter
{
public:
	AsyncFileWriter(const std::string& filePath, int32_t bufferSize, int32_t bufferCnt)
		: m_filePath(filePath), m_bufferSize(bufferSize), m_bufferCnt(bufferCnt)
	{
		for (int32_t i = 0; i < m_bufferCnt; i++)
			m_list.pushBackToEmptyList(std::make_shared<BufferItem>(bufferSize));
	}

	~AsyncFileWriter()
	{
		stop();
	}

	int32_t start()
	{
		if (m_hFile != nullptr)
		{
			fclose(m_hFile);
			m_hFile = nullptr;
		}

		const int32_t ret = fopen_s(&m_hFile, m_filePath.c_str(), "wb");
		if (ret != 0 || m_hFile == nullptr)
			return ret;

		m_hTask = asyncThread(threadPriority::lowest, std::string("AsyncFileWriter"), &AsyncFileWriter::taskThread,
		                      this);
		m_running = true;
		return 0;
	}

	void stop()
	{
		m_running = false;
		if (m_hTask.valid())
			m_hTask.join();
		if (m_hFile != nullptr)
			fclose(m_hFile);
		m_hFile = nullptr;
	}

	int32_t pushData(char* data, int dataSize)
	{
		spBufferItem emptyBufferItem;
		if (!m_list.popFrontFromEmptyList(emptyBufferItem))
			return -1;
		memcpy_s(emptyBufferItem->buffer.get(), m_bufferSize, data, dataSize);
		emptyBufferItem->usedSize = dataSize;
		m_list.pushBackToDataList(emptyBufferItem);
		return 0;
	}

	bool isRunning() const
	{
		return m_running;
	}

private:
	void taskThread()
	{
		spBufferItem dataBufferItem;
		while (m_running)
		{
			if (!m_list.waitForDataList(100))
				continue;

			if (!m_list.popFrontFromDataList(dataBufferItem))
				continue;

			fwrite(dataBufferItem->buffer.get(), 1, dataBufferItem->usedSize, m_hFile);
			m_list.pushBackToEmptyList(dataBufferItem);
			dataBufferItem = nullptr;
		}
		
		while (m_list.sizeOfDataList() > 0)
		{
			if (!m_list.popFrontFromDataList(dataBufferItem))
				continue;

			fwrite(dataBufferItem->buffer.get(), 1, dataBufferItem->usedSize, m_hFile);
			m_list.pushBackToEmptyList(dataBufferItem);
			dataBufferItem = nullptr;
		}
	}

	struct BufferItem
	{
		std::shared_ptr<uint8_t[]> buffer;
		uint32_t                   usedSize = 0;

		BufferItem() = default;

		explicit BufferItem(uint32_t bufferSize) :
			buffer(new uint8_t[bufferSize])
		{
		}
	};

	using spBufferItem = std::shared_ptr<BufferItem>;
	std::string             m_filePath;
	int32_t                 m_bufferSize;
	int32_t                 m_bufferCnt;
	FILE*                   m_hFile = nullptr;
	threadHandle            m_hTask;
	bool                    m_running = false;
	FrameList<spBufferItem> m_list;
};
