"""Build the 8088 engine bank with Open Watcom 1.9 on Windows.

Example:
  python tools/build_engine.py --roms path/to/outrun --watcom C:/WATCOM

The source is the opt8-lazy6-s1k engine used by the w16h1-320 game. The bank
uses 8088 code, small model, speed optimization and a 1024-byte reserved stack.
ROM includes and all compiler output are generated under build/engine only.
"""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
from extract_engine_rom import extract


def build(roms, watcom, output):
    root = Path(__file__).resolve().parents[1]
    output = output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    source = root / "src/engine"
    for item in source.iterdir():
        if item.is_file() and item.suffix in (".hpp", ".inc", ".cpp", ".asm"):
            shutil.copyfile(item, output / item.name)
    extract(roms, output)
    watcom = watcom.resolve()
    env = os.environ.copy()
    env["WATCOM"] = str(watcom)
    env["PATH"] = str(watcom / "BINNT") + os.pathsep + env.get("PATH", "")
    env["INCLUDE"] = str(watcom / "H")
    commands = (
        [watcom / "BINNT/wasm.exe", "-0", "-q", "native_mul_s16_s16.asm"],
        [watcom / "BINNT/wcl.exe", "-0", "-ms", "-ot", "-s", "-l=com", "-q",
         "-fe=runtime_kernel_r20_distance_cache",
         "-fm=runtime_kernel_r20_distance_cache",
         "runtime_kernel_r17_crash_y.cpp", "native_mul_s16_s16.obj"],
    )
    for command in commands:
        subprocess.run([str(arg) for arg in command], cwd=output, env=env, check=True)
    layout = (output / "runtime_kernel_r20_distance_cache.map").read_text()
    group = re.search(r"(?m)^DGROUP\s+0000:0000\s+([0-9a-fA-F]+)", layout)
    if not group:
        raise RuntimeError("DGROUP missing from compiler map")
    def symbol(name):
        match = re.search(r"(?m)^\s*0000:([0-9a-fA-F]+)[*+]?\s+_" +
                          name + r"\s*$", layout)
        if not match:
            raise RuntimeError("Missing engine symbol: " + name)
        return int(match.group(1), 16)
    raw = (output / "runtime_kernel_r20_distance_cache.com").read_bytes()
    canary = int(group.group(1), 16) + 256
    size = (max(canary, len(raw) + 256) + 1024 + 15) & ~15
    bank = bytes(256) + raw
    bank += bytes(size - len(bank))
    # The current game assembly binds this exact bank ABI.
    actual = (symbol("runtime_entry"), symbol("runtime_ipc"), canary, size)
    expected = (22462, 26048, 36422, 37456)
    if actual != expected:
        raise RuntimeError("Engine ABI differs from w16h1-320: %r != %r. "
                           "Do not pair this bank with that game." % (actual, expected))
    (output / "ENGINE.BIN").write_bytes(bank)
    print("ENGINE.BIN: %d bytes; entry %d; IPC %d; canary %d; stack top %d" %
          (size, actual[0], actual[1], canary, size - 16))
    return output / "ENGINE.BIN"


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--roms", type=Path, required=True)
    parser.add_argument("--watcom", type=Path,
                        default=Path(os.environ.get("WATCOM", "C:/WATCOM")))
    parser.add_argument("--output", type=Path,
                        default=Path(__file__).resolve().parents[1] / "build/engine")
    args = parser.parse_args()
    build(args.roms, args.watcom, args.output)
