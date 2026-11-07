"""Recompile OR-H320's executable sprite and sky banks from private pixel data.

Inputs are the existing OR-H320 package, never downloaded or distributed here.
SPRITES.BIN retains six player poses and traffic mask/value pixels. Two extra
player poses survive only as immediate operands in PLYRCODE.BIN; a strict
decoder recovers their mask/value pixels before recompiling all eight poses.
Sky banks retain their raw pixels after the code and row-pointer tables.
Every generated bank must equal the private reference before any output writes.
"""
import argparse
from pathlib import Path
import struct
from extract_game_data import load_reference

POSES = (0, 1, 2, 68, 69, 70, 71, 72)
SKY_POINTERS = (1724, 662, 752, 1292, 1256, 722, 758, 884,
                878, 542, 980, 1100, 836, 884, 1028)


def u16(data, offset):
    if offset < 0 or offset + 2 > len(data):
        raise ValueError("16-bit field outside bank")
    return struct.unpack_from("<H", data, offset)[0]


def sprite_rows(bank, frame):
    offset = u16(bank, frame * 2)
    if not offset:
        raise ValueError("Sprite frame has no retained raw pixels")
    width, height = u16(bank, offset), u16(bank, offset + 2)
    if not 0 < width <= 127 or not 0 < height <= 200:
        raise ValueError("Unsupported raw sprite dimensions or compression")
    end = offset + 4 + width * height * 2
    if end > len(bank):
        raise ValueError("Sprite payload exceeds bank")
    raw = bank[offset+4:end]
    return [[tuple(raw[(y*width+x)*2:(y*width+x)*2+2]) for x in range(width)]
            for y in range(height)]


def compile_sprite(rows, phase):
    """Emit 8088 ES:[DI+disp8] mask/write instructions and Tandy row strides."""
    if not rows or not 0 < len(rows[0]) <= 127:
        raise ValueError("Invalid sprite row width")
    width = len(rows[0])
    code = bytearray()
    for y, row in enumerate(rows):
        if len(row) != width:
            raise ValueError("Inconsistent sprite row widths")
        x = 0
        while x < width:
            mask, value = row[x]
            if not (0 <= mask <= 255 and 0 <= value <= 255):
                raise ValueError("Mask/value must be bytes")
            if mask == 255:
                x += 1
                continue
            if mask == 0 and x + 1 < width and row[x+1][0] == 0:
                code.extend((0x26, 0xC7, 0x45, x, value, row[x+1][1]))
                x += 2
                continue
            if mask == 0:
                code.extend((0x26, 0xC6, 0x45, x, value))
            else:
                code.extend((0x26, 0x80, 0x65, x, mask))
                if value:
                    code.extend((0x26, 0x80, 0x4D, x, value))
            x += 1
        if y != len(rows) - 1:
            stride = 8192 if (phase + y) % 4 != 3 else -24416
            code.extend((0x81, 0xC7, stride & 255, (stride >> 8) & 255))
    code.append(0xCB)
    return bytes(code)


def decode_player(code, width=40, height=32):
    """Recover pixels from the known compiler's instruction subset, not execute it."""
    rows = [[(255, 0) for _ in range(width)] for _ in range(height)]
    pos, y = 0, 0
    while pos < len(code):
        if code[pos] == 0xCB:
            if pos != len(code)-1 or y != height-1:
                raise ValueError("Unexpected player return/height")
            if compile_sprite(rows, 0) != code:
                raise ValueError("Player code is not a canonical pixel program")
            return rows
        if code[pos:pos+2] == b"\x81\xc7":
            expected = 8192 if y % 4 != 3 else (-24416 & 65535)
            if u16(code, pos+2) != expected or y >= height-1:
                raise ValueError("Unexpected player row stride")
            pos += 4
            y += 1
            continue
        op = code[pos:pos+3]
        size = 6 if op == b"\x26\xc7\x45" else 5
        if pos + size > len(code):
            raise ValueError("Truncated player instruction")
        x = code[pos+3]
        if x >= width:
            raise ValueError("Player write exceeds row")
        value = code[pos+4]
        if op == b"\x26\xc7\x45":
            if x+1 >= width:
                raise ValueError("Player word write exceeds row")
            rows[y][x], rows[y][x+1] = (0,value), (0,code[pos+5])
        elif op == b"\x26\xc6\x45":
            rows[y][x] = (0,value)
        elif op == b"\x26\x80\x65":
            rows[y][x] = (value,0)
        elif op == b"\x26\x80\x4d":
            rows[y][x] = (rows[y][x][0],value)
        else:
            raise ValueError("Unsupported player opcode")
        pos += size
    raise ValueError("Player program has no final return")


