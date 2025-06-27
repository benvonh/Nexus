#pragma once

#ifdef HOST_BUILD
#define DT_API __declspec(dllimport)
#else
#define DT_API __declspec(dllexport)
#endif