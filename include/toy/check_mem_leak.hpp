#pragma once
#ifdef new
#undef new
#endif

#include <cassert>// assert
#include <cstdlib>// abort
#include <cstring>// strcpy/strncpy/sprintf

#include <iostream>
#include <memory>
#include <mutex>

namespace toy {

    int check_leaks();
    int check_mem_corruption();
}// namespace toy

#ifndef _DEBUG_NEW_ALIGNMENT
#define _DEBUG_NEW_ALIGNMENT 16
#endif
#define ALIGN(s)                                                               \
    (((s) + _DEBUG_NEW_ALIGNMENT - 1) & ~(_DEBUG_NEW_ALIGNMENT - 1))

#ifndef DEBUG_NEW_CALLER_ADDRESS
#ifdef __GNUC__
#define DEBUG_NEW_CALLER_ADDRESS __builtin_return_address(0)
#else
#define DEBUG_NEW_CALLER_ADDRESS nullptr
#endif
#endif

#ifndef DEBUG_NEW_FILENAME_LEN
#define DEBUG_NEW_FILENAME_LEN 200
#endif


struct new_ptr_list_t {
    new_ptr_list_t *next;   ///< Pointer to the next memory block
    new_ptr_list_t *prev;   ///< Pointer to the previous memory block
    std::size_t actual_size;///< Size of the memory block
    union {
        char file[DEBUG_NEW_FILENAME_LEN];///< File name of the caller

        void *addr;///< Address of the caller to new
    };
    unsigned line : 31;   ///< Line number of the caller; or \c 0
    unsigned is_array : 1;///< Non-zero iff <em>new[]</em> is used
    unsigned magic;       ///< Magic number for error detection
};

static const unsigned MAGIC_NUM_FOR_DEBUG = 0x4442474E;

static const int ALIGNED_LIST_ITEM_SIZE = ALIGN(sizeof(new_ptr_list_t));

static const auto NEW_PTR_LIST_SIZE = sizeof(new_ptr_list_t);

static new_ptr_list_t new_ptr_list
    = {&new_ptr_list, &new_ptr_list, 0, {0}, 0, 0, MAGIC_NUM_FOR_DEBUG};

static std::mutex new_ptr_lock;

static std::mutex new_output_lock;

static std::size_t total_mem_alloc = 0;

bool new_auto_check_flag = false;

bool new_verbose_flag = false;

static void print_position(const void *ptr, int line) {
    if (line != 0) {// Is file/line information present?
        std::printf("%s:%d", (const char *) ptr, line);
    } else if (ptr != nullptr) {// Is caller address present?
        printf("%p", ptr);
    } else {// No information is present
        printf("<Unknown>");
    }
}

static void *
alloc_mem(std::size_t size, const char *file, int line, bool is_array) {
    assert(line >= 0);

    std::size_t s = size + NEW_PTR_LIST_SIZE;
    auto *ptr = (new_ptr_list_t *) malloc(s);
    if (ptr == nullptr) {
        std::unique_lock<std::mutex> lock(new_output_lock);
        std::printf(
            "Out of memory when allocating %lu bytes\n",
            (unsigned long) size);
        abort();
    }
    void *usr_ptr = (new_ptr_list_t *) ptr + 1;

    if (line) {
        std::strncpy(
            ptr->file,
            file,
            DEBUG_NEW_FILENAME_LEN - 1)[DEBUG_NEW_FILENAME_LEN - 1]
            = '\0';
    } else {
        ptr->addr = (void *) file;
    }

    ptr->line = line;
    ptr->is_array = is_array;
    ptr->actual_size = size;
    ptr->magic = MAGIC_NUM_FOR_DEBUG;
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        ptr->prev = new_ptr_list.prev;
        ptr->next = &new_ptr_list;
        new_ptr_list.prev->next = ptr;
        new_ptr_list.prev = ptr;
    }
    if (new_verbose_flag) {
        std::unique_lock<std::mutex> lock(new_output_lock);
        printf(
            "new%s: allocated at %p (size %lu, ",
            is_array ? "[]" : "",
            usr_ptr,
            (unsigned long) size);
        if (line != 0) {
            print_position(ptr->file, ptr->line);
        } else {
            print_position(ptr->addr, ptr->line);
        }
        printf(")\n");
    }
    total_mem_alloc += size;
    return usr_ptr;
}

