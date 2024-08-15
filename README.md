# PicoPlusPsram

Classes for accessing PSRAM on a Pimoroni Pico Plus 2.

This is basically a wrapper arounf `lwmem` with a C++ BaseClass to inhertit from for any classes you want to dynamically store in PSRAM, and a C++ Allocator.

## Basics

PicoPlusPsram is a singleton and can be accessed using `PicoPlusPsram::instance()`

To check for available memory:

```
PicoPlusPsram &ps = PicoPlusPsram::getInstance();

size_t uMemorySize = ps.GetMemorySize();
```

if `uMemorySize` is 0, you have no PSRAM or something terrible has gone wrong.

if you need to know how mush PSRAM you still have available:
```
PicoPlusPsram &ps = PicoPlusPsram::getInstance();

size_t uStartFreeMem = ps.GetAvailableBytes();
```

## C Style Allocations

Malloc, Calloc, Realloc and Free are implemented.

eg:

```
PicoPlusPsram &ps = PicoPlusPsram::getInstance();

void *pMem = ps.Malloc(1024);
```

## C++ goodies

If you want your objects to be dynamically stored in PSRAM then inherit from `PicoPlusPsram::BaseClass`

eg:

```
#include "PicoPlusPsram.h"

class TestClass : public PicoPlusPsram::BaseClass
{
public:
  TestClass(uint32_t uVal) : m_uVal(uVal) {};
  ~TestClass(void) {}

  uint32_t Get(void)
  {
    return m_uVal;
  }
private:
  uint32_t  m_uVal;
};
```

Now if you new this class it will be stored in psram.
```
TestClass *pImInPsram = new TestClass(1);
```

There is also a C++ allocator `PicoPlusPsram::Allocator`

This can be used anywhere an allocator is needed, eg:

```
std::vector<int, PicoPlusPsram::Allocator<int>> vInPsram;
vInPsram.push_back(1);
vInPsram.push_back(2);
```

## Notes: 

The c++ allocation and new stuff should really be thowing exceptions, but this seems to be disabled by default for the pico builds.

`PicoPlusPsram::Detect` and `PicoPlusPsram::Init` are from: https://github.com/pimoroni/micropython

`lwmem` from: https://github.com/MaJerle/lwmem and is a submodule so make sure you fetch that.

`Test.cpp` and `TestClass.h` test out the functionality, best to start there.
