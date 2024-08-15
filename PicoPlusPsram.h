#pragma once

#include "lwmem/lwmem.h"

class PicoPlusPsram
{
  public:
    PicoPlusPsram(const PicoPlusPsram&) = delete;
    PicoPlusPsram& operator = (const PicoPlusPsram&) = delete;

    static PicoPlusPsram& getInstance()
    {
      static PicoPlusPsram instance;       
      return instance;
    }

    size_t GetMemorySize(void)
    {
      return m_uMemorySize;
    }

    void *Malloc(size_t uSize)
    {
      return lwmem_malloc_ex(nullptr, nullptr, uSize);
    }

    void *Calloc(size_t uItems, size_t uSize) 
    {
      return lwmem_calloc_ex(nullptr, nullptr, uItems, uSize);
    }

    void Realloc(void * const pMem, const size_t uSize)
    {
      lwmem_realloc_ex(nullptr, nullptr, pMem, uSize);
    }

    void Free(void * const pMem)
    {
      lwmem_free_ex(nullptr, pMem);
    }

    size_t GetSize(void *pMem)
    {
      return lwmem_get_size_ex(nullptr, pMem);
    }

#if LWMEM_CFG_ENABLE_STATS
    size_t GetAvailableBytes(void)
    {
      lwmem_get_stats_ex(nullptr, &m_lwmemStats);
      return m_lwmemStats.mem_available_bytes; 
    }
#endif

  private:
    PicoPlusPsram(void);
    ~PicoPlusPsram(void) = default;

    size_t Detect(void);
    size_t Init(uint cs_pin);

    size_t m_uMemorySize = 0;

#if LWMEM_CFG_ENABLE_STATS
    lwmem_stats_t m_lwmemStats;
#endif
};
