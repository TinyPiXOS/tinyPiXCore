//dma单独测试，不对外公开

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "../../../src/include_p/TpUtils/allocator/dma_alloc.h"

#define TEST_SIZE   (1024 * 1024)   // 1MB

static uint64_t virt_to_pfn(void *addr)
{
    uint64_t value;
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        perror("open pagemap");
        return 0;
    }

    off_t offset = ((uintptr_t)addr / getpagesize()) * sizeof(uint64_t);
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return 0;
    }

    if (read(fd, &value, sizeof(value)) != sizeof(value)) {
        perror("read pagemap");
        close(fd);
        return 0;
    }

    close(fd);

    if (!(value & (1ULL << 63))) {
        // 页面不存在
        return 0;
    }

    return value & ((1ULL << 55) - 1); // PFN
}

static void check_phys_contiguous(const char *name, void *buf, size_t size)
{
    size_t page_size = getpagesize();
    size_t pages = size / page_size;

    printf("\n[%s]\n", name);
    printf("virt addr: %p, size: %zu bytes (%zu pages)\n",
           buf, size, pages);

    uint64_t last_pfn = 0;
    int contiguous = 1;

    for (size_t i = 0; i < pages; i++) {
        void *addr = (char *)buf + i * page_size;
        uint64_t pfn = virt_to_pfn(addr);

        if (i == 0) {
            last_pfn = pfn;
            printf("page %zu: PFN = 0x%lx\n", i, pfn);
        } else {
            printf("page %zu: PFN = 0x%lx %s\n",
                   i, pfn,
                   (pfn == last_pfn + 1) ? "(+1)" : "(BREAK)");
            if (pfn != last_pfn + 1)
                contiguous = 0;
            last_pfn = pfn;
        }
    }

    printf("=> physical contiguous: %s\n",
           contiguous ? "YES" : "NO");
}

int main(void)
{
    printf("Page size: %ld bytes\n", sysconf(_SC_PAGESIZE));

    /* ---------- malloc ---------- */
    void *malloc_buf = aligned_alloc(getpagesize(), TEST_SIZE);
    memset(malloc_buf, 0xAA, TEST_SIZE);

    check_phys_contiguous("malloc", malloc_buf, TEST_SIZE);

    free(malloc_buf);

    /* ---------- DMA heap ---------- */
    void *dma_ptr = NULL;
    int dma_fd = dma_alloc(NULL, TEST_SIZE, &dma_ptr);
    if (dma_fd < 0) {
        printf("dma_alloc failed\n");
        return -1;
    }

    memset(dma_ptr, 0x55, TEST_SIZE);

    check_phys_contiguous("dma_heap", dma_ptr, TEST_SIZE);

    dma_buf_free(TEST_SIZE, dma_fd, dma_ptr);

    return 0;
}
