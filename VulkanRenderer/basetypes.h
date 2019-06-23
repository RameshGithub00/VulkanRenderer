#pragma once
#include <vector>
#include <vulkan\vulkan.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef int64_t		u64;

//GrowingArray
#define GrowingArray std::vector
#define EMPTY_GROWING_ARRAY(x) GrowingArray<x>()

#define assert(x,y) _ASSERT_EXPR(x,y)