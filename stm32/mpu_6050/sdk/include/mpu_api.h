#pragma once

#if defined(MPUSDK_BUILD)
  #define MPU_API __declspec(dllexport)
#else
  #define MPU_API __declspec(dllimport)
#endif
#define MPU_LOCAL
