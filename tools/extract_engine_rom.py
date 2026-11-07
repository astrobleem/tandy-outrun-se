"""Generate local-only engine lookup includes from user-supplied OutRun ROMs.

No ROM bytes are distributed by this tool. Inputs are the four 64 KiB main-CPU
chips from the OutRun revision B set, or a 256 KiB interleaved main-CPU image.
Generated includes must stay in the ignored build directory.
"""
import argparse
from pathlib import Path

CHIPS = (
    ("epr-10380b.133", 0x00000),
    ("epr-10382b.118", 0x00001),
    ("epr-10381b.132", 0x20000),
    ("epr-10383b.117", 0x20001),
)
SEGMENTS = ((0x1000, 512), (0x2200, 1168), (0x30800, 256),
            (0x1212A, 1024), (0x4CFA, 160))


def load_rom(path):
    path = Path(path)
    if path.is_file():
        data = path.read_bytes()
        if len(data) != 0x40000:
            raise ValueError("Interleaved main-CPU ROM must be exactly 256 KiB")
        return data
    data = bytearray(0x40000)
    for name, offset in CHIPS:
        chip = (path / name).read_bytes()
        if len(chip) != 0x10000:
            raise ValueError("%s must be exactly 64 KiB" % name)
        data[offset:offset + 0x20000:2] = chip
    return bytes(data)


def declaration(name, data):
    rows = [",".join("0x%02X" % value for value in data[i:i+16])
            for i in range(0, len(data), 16)]
    return "static const uint8_t %s[%d]={\n%s\n};\n" % (
        name, len(data), ",\n".join(rows))


def extract(rom_path, output):
    data = load_rom(rom_path)
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)
    segments = "".join(declaration("ROM_SEG_%d" % i, data[start:start+size])
                       for i, (start, size) in enumerate(SEGMENTS))
    (output / "engine_rom_segments.inc").write_text(segments, encoding="ascii")
    (output / "engine_rom_traffic.inc").write_text(
        declaration("RUNTIME_TRAFFIC_ROM", data[0x5424:0x5524]), encoding="ascii")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--roms", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=Path(__file__).resolve().parents[1] / "build/engine")
    args = parser.parse_args()
    extract(args.roms, args.output)
    print("Generated local ROM includes in", args.output)
