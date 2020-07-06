/*
 * Copyright (c) Bridgetek Pte Ltd
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 * LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON
 * BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE
 * INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED,
 * IT IS THE RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES
 * AND SUBSEQUENT WHQL RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 */

#ifndef GPU_H_
#define GPU_H_

#define DL_SIZE           (8*1024L)	/* 8KB Display List buffer size */
#define CMD_FIFO_SIZE     (4*1024L)	/* 4KB coprocessor Fifo size */
#define CMD_SIZE          (4)		/* 4 byte per coprocessor command of EVE */

#include <stdint.h>

constexpr uint32_t VERTEX2F(uint32_t x, uint32_t y)
{
    return ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0));
}

constexpr uint32_t VERTEX2II(uint32_t x, uint32_t y, uint32_t handle, uint32_t cell)
{
    return ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0));
}

uint32_t BITMAP_SOURCE(uint32_t addr);
uint32_t BITMAP_SOURCE2(uint32_t flash_or_ram, uint32_t addr);

constexpr uint32_t CLEAR_COLOR_RGB(uint32_t red, uint32_t green, uint32_t blue)
{
    return ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0));
}

constexpr uint32_t TAG(uint32_t s){
    return ((3UL<<24)|(((s)&255UL)<<0));
}

constexpr uint32_t COLOR_RGB(uint32_t red, uint32_t green, uint32_t blue) {
    return ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0));
}

constexpr uint32_t BITMAP_HANDLE(uint32_t handle) {
    return ((5UL<<24)|(((handle)&31UL)<<0));
}

constexpr uint32_t CELL(uint32_t cell) {
    return ((6UL<<24)|(((cell)&127UL)<<0));
}

constexpr uint32_t BITMAP_LAYOUT(uint32_t format, uint32_t linestride, uint32_t height) {
    return ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0));
}

constexpr uint32_t BITMAP_SIZE(uint32_t filter,uint32_t wrapx,uint32_t wrapy,uint32_t width,uint32_t height) {
    return ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0));
}

constexpr uint32_t ALPHA_FUNC(uint32_t func, uint32_t ref) {
    return ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0));
}

constexpr uint32_t STENCIL_FUNC(uint32_t func, uint32_t ref, uint32_t mask) {
    return ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0));
}

constexpr uint32_t BLEND_FUNC(uint32_t src, uint32_t dst) {
    return ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0));
}

constexpr uint32_t STENCIL_OP(uint32_t sfail, uint32_t spass) {
    return ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0));
}

constexpr uint32_t POINT_SIZE(uint32_t size) {
    return ((13UL<<24)|(((size)&8191UL)<<0));
}

constexpr uint32_t LINE_WIDTH(uint32_t width) {
    return ((14UL<<24)|(((width)&4095UL)<<0));
}

constexpr uint32_t CLEAR_COLOR_A(uint32_t alpha) {
    return ((15UL<<24)|(((alpha)&255UL)<<0));
}

constexpr uint32_t COLOR_A(uint32_t alpha) {
    return ((16UL<<24)|(((alpha)&255UL)<<0));
}

constexpr uint32_t CLEAR_STENCIL(uint32_t s) {
    return ((17UL<<24)|(((s)&255UL)<<0));
}

constexpr uint32_t CLEAR_TAG(uint32_t s) {
    return ((18UL<<24)|(((s)&255UL)<<0));
}

constexpr uint32_t STENCIL_MASK(uint32_t mask) {
    return ((19UL<<24)|(((mask)&255UL)<<0));
}

constexpr uint32_t TAG_MASK(uint32_t mask) {
    return ((20UL<<24)|(((mask)&1UL)<<0));
}

constexpr uint32_t BITMAP_TRANSFORM_A(uint32_t a)
{
    return ((21UL<<24)|(((a)&131071UL)<<0))  ;
}

constexpr uint32_t BITMAP_TRANSFORM_B(uint32_t b)
{
    return ((22UL<<24)|(((b)&131071UL)<<0))  ;
}

constexpr uint32_t BITMAP_TRANSFORM_C(uint32_t c)
{
    return ((23UL<<24)|(((c)&16777215UL)<<0));
}

