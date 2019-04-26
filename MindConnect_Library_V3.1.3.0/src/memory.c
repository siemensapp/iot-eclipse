/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     memory.c
* @date     Jul 28, 2016
* @brief    Memory module implementation file.
*
************************************************************************/

#include "memory.h"
#include "definitions.h"
#include "log_util.h"

#if (1 == HAVE_STDLIB_H_)
#include <stdlib.h>
#endif

void *memory_allocate(mcl_size_t bytes, const char *function, unsigned line)
{
    VERBOSE_ENTRY("mcl_size_t bytes = <%u>, const char *function = <%s>, unsigned line = <%u>", bytes, function, line)

	void *p = MCL_NULL;

    ASSERT_MESSAGE(0 != bytes, "Requested bytes size is equal to 0!");

    p = memory_malloc(bytes);

    ASSERT_MESSAGE(MCL_NULL != p, "Memory couldn't be allocated!");

    MCL_VERBOSE("%d bytes allocated in memory successfully. Address = <%p>", bytes, p);
    VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

void *memory_allocate_with_zero(mcl_size_t count, mcl_size_t bytes, const char *function, unsigned line)
{
    VERBOSE_ENTRY("mcl_size_t count = <%u>, mcl_size_t bytes = <%u>, const char *function = <%s>, unsigned line = <%u>", count, bytes, function, line)

	void *p;

    ASSERT_MESSAGE(0 != count, "Requested count size is equal to 0!");
    ASSERT_MESSAGE(0 != bytes, "Requested bytes size is equal to 0!");

    p = memory_calloc(count, bytes);

    ASSERT_MESSAGE(p, "Memory couldn't be allocated!");

    MCL_VERBOSE("%d bytes allocated in memory successfully. Address = <%p>", count * bytes, p);
    VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

void *memory_reallocate(void *p, mcl_size_t bytes, const char *function, unsigned line)
{
    VERBOSE_ENTRY("void *p = <%p>, mcl_size_t bytes = <%u>, const char *function = <%s>, unsigned line = <%u>", p, bytes, function, line)

	void *temp = MCL_NULL;

    ASSERT_MESSAGE(0 != bytes, "Requested bytes size is equal to 0!");

    temp = memory_realloc(p, bytes);
    if (MCL_NULL != temp)
    {
        p = temp;
    }
    else
    {
        MCL_VERBOSE("Memory couldn't be re-allocated! Freeing the original one.");
        MCL_FREE(p);
        VERBOSE_LEAVE("retVal = <%p>", p);
        return p;
    }

    MCL_VERBOSE("%d bytes re-allocated in memory successfully. Address = <%p>", bytes, p);
    VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

void memory_release(void *p, const char *function, unsigned line)
{
    VERBOSE_ENTRY("void *p = <%p>, const char *function = <%s>, unsigned line = <%u>", p, function, line)

    if (MCL_NULL != p)
    {
        memory_free(p);
    }
    else
    {
        MCL_VERBOSE("Pointer is already NULL!");
    }

    VERBOSE_LEAVE("retVal = void");
}

void *memory_malloc(mcl_size_t size)
{
    VERBOSE_ENTRY("mcl_size_t size = <%u>", size)

    void *p = malloc(size);

    VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

void *memory_calloc(mcl_size_t count, mcl_size_t bytes)
{
    VERBOSE_ENTRY("mcl_size_t count = <%u>, mcl_size_t bytes = <%u>", count, bytes)

    void *p = calloc(count, bytes);

    VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

void *memory_realloc(void *p, mcl_size_t bytes)
{
    VERBOSE_ENTRY("void *p = <%p>, mcl_size_t bytes = <%u>", p, bytes)

    p = realloc(p, bytes);

    VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

void memory_free(void *p)
{
    VERBOSE_ENTRY("void *p = <%p>", p)

    MCL_VERBOSE("Memory will be freed at location = <%p>.", p);
    free(p);

    VERBOSE_LEAVE("retVal = void");
}
