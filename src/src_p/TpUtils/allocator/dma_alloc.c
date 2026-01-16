#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <linux/types.h>
#include "allocator/dma_alloc.h"


// DMA heap ioctl定义
struct dma_heap_allocation_data {
    __u64 len;
    __u32 fd;
    __u32 fd_flags;
    __u64 heap_flags;
};

const char *DmaHeapList[] = {
	"system-uncached-dma32",
	"system-uncached",
	"system",
	"linux,cma",
	"cma",
	"carveout",
	//添加更多heap名称根据需要
	NULL
};

int dma_heap_open(const char *heap_name)
{
    char path[256];
    int fd;
	
	// 尝试打开指定的heap
    snprintf(path, sizeof(path), "%s%s", DMA_HEAP_PATH_PREFIX, heap_name);
    
    fd = open(path, O_RDWR);
    if (fd < 0) {
       //使用列表中的heap名称尝试打开
	   const char **name = DmaHeapList;
	   while (*name) {
		   snprintf(path, sizeof(path), "%s%s", DMA_HEAP_PATH_PREFIX, *name);
		   fd = open(path, O_RDWR);
		   if (fd >= 0) {
			   printf("[Debug] 使用替代DMA堆: %s\n", *name);
			   return fd;
		   }
		   name++;
		}
	   perror("Failed to open DMA heap");
	   return -1;
    }

    if (fd < 0) {
        return -1;
    }
    
    return fd;
}

//申请dma内存，返回dma_fd
int dma_alloc(const char* heap_name, size_t size, void** out_ptr) {
	int heap_fd = dma_heap_open(heap_name);
	if (heap_fd < 0) {
		perror("Failed to open DMA heap");
		return -1;
	}

	struct dma_heap_allocation_data alloc_data = {
		.len = size,
		.fd_flags = O_CLOEXEC | O_RDWR,
		.heap_flags = 0,
	};

	if (ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &alloc_data) < 0) {
		perror("DMA heap allocation failed");
		close(heap_fd);
		return -1;
	}

	close(heap_fd);

	int dma_fd = alloc_data.fd;
	void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, dma_fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap failed");
		close(dma_fd);
		return -1;
	}

	*out_ptr = ptr;
	return dma_fd;
}


void dma_buf_free(size_t size, int fd, void *va) {
    int len;

    len =  size;
    munmap(va, len);

    close(fd);
}
