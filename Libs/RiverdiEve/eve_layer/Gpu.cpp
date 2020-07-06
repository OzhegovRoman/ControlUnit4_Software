#include "displaysettings.h"
#include <cassert>

#include "Gpu.h"

uint32_t BITMAP_SOURCE(uint32_t addr)
{
    return ((1UL<<24)|(((addr) & (DisplaySettings::getInstance().getDisplayGeneration() == DisplaySettings::dgEve1 ? 1048575UL : 4194303UL))<<0));
}

uint32_t BITMAP_SOURCE2(uint32_t flash_or_ram, uint32_t addr)
{
    assert(EVE3 && "BITMAP_SOURCE2 worked only on EVE_3");
    return  ((1UL<<24)|((flash_or_ram) << 23)|(((addr)&8388607UL)<<0));
}

uint32_t SCISSOR_XY(uint32_t x, uint32_t y) {
    if (DisplaySettings::getInstance().getDisplayGeneration() == DisplaySettings::dgEve1)
        return ((27UL<<24)|(((x)&511UL)<<9)|(((y)&511UL)<<0));
    return ((27UL<<24)|(((x)&2047UL)<<11)|(((y)&2047UL)<<0));
}

uint32_t SCISSOR_SIZE(uint32_t width, uint32_t height)
{
    if (DisplaySettings::getInstance().getDisplayGeneration() == DisplaySettings::dgEve1)
        return ((28UL<<24)|(((width)&1023UL)<<10)|(((height)&1023UL)<<0));
    return ((28UL<<24)|(((width)&4095UL)<<12)|(((height)&4095UL)<<0));
}

uint32_t VERTEX_FORMAT(uint32_t frac) {
    assert(!EVE1 && "VERTEX_FORMAT function worked only on EVE_2 or EVE_3");
    return ((39UL<<24)|(((frac)&7UL)<<0));
}

uint32_t BITMAP_LAYOUT_H(uint32_t linestride, uint32_t height) {
    assert(!EVE1 && "BITMAP_LAYOUT_H function worked only on EVE_2 or EVE_3");
    return ((40UL<<24)|(((linestride)&3UL)<<2)|(((height)&3UL)<<0));
}

uint32_t BITMAP_SIZE_H(uint32_t width, uint32_t height) {
    assert(!EVE1 && "BITMAP_SIZE_H function worked only on EVE_2 or EVE_3");
    return ((41UL<<24)|(((width)&3UL)<<2)|(((height)&3UL)<<0));
}

uint32_t PALETTE_SOURCE(uint32_t addr) {
    assert(!EVE1 && "PALETTE_SOURCE function worked only on EVE_2 or EVE_3");
    return ((42UL<<24)|(((addr)&4194303UL)<<0));
}

uint32_t VERTEX_TRANSLATE_X(uint32_t x) {
    assert(!EVE1 && "VERTEX_TRANSLATE_X function worked only on EVE_2 or EVE_3");
    return ((43UL<<24)|(((x)&131071UL)<<0));
}

uint32_t VERTEX_TRANSLATE_Y(uint32_t y) {
    assert(!EVE1 && "VERTEX_TRANSLATE_Y function worked only on EVE_2 or EVE_3");
    return ((44UL<<24)|(((y)&131071UL)<<0));
}

uint32_t NOP() {
    assert(!EVE1 && "NOP function worked only on EVE_2 or EVE_3");
    return ((45UL<<24));
}

uint32_t BITMAP_EXT_FORMAT(uint32_t format) {
    assert(EVE3 && "BITMAP_EXT_FORMAT function worked only on EVE_3");
    return ((46UL<<24)|(((format)&65535UL)<<0));
}

uint32_t BITMAP_SWIZZLE(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    assert(EVE3 && "BITMAP_SWIZZLE function worked only on EVE_3");
    return ((47UL<<24)|(((r)&7UL)<<9)|(((g)&7UL)<<6)|(((b)&7UL)<<3)|(((a)&7UL)<<0));
}

uint32_t INT_FRR() {
    assert(EVE3 && "INT_FRR function worked only on EVE_3");
    return  ((48UL<<24));
}
