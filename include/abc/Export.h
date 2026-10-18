#pragma once

// ABC_CORE_API marks symbols that live in AbcCore.dll.
// Define ABC_CORE_STATIC in a static consumer to disable the import attribute.
#if !defined(_WIN32) || defined(ABC_CORE_STATIC)
#define ABC_CORE_API
#elif defined(ABC_CORE_EXPORTS)
#define ABC_CORE_API __declspec(dllexport)
#else
#define ABC_CORE_API __declspec(dllimport)
#endif