static void free_pointer(void *usr_ptr, void *addr, bool is_array) {
    if (usr_ptr == nullptr) {
        return;
    }
    auto *ptr = (new_ptr_list_t *) ((char *) usr_ptr - ALIGNED_LIST_ITEM_SIZE);
    if (ptr->magic != MAGIC_NUM_FOR_DEBUG) {// 可以检测栈是否有损坏
        {
            std::unique_lock<std::mutex> lock(new_output_lock);
            std::printf(
                "delete%s: invalid pointer %p (",
                is_array ? "[]" : "",
                usr_ptr);
            print_position(addr, 0);
            std::printf(")\n");
        }
        toy::check_mem_corruption();
        abort();
    }
    if ((unsigned) is_array
        != ptr->is_array) {// 可以检测new delete new[] delete[]是否配对使用
        const char *msg;
        if (is_array) {
            msg = "delete[] after new";
        } else {
            msg = "delete after new[]";
        }
        std::unique_lock<std::mutex> lock(new_output_lock);
        printf(
            "%s: pointer %p (size %lu)\n\tat ",
            msg,
            (char *) ptr + ALIGNED_LIST_ITEM_SIZE,
            (unsigned long) ptr->actual_size);
        print_position(addr, 0);
        printf("\n\toriginally allocated at ");
        if (ptr->line != 0) {
            print_position(ptr->file, ptr->line);
        } else {
            print_position(ptr->addr, ptr->line);
        }
        printf("\n");
        abort();
    }

    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        total_mem_alloc -= ptr->actual_size;
        ptr->magic = 0;
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
    }

    if (new_verbose_flag) {
        std::unique_lock<std::mutex> lock(new_output_lock);
        printf(
            "delete%s: freed %p (size %lu, %lu bytes still allocated)\n",
            is_array ? "[]" : "",
            (new_ptr_list_t *) ptr + 1,
            (unsigned long) ptr->actual_size,
            (unsigned long) total_mem_alloc);
    }
    free(ptr);
}

int toy::check_leaks() {
    int leak_cnt = 0;
    int whitelisted_leak_cnt = 0;
    new_ptr_list_t *ptr = new_ptr_list.next;

    while (ptr != &new_ptr_list) {
        const char *const usr_ptr = (char *) ptr + NEW_PTR_LIST_SIZE;
        if (ptr->magic != MAGIC_NUM_FOR_DEBUG) {
            printf("warning: heap data corrupt near %p\n", usr_ptr);
        }

        printf(
            "Leaked object at %p (size %lu, ",
            usr_ptr,
            (unsigned long) ptr->actual_size);

        if (ptr->line != 0) {
            print_position(ptr->file, ptr->line);
        } else {
            print_position(ptr->addr, ptr->line);
        }

        printf(")\n");

        ptr = ptr->next;
        ++leak_cnt;
    }
    if (new_verbose_flag || leak_cnt) {
        printf("*** %d leaks found\n", leak_cnt);
    }

    return leak_cnt;
}

int toy::check_mem_corruption() {
    int corrupt_cnt = 0;
    printf("*** Checking for memory corruption: START\n");
    for (new_ptr_list_t *ptr = new_ptr_list.next; ptr != &new_ptr_list;
         ptr = ptr->next) {
        const char *const usr_ptr = (char *) ptr + ALIGNED_LIST_ITEM_SIZE;
        if (ptr->magic == MAGIC_NUM_FOR_DEBUG) {
            continue;
        }

        printf(
            "Heap data corrupt near %p (size %lu, ",
            usr_ptr,
            (unsigned long) ptr->actual_size);

        if (ptr->line != 0) {
            print_position(ptr->file, ptr->line);
        } else {
            print_position(ptr->addr, ptr->line);
        }
        printf(")\n");
        ++corrupt_cnt;
    }
    printf("*** Checking for memory corruption: %d FOUND\n", corrupt_cnt);
    return corrupt_cnt;
}

void *operator new(std::size_t size, const char *file, int line) {
    void *ptr = alloc_mem(size, file, line, false);
    return ptr;
}

void *operator new[](std::size_t size, const char *file, int line) {
    void *ptr = alloc_mem(size, file, line, true);
    return ptr;
}

void *operator new(std::size_t size) {
    return operator new(size, (char *) DEBUG_NEW_CALLER_ADDRESS, 0);
}

void *operator new[](std::size_t size) {
    return operator new[](size, (char *) DEBUG_NEW_CALLER_ADDRESS, 0);
}

void operator delete(void *ptr) noexcept {
    free_pointer(ptr, DEBUG_NEW_CALLER_ADDRESS, false);
}

void operator delete[](void *ptr) noexcept {
    free_pointer(ptr, DEBUG_NEW_CALLER_ADDRESS, true);
}

#define new new (__FILE__, __LINE__)