constexpr uint32_t BITMAP_TRANSFORM_D(uint32_t d)
{
    return ((24UL<<24)|(((d)&131071UL)<<0))  ;
}

constexpr uint32_t BITMAP_TRANSFORM_E(uint32_t e)
{
    return ((25UL<<24)|(((e)&131071UL)<<0))  ;
}

constexpr uint32_t BITMAP_TRANSFORM_F(uint32_t f)
{
    return ((26UL<<24)|(((f)&16777215UL)<<0));
}

uint32_t SCISSOR_XY(uint32_t x, uint32_t y);
uint32_t SCISSOR_SIZE(uint32_t width, uint32_t height);

constexpr uint32_t CALL(uint32_t dest) {
    return ((29UL<<24)|(((dest)&65535UL)<<0));
}

constexpr uint32_t JUMP(uint32_t dest)  {
    return ((30UL<<24)|(((dest)&65535UL)<<0));
}

constexpr uint32_t BEGIN(uint32_t prim) {
    return ((31UL<<24)|(((prim)&15UL)<<0));
}

constexpr uint32_t COLOR_MASK(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    return ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0));
}

constexpr uint32_t CLEAR(uint32_t c, uint32_t s, uint32_t t) {
    return ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0));
}

uint32_t VERTEX_FORMAT(uint32_t frac);
uint32_t BITMAP_LAYOUT_H(uint32_t linestride, uint32_t height);
uint32_t BITMAP_SIZE_H(uint32_t width, uint32_t height);
uint32_t PALETTE_SOURCE(uint32_t addr);
uint32_t VERTEX_TRANSLATE_X(uint32_t x);
uint32_t VERTEX_TRANSLATE_Y(uint32_t y);
uint32_t NOP();
uint32_t BITMAP_EXT_FORMAT(uint32_t format);
uint32_t BITMAP_SWIZZLE(uint32_t r,uint32_t g,uint32_t b,uint32_t a);
uint32_t INT_FRR();

constexpr uint32_t END() {
    return ((33UL<<24));
}

constexpr uint32_t SAVE_CONTEXT() {
    return ((34UL<<24));
}

constexpr uint32_t RESTORE_CONTEXT() {
    return ((35UL<<24));
}

constexpr uint32_t RETURN() {
    return ((36UL<<24));
}

constexpr uint32_t MACRO(uint32_t m) {
    return ((37UL<<24)|(((m)&1UL)<<0));
}

constexpr uint32_t DISPLAY() {
    return ((0UL<<24));
}

#ifndef CTOUCH_MODE_COMPATIBILITY
#define CTOUCH_MODE_COMPATIBILITY 1
#endif
#ifndef CTOUCH_MODE_EXTENDED
#define CTOUCH_MODE_EXTENDED 0
#endif

#define GPU_NUMCHAR_PERFONT (128)
#define GPU_FONT_TABLE_SIZE (148)

/* FT81x and FT80x font table structure */
/* Font table address in ROM can be found by reading the address from 0xFFFFC location. */
/* 16 font tables are present at the address read from location 0xFFFFC */
typedef struct Gpu_Fonts
{
    /* All the values are in bytes */
    /* Width of each character font from 0 to 127 */
    uint8_t    FontWidth[GPU_NUMCHAR_PERFONT];
    /* Bitmap format of font wrt bitmap formats supported by FT800 - L1, L4, L8 */
    uint32_t    FontBitmapFormat;
    /* Font line stride in FT800 ROM */
    uint32_t    FontLineStride;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
}Gpu_Fonts_t;

#ifdef BT81X
/* BT815 inclusion - extended font table for unicode support */
typedef struct Gpu_FontsExt_t
{
    uint32_t Signature;
    uint32_t Size;
    uint32_t Format;
    uint32_t Swizzle;
    uint32_t LayoutWidth;
    uint32_t LayoutHeight;
    uint32_t PixelWidth;
    uint32_t pixelHeight;
    uint32_t StartOfGraphicData;
    uint32_t OffsetGlyphData;
}Gpu_FontsExt_t;
#endif

#endif /* #ifndef GPU_H_ */
