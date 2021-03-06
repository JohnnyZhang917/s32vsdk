/*****************************************************************************
*
* Freescale Confidential Proprietary
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
*****************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/
/**
* \file oal_memory.cpp
* \brief OAL memory allocation implementation
* \author Igor Aleksandrowicz and Rostislav Hulik
* \version
* \date 4-March-2016
****************************************************************************/


#include <oal.h>
#include <oal_protected.h>
#include <standalone/oal_memory_driver_if.h>
#include <oal_allocator_private.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/////////////////////////////////////////////////////////////////////////
/// Forward declarations
/////////////////////////////////////////////////////////////////////////
static const size_t CACHELINE_SIZE = 0x1000;
static const size_t PAGE_SIZE = 0x1000;
static const size_t MINIMUM_ALIGN = 0x1000;
extern bool ddr_init;

extern "C"
{
  void flush_dcache_range(void *, unsigned int);	
  void flush_and_invalidate_dcache_range(void *, unsigned int);	
  void invalidate_dcache_range(void *, unsigned int);	
}
/////////////////////////////////////////////////////////////////////////
/// Get alignment from flags
/////////////////////////////////////////////////////////////////////////
uint32_t AlignmentGet(OAL_MEMORY_FLAG aFlags)
{
  uint32_t ret = 0;

  if ((aFlags & OAL_MEMORY_FLAG_ALIGN(ALIGN2_CACHELINE)) ==
        OAL_MEMORY_FLAG_ALIGN(ALIGN2_CACHELINE))
  {
    ret = CACHELINE_SIZE;
  }
  else if ((aFlags & OAL_MEMORY_FLAG_ALIGN(ALIGN2_PAGE)) ==
      OAL_MEMORY_FLAG_ALIGN(ALIGN2_PAGE))
  {
    ret = PAGE_SIZE;
  }
  else if ((aFlags & OAL_MEMORY_FLAG_ALIGN(BYTE_N)) ==
      OAL_MEMORY_FLAG_ALIGN(BYTE_N))
  {
    uint32_t alignment = (uint32_t)((aFlags & 0x00FF0000) >> 16);
    ret = 1 << alignment;
  }
  
  return ret;
}

/////////////////////////////////////////////////////////////////////////
/// Get chunk from flags
/////////////////////////////////////////////////////////////////////////
uint8_t ChunkGet(OAL_MEMORY_FLAG aFlags)
{
  uint8_t chunk_id = (uint8_t)((aFlags & 0xFF000000) >> 24);
  
  if (chunk_id == 0) chunk_id = 0xFF;
  else chunk_id = chunk_id - 1;
  
  return chunk_id;
}

/////////////////////////////////////////////////////////////////////////
/// Allocate a block of memory that meets certain criteria. Memory will be allocated from
/// the appropriate heap while satisfying all the properties of flags. Memory
/// allocation of size will allocate a block of size and return 
/// a valid pointer.
/////////////////////////////////////////////////////////////////////////
void *OAL_MemoryAllocFlag(uint32_t size, OAL_MEMORY_FLAG flags)
{
  CHECK_INIT
  void* retval = NULL;
    
  uint32_t alignment = AlignmentGet(flags);
  uint8_t chunk_id = ChunkGet(flags);
  retval = OAL_ContAllocAlloc(size, alignment, chunk_id);
  
  if (retval != NULL)
  {
    void *virt = OAL_MemoryReturnAddress(retval, ACCESS_CH_WB);
   
    if (virt != NULL)
    {
      // if memory is supposed to be zeroed
      if (flags & OAL_MEMORY_FLAG_ZERO)
      {
        memset(virt, 0, size); 
      }
      
      OAL_MemoryFlushAndInvalidate(virt, size);
      OAL_MemoryUnmap(retval);
    }
  }
  return retval;
}

/////////////////////////////////////////////////////////////////////////
/// Returns a number of current allocations from all processes (currently hidden)
/////////////////////////////////////////////////////////////////////////
uint64_t OAL_MemoryGetNumAllocations()
{
  CHECK_INIT
  return OAL_ContAllocGetNumAllocations();
}

/////////////////////////////////////////////////////////////////////////
/// Returns a number of current mappings from all processes (currently hidden)
/////////////////////////////////////////////////////////////////////////
uint64_t OAL_MemoryGetNumMappings()
{
  CHECK_INIT
  return OAL_ContAllocGetNumMappings();
}

////////////////////////////////////////////////////////////////////////
/// Release the allocated memory and return it to memory manager. 
////////////////////////////////////////////////////////////////////////
int32_t OAL_MemoryFree(void *pBuffer)
{  
  CHECK_INIT
  if (pBuffer && OAL_ContAllocFree(pBuffer))
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

void OAL_MemoryUnmap(void *pBuffer)
{
  CHECK_INIT
  if (pBuffer)
  {
    OAL_ContAllocUnmap(pBuffer);
  }
}

////////////////////////////////////////////////////////////////////////
/// CPU cache function - flush (todo)
////////////////////////////////////////////////////////////////////////
void OAL_MemoryFlush(void *start, uint32_t size)
{
  CHECK_INIT
  flush_dcache_range((void *)start, size);
}

////////////////////////////////////////////////////////////////////////
/// CPU cache function - flush and invalidate specific region
////////////////////////////////////////////////////////////////////////
void OAL_MemoryFlushAndInvalidate(void *start, uint32_t size)
{
  CHECK_INIT
  flush_and_invalidate_dcache_range((void *)start, size);
}

////////////////////////////////////////////////////////////////////////
/// CPU cache function - invalidate (todo)
////////////////////////////////////////////////////////////////////////
void OAL_MemoryInvalidate(void *start, uint32_t size)
{
  CHECK_INIT
  invalidate_dcache_range((void *)start, size);
}

////////////////////////////////////////////////////////////////////////
/// CPU cache function - flush and invalidate all
////////////////////////////////////////////////////////////////////////
void OAL_MemoryFlushAndInvalidateAll (void)
{
  CHECK_INIT
  printf("TODO IMPLEMENT FLUSH ALL!!!!!\n");
}

////////////////////////////////////////////////////////////////////////
/// Takes the given memory pointer and returns the pointer
/// that allows proper access as identified in the access parameter
////////////////////////////////////////////////////////////////////////
void *OAL_MemoryReturnAddress(void *pBuffer, OAL_MEMORY_ACCESS access)
{
  CHECK_INIT
  return OAL_ContAllocGetBuffer((uint64_t)pBuffer, access);
}

/////////////////////////////////////////////////////////////////////////
// Function returns total size in bytes being managed by OAL Memory.
/////////////////////////////////////////////////////////////////////////
int64_t OAL_MemorySizeTotal(void)
{
  CHECK_INIT
  return OAL_ContAllocGetTotalSize();
}

/////////////////////////////////////////////////////////////////////////
// Function returns free size in bytes remaining in OAL Memory.
/////////////////////////////////////////////////////////////////////////
int64_t OAL_MemorySizeFree(void)
{
  CHECK_INIT
  return OAL_ContAllocGetFreeSize();
}
