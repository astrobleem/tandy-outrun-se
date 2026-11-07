"""Synthetic pixel/compiler tests; no game assets required."""
from pathlib import Path
import sys
import unittest

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/"tools"))
import build_draw_banks as draw


class DrawCodeTests(unittest.TestCase):
    def test_sprite_roundtrip_masked_opaque_transparent(self):
        row=[(255,0),(0,18),(0,52),(15,160),(240,5),(0,103)]
        rows=[row[:] for _ in range(8)]
        code=draw.compile_sprite(rows,0)
        self.assertEqual(draw.decode_player(code,len(row),len(rows)),rows)

    def test_transparent_player_is_strides_and_return(self):
        rows=[[(255,0)]*40 for _ in range(32)]
        code=draw.compile_sprite(rows,0)
        self.assertEqual(len(code),31*4+1)
        self.assertEqual(draw.decode_player(code),rows)

    def test_player_rejects_unknown_opcode(self):
        with self.assertRaises(ValueError):
            draw.decode_player(b"\x90\x90\x90\x00\x00\xcb")

    def test_player_rejects_truncated_instruction(self):
        with self.assertRaises(ValueError):
            draw.decode_player(b"\x26\xc7\x45")

    def test_sprite_rejects_ragged_rows(self):
        with self.assertRaises(ValueError):
            draw.compile_sprite([[(0,0)],[(0,0),(0,0)]],0)

    def test_sky_uniform_uses_short_program(self):
        rows=[b"\x12\x34"*80]*184
        bank,pointer=draw.compile_sky(rows)
        self.assertEqual(pointer,380)
        self.assertEqual(draw.sky_rows(bank,pointer),rows)
        self.assertEqual(bank[368:380],b"\xb8\x12\x34\xb9\x50\x00\x9c\xfa\xf3\xab\x9d\xcb")

    def test_sky_complex_uses_copy_stub_and_deduplicates(self):
        a=bytes(range(160))
        b=bytes(reversed(range(160)))
        rows=[a,b]*92
        bank,pointer=draw.compile_sky(rows)
        self.assertEqual(pointer,404)
        self.assertEqual(draw.sky_rows(bank,pointer),rows)
        self.assertEqual(len(bank),404+368+320)

    def test_sky_rejects_invalid_row_offset(self):
        with self.assertRaises(ValueError):
            draw.sky_rows(bytes(736),368)

    def test_sky_rejects_wrong_height(self):
        with self.assertRaises(ValueError):
            draw.compile_sky([bytes(160)]*183)

    def test_sprite_rejects_missing_frame(self):
        with self.assertRaises(ValueError):
            draw.sprite_rows(bytes(256),0)


if __name__=="__main__":
    unittest.main()
