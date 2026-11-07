"""Assemble the current 320x200 DOS game without distributing its asset data.

  python tools/build_game.py --assets path/to/OR-H320 --jwasm path/to/jwasm.exe
Add --verify-reference to require exact equality with the retained executable.
"""
import argparse
from pathlib import Path
import shutil
import subprocess
from extract_game_data import extract


def build(assets, jwasm, output, verify_reference=False):
    root = Path(__file__).resolve().parents[1]
    tool = Path(jwasm).resolve()
    if not tool.is_file():
        raise FileNotFoundError("JWasm executable not found: " + str(tool))
    output = Path(output).resolve()
    reference = extract(assets, output)
    for item in (root / "src/game").iterdir():
        if item.is_file() and item.suffix.upper() in (".ASM", ".INC"):
            shutil.copyfile(item, output / item.name)
    subprocess.run([str(tool), "-q", "-bin", "-Fo=TOSE.COM",
                    "-Fl=TOSE.LST", "TOSE.ASM"], cwd=output, check=True)
    result = output / "TOSE.COM"
    if verify_reference and result.read_bytes() != reference:
        raise RuntimeError("Rebuilt TOSE.COM differs from OR-H320")
    print("TOSE.COM:", result.stat().st_size, "bytes")
    if verify_reference:
        print("Byte-identical to the supplied OR-H320 reference.")
    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets", type=Path, required=True,
                        help="Private OR-H320 folder or its TOSE.COM")
    parser.add_argument("--jwasm", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=Path(__file__).resolve().parents[1] / "build/game")
    parser.add_argument("--verify-reference", action="store_true")
    args = parser.parse_args()
    build(args.assets, args.jwasm, args.output, args.verify_reference)
