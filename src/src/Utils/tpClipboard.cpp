#include "tpClipboard.h"
#include <mutex>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <tpString.h>

#ifndef CLIPBOARD_SHARED_SIZE
#define CLIPBOARD_SHARED_SIZE 4096
#endif

#ifndef CLIPBOARD_NAME
#define CLIPBOARD_NAME "tinyPiXOSClipboard"
#endif

// 共享内存内 数据头结构
struct SharedMemoryHeader
{
	uint32_t magic = 0;		// 魔数校验
	uint32_t data_size = 0; // 实际数据长度
	uint32_t checksum = 0;	// CRC32校验码
	uint32_t version = 0;	// 版本号
};

struct tpClipboardData
{
	int fileDevice = 0;
	std::mutex gMutex;

	// 缓冲区实际分配大小
	uint64_t allocatedSize = CLIPBOARD_SHARED_SIZE;
};

// 简易CRC16实现（替代CRC32）
uint16_t calculateChecksum(const char *data, uint64_t len)
{
	const uint16_t polynomial = 0x8005;
	uint16_t crc = 0xFFFF;

	for (size_t i = 0; i < len; ++i)
	{
		crc ^= (static_cast<uint16_t>(data[i]) << 8);
		for (int j = 0; j < 8; ++j)
		{
			if (crc & 0x8000)
			{
				crc = (crc << 1) ^ polynomial;
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	return crc;
}

// 兼容原有uint32_t类型的校验存储
uint32_t calculateChecksumWrapper(const char *data, uint64_t len)
{
	return static_cast<uint32_t>(calculateChecksum(data, len));
}

tpClipboard *tpClipboard::Inst()
{
	static tpClipboard *clipboard = nullptr;
	if (clipboard == nullptr)
	{
		clipboard = new tpClipboard();
	}

	return clipboard;
}

tpClipboard::tpClipboard()
{
	tpClipboardData *clipData = new tpClipboardData();
	data_ = clipData;

	// 改进：添加错误重试机制
	for (int retry = 0; retry < 3; ++retry)
	{
		clipData->fileDevice = shm_open(CLIPBOARD_NAME, O_CREAT | O_RDWR, 0666);
		if (clipData->fileDevice != -1)
			break;
		usleep(100000); // 100ms重试间隔
	}

	if (clipData->fileDevice == -1)
	{
		perror("剪切板缓冲区创建失败！");
		delete clipData;
		data_ = nullptr;
		return;
	}

	// 原子化设置大小
	std::lock_guard<std::mutex> lock(clipData->gMutex);
	if (ftruncate(clipData->fileDevice, CLIPBOARD_SHARED_SIZE) == -1)
	{
		perror("共享内存初始化失败");
		close(clipData->fileDevice);
		delete clipData;
		data_ = nullptr;
	}
}

tpClipboard::~tpClipboard()
{
	tpClipboardData *clipData = static_cast<tpClipboardData *>(data_);
	if (clipData)
	{
		// 安全清理资源
		if (clipData->fileDevice != -1)
		{
			close(clipData->fileDevice);
			shm_unlink(CLIPBOARD_NAME);
		}
		delete clipData;
	}
}

void tpClipboard::setText(const tpString &text)
{
	tpClipboardData *clipData = static_cast<tpClipboardData *>(data_);
	if (!clipData || text.empty())
		return;

	std::lock_guard<std::mutex> lock(clipData->gMutex);

	// 计算需要总空间（数据头 + 实际数据）
	const uint64_t requiredSize = sizeof(SharedMemoryHeader) + text.length();

	// 动态扩展内存（按2倍增长策略）
	if (requiredSize > clipData->allocatedSize)
	{
		uint64_t newSize = clipData->allocatedSize;
		while (newSize < requiredSize)
			newSize *= 2;

		if (ftruncate(clipData->fileDevice, newSize) == -1)
		{
			perror("共享内存扩展失败");
			return;
		}
		clipData->allocatedSize = newSize;
	}

	// 映射内存
	char *ptr = (char *)mmap(NULL, clipData->allocatedSize,
							 PROT_READ | PROT_WRITE,
							 MAP_SHARED, clipData->fileDevice, 0);
	if (ptr == MAP_FAILED)
	{
		perror("内存映射失败");
		return;
	}

	// 构造数据头
	SharedMemoryHeader header;

	header.magic = 0x54494E59;
	header.data_size = static_cast<uint32_t>(text.length());
	header.checksum = calculateChecksumWrapper(text.c_str(), text.length());
	header.version = 1;

	// 写入数据
	memcpy(ptr, &header, sizeof(header));
	memcpy(ptr + sizeof(header), text.c_str(), text.length());

	// 确保数据持久化
	if (msync(ptr, sizeof(header) + text.length(), MS_SYNC) == -1)
	{
		perror("数据同步失败");
	}

	munmap(ptr, clipData->allocatedSize);
}

tpString tpClipboard::text()
{
	tpClipboardData *clipData = static_cast<tpClipboardData *>(data_);
	if (!clipData)
		return "";

	std::lock_guard<std::mutex> lock(clipData->gMutex);

	// 映射内存
	char *ptr = (char *)mmap(NULL, clipData->allocatedSize,
							 PROT_READ,
							 MAP_SHARED, clipData->fileDevice, 0);
	if (ptr == MAP_FAILED)
	{
		perror("内存映射失败");
		return "";
	}

	// 读取数据头
	SharedMemoryHeader header;
	memcpy(&header, ptr, sizeof(header));

	tpString result;
	// 验证数据有效性
	if (header.magic == 0x54494E59 &&
		header.data_size <= (clipData->allocatedSize - sizeof(header)) &&
		header.checksum == calculateChecksumWrapper(ptr + sizeof(header), header.data_size))
	{
		result.assign(ptr + sizeof(header), header.data_size);
	}

	munmap(ptr, clipData->allocatedSize);
	return result;
}

uint64_t tpClipboard::size()
{
	tpClipboardData *clipData = static_cast<tpClipboardData *>(data_);
	if (!clipData)
		return 0;

	std::lock_guard<std::mutex> lock(clipData->gMutex);
	struct stat sb;
	if (fstat(clipData->fileDevice, &sb) == -1)
	{
		return 0;
	}
	return sb.st_size;
}

bool tpClipboard::hasText()
{
	return !text().empty();
}

void tpClipboard::clear()
{
	tpClipboardData *clipData = static_cast<tpClipboardData *>(data_);
	if (!clipData)
		return;

	std::lock_guard<std::mutex> lock(clipData->gMutex);
	if (ftruncate(clipData->fileDevice, 0) == -1)
	{
		perror("清空剪贴板失败");
	}
}
