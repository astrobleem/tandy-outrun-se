"""Recover local-only asset includes from a privately held OR-H320 package.

The public source does not contain the HUD pixels, startup tune, course lookup or road-ROM
width table. No game data is downloaded or bundled by this tool.
"""
import argparse
import hashlib
from pathlib import Path

# This identifies the supported executable input, not a redistributable asset.
REFERENCE_BYTES = 36117
REFERENCE_SHA256 = "e0a1ac24486d5bb83c9eeba04d73bb6a5c81146e532f8c507fd774629be11a95"
COM_ORIGIN = 0x100
# Logical offsets in the accepted w16h1-320 COM.
DATA = (
    ("SOURCE_ACTIVE.INC", 6147, 7122,
     "source_first_active label byte\n"),
    ("SPLTUNE.INC", 12577, 13647,
     "SPLASH_END_TICKS equ 215\nsplash_data label byte\n"),
    ("SOURCE_WIDTH.INC", 22692, 24740,
     "source_half_width label word\n"),
    ("HUD_FONT.INC", 27109, 28525,
     "hud_font label byte\n"),
)


def load_reference(path):
    path = Path(path)
    if path.is_dir():
        path = path / "TOSE.COM"
    raw = path.read_bytes()
    if len(raw) != REFERENCE_BYTES or hashlib.sha256(raw).hexdigest() != REFERENCE_SHA256:
        raise ValueError("Expected the exact w16h1-320 / OR-H320 TOSE.COM. "
                         "Other builds have different embedded-data offsets.")
    return raw


def byte_directives(data):
    return "".join("db " + ",".join(str(value) for value in data[i:i+16]) + "\n"
                   for i in range(0, len(data), 16))


def extract(reference, output):
    raw = load_reference(reference)
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)
    for name, start, end, header in DATA:
        payload = raw[start-COM_ORIGIN:end-COM_ORIGIN]
        body = byte_directives(payload)
        if name == "SOURCE_WIDTH.INC":
            body = (byte_directives(payload[:1024]) + "source_lane_width label word\n" +
                    byte_directives(payload[1024:]))
        text = "; Local game data. Do not redistribute.\n" + header + body
        (output / name).write_text(text, encoding="ascii")
    return raw


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets", type=Path, required=True,
                        help="Private OR-H320 folder or its TOSE.COM")
    parser.add_argument("--output", type=Path,
                        default=Path(__file__).resolve().parents[1] / "build/game")
    args = parser.parse_args()
    extract(args.assets, args.output)
    print("Generated local asset includes in", args.output)
