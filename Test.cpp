#include <cstdio>

#include "pico/stdlib.h"

#include "PicoPlusPsram.h"

void *TestMalloc(PicoPlusPsram &ps, size_t uSize)
{
  void *pMem = ps.Malloc(uSize);
  if(pMem)
    printf("Malloc %lu bytes succeeded %p, size: %lu, memory left: %lu bytes\n", uSize, pMem, ps.GetSize(pMem), ps.GetAvailableBytes());
  else
    printf("Error: Malloc %lu bytes failed\n", uSize);

  return pMem;
}

bool TestRealloc(PicoPlusPsram &ps, void **pMem, size_t uSize)
{
  bool bResult = false;

  ps.Realloc(*pMem, uSize);
  if(ps.GetSize(*pMem) == uSize)
  {
    printf("Realoc %lu butes succeeded %p, size: %lu, memory left: %lu bytes\n", uSize, *pMem, ps.GetSize(*pMem), ps.GetAvailableBytes());
    bResult = true;
  }
  else
    printf("Error: Realoc %lu bytes failed\n", uSize);

  return bResult;
}

void *TestCalloc(PicoPlusPsram &ps, size_t uItems, size_t uSize)
{
  void *pMem = ps.Calloc(uItems, uSize);
  if(pMem)
    printf("Calloc %lu items of %lu bytes succeeded %p, size: %lu, memory left: %lu bytes\n", uItems, uSize, pMem, ps.GetSize(pMem), ps.GetAvailableBytes());
  else
    printf("Error: Calloc %lu items of %lu bytes failed\n", uItems, uSize);

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
    printf("Free '%s' at %p with %lu bytes succeeded, memory left: %lu bytes\n", pStr, pMem, uFreeing, uEndFreeMem);
    bResult = true;
  }
  else
    printf("Error: Free '%s' at %p with %lu bytes failed\n", pStr, pMem, uFreeing);

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
  printf("Malloc Test      : %s\n", bMallocTestResult ? "Success" : "failure");
  printf("Realloc Test     : %s\n", bReallocTestResult ? "Success" : "failure");
  printf("Calloc Test      : %s\n", bCallocTestResult ? "Success" : "failure");
  printf("Free Malloc Test : %s\n", bFreeMallocTestResult ? "Success" : "failure");
  printf("Free Calloc Test : %s\n", bFreeCallocTestResult ? "Success" : "failure");
  printf("Memory loss Test : %s\n", bMemoryLossTestResult ? "Success" : "failure");
  
  bool bAllTestsResult = bMallocTestResult && bReallocTestResult && bCallocTestResult && bFreeMallocTestResult && bFreeCallocTestResult && bMemoryLossTestResult;
  printf("\n");
  if(bAllTestsResult)
    printf("All tests passed\n");
  else
    printf("Some tests failed\n");


  return   bAllTestsResult;
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
  }
  else
    printf("No PSRAM found.\n");
  
  //spin
  while(true)
    sleep_ms(1000);
}