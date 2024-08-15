#include <cstdio>
#include <vector>

#include "pico/stdlib.h"

#include "PicoPlusPsram.h"
#include "TestClass.h"

void *TestMalloc(PicoPlusPsram &ps, size_t uSize)
{
  void *pMem = ps.Malloc(uSize);
  if(pMem)
    printf("TestMalloc: Malloc %lu bytes succeeded %p, size: %lu, memory left: %lu bytes\n", uSize, pMem, ps.GetSize(pMem), ps.GetAvailableBytes());
  else
    printf("TestMalloc: Error: Malloc %lu bytes failed\n", uSize);

  return pMem;
}

bool TestRealloc(PicoPlusPsram &ps, void **pMem, size_t uSize)
{
  bool bResult = false;

  ps.Realloc(*pMem, uSize);
  if(ps.GetSize(*pMem) == uSize)
  {
    printf("TestRealloc: Realoc %lu butes succeeded %p, size: %lu, memory left: %lu bytes\n", uSize, *pMem, ps.GetSize(*pMem), ps.GetAvailableBytes());
    bResult = true;
  }
  else
    printf("TestRealloc: Error: Realoc %lu bytes failed\n", uSize);

  return bResult;
}

void *TestCalloc(PicoPlusPsram &ps, size_t uItems, size_t uSize)
{
  void *pMem = ps.Calloc(uItems, uSize);
  if(pMem)
    printf("TestCalloc: Calloc %lu items of %lu bytes succeeded %p, size: %lu, memory left: %lu bytes\n", uItems, uSize, pMem, ps.GetSize(pMem), ps.GetAvailableBytes());
  else
    printf("TestCalloc: Error: Calloc %lu items of %lu bytes failed\n", uItems, uSize);

  return pMem;
}

bool TestFree(PicoPlusPsram &ps, void *pMem, const char *pStr)
{
  bool bResult = false;

  size_t uStartFreeMem = ps.GetAvailableBytes();
  size_t uFreeing = ps.GetSize(pMem);
  ps.Free(pMem);
  size_t uEndFreeMem = ps.GetAvailableBytes();

  if(uEndFreeMem == uStartFreeMem+uFreeing+8) // 8 is overhead bytes
  {
    printf("TestFree: Free '%s' at %p with %lu bytes succeeded, memory left: %lu bytes\n", pStr, pMem, uFreeing, uEndFreeMem);
    bResult = true;
  }
  else
    printf("TestFree: Error: Free '%s' at %p with %lu bytes failed\n", pStr, pMem, uFreeing);

  return bResult;
}


bool TestMethods(PicoPlusPsram &ps)
{
  // Get available memory ond log it
  size_t uStartFreeMem = ps.GetAvailableBytes();
  printf("Start test available memory: %lu bytes\n", uStartFreeMem);

  // Test Malloc
  void *pMallocMem = TestMalloc(ps, 1024*256);
  bool bMallocTestResult = pMallocMem != nullptr;

  // Test Realloc
  bool bReallocTestResult = false;
  if(bMallocTestResult)
    bReallocTestResult = TestRealloc(ps, &pMallocMem, 1024*512);

  // Test CMalloc
  void *pCallocMem = TestCalloc(ps, 1024, 1024);
  bool bCallocTestResult = pCallocMem != nullptr;

  // Free MallocMem
  bool bFreeMallocTestResult = false;
  if(pMallocMem)
    bFreeMallocTestResult = TestFree(ps, pMallocMem, "Malloc Memory");

  // Free CallocMem
  bool bFreeCallocTestResult = false;
  if(pCallocMem)
    bFreeCallocTestResult = TestFree(ps, pCallocMem, "Calloc Memory");

  bool bMemoryLossTestResult = false;
  size_t uEndFreeMem = ps.GetAvailableBytes();
  printf("End test available memory: %lu bytes\n", uEndFreeMem);
  bMemoryLossTestResult = (uEndFreeMem == uStartFreeMem);

  printf("\n");
  printf("Malloc Test        : %s\n", bMallocTestResult ? "Success" : "failure");
  printf("Realloc Test       : %s\n", bReallocTestResult ? "Success" : "failure");
  printf("Calloc Test        : %s\n", bCallocTestResult ? "Success" : "failure");
  printf("Free Malloc Test   : %s\n", bFreeMallocTestResult ? "Success" : "failure");
  printf("Free Calloc Test   : %s\n", bFreeCallocTestResult ? "Success" : "failure");
  printf("Memory loss Test   : %s\n", bMemoryLossTestResult ? "Success" : "failure");
  
  bool bAllTestsResult = bMallocTestResult && bReallocTestResult && bCallocTestResult && bFreeMallocTestResult && bFreeCallocTestResult && bMemoryLossTestResult;

  return   bAllTestsResult;
}

