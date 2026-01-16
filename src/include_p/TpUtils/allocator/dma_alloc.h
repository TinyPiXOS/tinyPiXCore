#ifndef __DMA_ALLOC_H__
#define __DMA_ALLOC_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


#define DMA_HEAP_PATH_PREFIX "/dev/dma_heap/"

#define DMA_HEAP_IOC_MAGIC		'H'
#define DMA_HEAP_IOCTL_ALLOC    _IOWR(DMA_HEAP_IOC_MAGIC, 0x0, \
                                      struct dma_heap_allocation_data)



								  

int dma_alloc(const char* heap_name, size_t size, void** out_ptr);
void dma_buf_free(size_t size, int dma_fd, void* ptr);




#endif