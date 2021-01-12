#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if _CONTAINER_DEBUG_LEVEL > 0
#define SE_ASSERT(cond, msg) _STL_VERIFY(cond, msg)
#endif