bool TestObjects(PicoPlusPsram &ps)
{
  bool bResult = false;
  size_t uMemorySizeStart = ps.GetAvailableBytes();

  TestClass *pTc1 = new TestClass(1);
  TestClass *pTc2 = new TestClass(2);

  if(pTc1 && pTc2)
  {
    pTc1->DoSomething();
    pTc2->DoSomething();
    bool bTc1Result = pTc1->CheckSomething();
    bool bTc2Result = pTc1->CheckSomething();
    delete pTc1;
    delete pTc2;
    bResult = bTc1Result && bTc2Result;
    if(!bResult)
      printf("TestObjects: Error: Object methods failed\n");
  }
  else 
    printf("TestObjects: Error: Objects not created\n");

  size_t uMemorySizeEnd = ps.GetAvailableBytes();

  if(bResult)
  {
    bResult = uMemorySizeEnd == uMemorySizeStart;
    if(!bResult)
      printf("TestObjects: Error: Memory loss\n");
  }

  return bResult;
}

bool TestAllocator(PicoPlusPsram &ps)
{
  bool bResult = false;
  size_t uMemorySizeStart = ps.GetAvailableBytes();

  // block to make sure vector is destroyed
  {
    std::vector<int, PicoPlusPsram::Allocator<int>> v;
    v.push_back(1);
    v.push_back(2);
    
    if(v.back() == 2)
    {
      v.pop_back();
      if(v.back() == 1)
      {
        v.pop_back();
        bResult = true;
      }
    }

    if(!bResult)
      printf("TestAllocator: Error: Incorrect data\n");
  }

  size_t uMemorySizeEnd = ps.GetAvailableBytes();

  if(bResult)
  {
    bResult = uMemorySizeEnd == uMemorySizeStart;
    if(!bResult)
      printf("TestAllocator: Error: Memory loss\n");
  }
  
  return bResult;
}

bool TestCpp(PicoPlusPsram &ps)
{
  // Test objects in psram
  bool bTestObjectsResult = TestObjects(ps);

  // Test allocator for psram
  bool bTestAllocatorResult = TestAllocator(ps);

  printf("C++ Objects Test   : %s\n", bTestObjectsResult ? "Success" : "failure");
  printf("C++ Allocator Test : %s\n", bTestAllocatorResult ? "Success" : "failure");

  return bTestObjectsResult && bTestAllocatorResult;
}

int main(void)
{
  // Get reference
  PicoPlusPsram &ps = PicoPlusPsram::getInstance();

  // 	init stdio
  stdio_init_all();

  // get memory size
  size_t uMemorySize = ps.GetMemorySize();

  if(uMemorySize)
  {
    printf("PSRAM Found: %lu MB total.\n", uMemorySize/1024/1024);

    bool bTestMethodsResult = TestMethods(ps);

    bool bTestCpp = TestCpp(ps);

    if(bTestMethodsResult && bTestCpp)
      printf("\nAll Tests Passed\n");
    else
      printf("\nSome Tests Failed\n");
    
  }
  else
    printf("No PSRAM found.\n");
  
  //spin
  while(true)
    sleep_ms(1000);
}