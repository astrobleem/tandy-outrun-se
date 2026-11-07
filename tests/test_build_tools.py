"""Asset-boundary and preflight tests. No proprietary game data required."""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import extract_game_data as game
import extract_engine_rom as engine
import build_game


class BuildToolsTests(unittest.TestCase):
    def test_game_rejects_wrong_executable_before_writing(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / "TOSE.COM").write_bytes(b"not the game")
            with self.assertRaises(ValueError):
                game.extract(path, path / "output")
            self.assertFalse((path / "output").exists())

    def test_game_rejects_same_size_wrong_content(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "TOSE.COM"
            path.write_bytes(bytes(game.REFERENCE_BYTES))
            with self.assertRaises(ValueError):
                game.load_reference(path)

    def test_game_ranges_are_bounded(self):
        for _, start, end, _ in game.DATA:
            self.assertGreaterEqual(start, game.COM_ORIGIN)
            self.assertGreater(end, start)
            self.assertLessEqual(end - game.COM_ORIGIN, game.REFERENCE_BYTES)

    def test_byte_directives_preserve_values(self):
        values = bytes(range(256))
        text = game.byte_directives(values)
        decoded = bytes(int(value) for row in text.splitlines()
                        for value in row.removeprefix("db ").split(","))
        self.assertEqual(decoded, values)

    def test_engine_rejects_wrong_sized_image(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "rom.bin"
            path.write_bytes(b"invalid")
            with self.assertRaises(ValueError):
                engine.load_rom(path)

    def test_engine_chip_interleave(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            for value, (name, _) in enumerate(engine.CHIPS):
                (path / name).write_bytes(bytes([value]) * 65536)
            result = engine.load_rom(path)
            self.assertEqual(result[:8], bytes([0, 1]) * 4)
            self.assertEqual(result[0x20000:0x20008], bytes([2, 3]) * 4)
            self.assertEqual(len(result), 262144)

    def test_missing_assembler_does_not_create_output(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            with self.assertRaises(FileNotFoundError):
                build_game.build(path, path / "missing.exe", path / "output")
            self.assertFalse((path / "output").exists())

    def test_public_game_includes_are_resolved(self):
        source = ROOT / "src/game"
        private = {item[0].lower() for item in game.DATA}
        import re
        for path in source.iterdir():
            for include in re.findall(r"(?im)^include\s+(\S+)", path.read_text()):
                self.assertTrue((source / include).is_file() or include.lower() in private,
                                str(path) + ": " + include)


if __name__ == "__main__":
    unittest.main()