def compile_player(sprites, old_code):
    code = bytearray(len(POSES)*2)
    for index, frame in enumerate(POSES):
        if frame < 71:
            rows = sprite_rows(sprites, frame)
        else:
            start = u16(old_code, index*2)
            end = u16(old_code, (index+1)*2) if index+1 < len(POSES) else len(old_code)
            if not 16 <= start < end <= len(old_code):
                raise ValueError("Invalid private player pose boundary")
            rows = decode_player(old_code[start:end])
        if len(rows) != 32 or len(rows[0]) != 40:
            raise ValueError("Expected 80x32 player pose")
        struct.pack_into("<H", code, index*2, len(code))
        code.extend(compile_sprite(rows, 152))
    return bytes(code)


def compile_objects(sprites):
    banks, table = [bytearray()], []
    for frame in range(71):
        for phase in range(4):
            if not 3 <= frame < 20:
                table.extend((0,0))
                continue
            code = compile_sprite(sprite_rows(sprites,frame),phase)
            if len(banks[-1]) + len(code) > 60000:
                banks.append(bytearray())
            table.extend((16 + (len(banks)-1)*2, len(banks[-1])))
            banks[-1].extend(code)
    if len(banks) > 3:
        raise ValueError("Object code exceeds three banks")
    while len(banks) < 3:
        banks.append(bytearray((0xCB,)))
    return [bytes(bank) for bank in banks], table


def sky_rows(bank, pointer_base):
    if pointer_base < 368 or pointer_base + 368 > len(bank):
        raise ValueError("Invalid sky pointer table")
    rows = []
    for y in range(184):
        offset = u16(bank, pointer_base+y*2)
        if offset < pointer_base+368 or offset+160 > len(bank):
            raise ValueError("Sky raw row exceeds bank")
        rows.append(bank[offset:offset+160])
    return rows


def compile_sky(rows):
    """Emit 184 row tokens, short run programs/raw-copy stubs, pointers and pixels."""
    if len(rows) != 184 or any(len(row) != 160 for row in rows):
        raise ValueError("Sky must contain 184 packed 320-pixel rows")
    unique = list(dict.fromkeys(rows))
    programs = []
    for row in unique:
        code, x = bytearray(), 0
        while x < len(row):
            word, count = row[x:x+2], 1
            while x+2*(count+1) <= len(row) and row[x+2*count:x+2*count+2] == word:
                count += 1
            code += b"\xb8"+word+b"\xb9"+struct.pack("<H",count)+b"\x9c\xfa\xf3\xab\x9d"
            x += 2*count
        code.append(0xCB)
        programs.append(bytes(code) if len(code) <= 18 else None)
    pointer_base = 368 + sum(len(code) if code else 18 for code in programs)
    raw_base = pointer_base + 368
    offsets = {row:raw_base+i*160 for i,row in enumerate(unique)}
    if raw_base+len(unique)*160 > 65536:
        raise ValueError("Sky exceeds 16-bit bank")
    bank, tokens = bytearray(368), {}
    for row,code in zip(unique,programs):
        tokens[row] = len(bank)
        bank += code if code else (b"\x1e\x0e\x1f\x56\xbe"+struct.pack("<H",offsets[row])+
                                  b"\xb9\x50\x00\x9c\xfa\xf3\xa5\x9d\x5e\x1f\xcb")
    for y,row in enumerate(rows):
        struct.pack_into("<H",bank,y*2,tokens[row])
    bank += b"".join(struct.pack("<H",offsets[row]) for row in rows)
    bank += b"".join(unique)
    return bytes(bank), pointer_base


def build(assets, output):
    assets, output = Path(assets).resolve(), Path(output).resolve()
    if not assets.is_dir():
        raise ValueError("--assets must be the private OR-H320 folder")
    if output == assets:
        raise ValueError("Output must not overwrite the private package")
    load_reference(assets)
    sprites = (assets/"SPRITES.BIN").read_bytes()
    old_player = (assets/"PLYRCODE.BIN").read_bytes()
    generated = {"PLYRCODE.BIN":compile_player(sprites,old_player)}
    objects, table = compile_objects(sprites)
    for i,bank in enumerate(objects):
        generated["OBJCOD%d.BIN" % i] = bank
    for name,pointer in [("CAMSKY.BIN",SKY_POINTERS[0])]+[
            ("CSK%02d.BIN" % i,p) for i,p in enumerate(SKY_POINTERS)]:
        bank,new_pointer = compile_sky(sky_rows((assets/name).read_bytes(),pointer))
        if new_pointer != pointer:
            raise ValueError("Sky pointer ABI changed: "+name)
        generated[name] = bank
    # This tool reconstructs the current snapshot, not arbitrary replacement art.
    # Reject mismatched inputs or any change in compiler output before writing.
    for name,data in generated.items():
        if data != (assets/name).read_bytes():
            raise ValueError("Recompiled bank differs from OR-H320: "+name)
    output.mkdir(parents=True,exist_ok=True)
    for name,data in generated.items():
        (output/name).write_bytes(data)
        print("%s: %d bytes, byte-identical" % (name,len(data)))
    return generated


if __name__ == "__main__":
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets",type=Path,required=True)
    parser.add_argument("--output",type=Path,
                        default=Path(__file__).resolve().parents[1]/"build/draw")
    args=parser.parse_args()
    build(args.assets,args.output)
