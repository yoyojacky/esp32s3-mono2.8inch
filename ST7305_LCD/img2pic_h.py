#!/usr/bin/env python3
"""
img2pic_h.py  ——  任意图片 → ST7305 专用 pic.h （静默版）
用法:  python img2pic_h.py your.png
"""
import sys
from pathlib import Path
from PIL import Image

W, H = 168, 384
BYTES = W * H // 4

"""
def img_to_packed_bytes(im: Image.Image) -> bytearray:
    im = im.convert('L').resize((W, H), Image.LANCZOS)
    gs = im.point(lambda x: 3 - (x * 4 // 256))
    px = list(gs.getdata())
    packed = bytearray(BYTES)
    for i in range(0, W * H, 4):
        p0, p1, p2, p3 = px[i:i+4]
        packed[i//4] = (p0 << 6) | (p1 << 4) | (p2 << 2) | p3
    return packed
"""

def img_to_packed_bytes(im: Image.Image) -> bytearray:
    im = im.convert('L').resize((W, H), Image.LANCZOS)
    # 0 最黑，3 最白
    gs = im.point(lambda x: 3 - (x * 4 // 256))
    px = list(gs.getdata())
    packed = bytearray(BYTES)
    for i in range(0, W * H, 4):
        p0, p1, p2, p3 = px[i:i+4]
        packed[i//4] = (p0 << 6) | (p1 << 4) | (p2 << 2) | p3
    return packed

def build_header(data: bytes) -> str:
    lines = [', '.join(f'0x{b:02X}' for b in data[i:i+16])
             for i in range(0, len(data), 16)]
    body = ',\n'.join(f'    {l}' for l in lines)
    return f"""\
#ifndef PIC_H
#define PIC_H

#include <stdint.h>

const uint8_t pic1[] = {{
{body}
}};

#endif
"""

def main():
    if len(sys.argv) != 2:
        sys.exit(1)
    in_path = Path(sys.argv[1])
    if not in_path.exists():
        sys.exit(1)
    packed = img_to_packed_bytes(Image.open(in_path))
    Path('pic.h').write_text(build_header(packed), encoding='utf-8')

if __name__ == '__main__':
    main()
