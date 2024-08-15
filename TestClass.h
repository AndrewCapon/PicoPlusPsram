#include "PicoPlusPsram.h"

class TestClass : public PicoPlusPsram::BaseClass
{
public:
  TestClass(uint32_t uVal)
  : m_uVal(uVal) 
  {};

  ~TestClass(void)
  {
  }

  void DoSomething(void)
  {
    for(uint32_t u = 0; u < 128; u++)
      m_data[u]=u;
  }

  bool CheckSomething(void)
  {
    bool bResult = true;
    for(uint32_t u = 0; bResult && (u < 128); u++)
      bResult = (m_data[u]==u);

    return bResult;
  }

private:
  uint32_t  m_uVal;
  uint8_t   m_data[128];
};