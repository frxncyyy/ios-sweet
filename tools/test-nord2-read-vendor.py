#!/usr/bin/env python3
"""Corruption/boundary tests for the read-only LP firmware collector."""
import hashlib
import importlib.util
from pathlib import Path
import struct
import unittest

spec = importlib.util.spec_from_file_location(
    "collector", Path(__file__).with_name("nord2-read-vendor.py"))
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)


def fixture(extent_sector=2048, extent_source=0):
    geometry = bytearray(4096)
    struct.pack_into("<II", geometry, 0, 0x616c4467, 52)
    struct.pack_into("<III", geometry, 40, 4096, 1, 4096)
    geometry[8:40] = hashlib.sha256(geometry[:52]).digest()
    tables = [
        struct.pack("<36sIIII", b"vendor", 1, 0, 2, 0),
        struct.pack("<QIQI", 16, 0, extent_sector, extent_source) +
        struct.pack("<QIQI", 8, 1, 0, 0),
        struct.pack("<36sIQ", b"default", 0, 0),
        struct.pack("<QIIQ36sI", 2048, 1048576, 0, 2**30, b"super", 0),
    ]
    header = bytearray(128)
    struct.pack_into("<IHHI", header, 0, 0x414c5030, 10, 0, 128)
    data = b"".join(tables)
    struct.pack_into("<I", header, 44, len(data))
    header[48:80] = hashlib.sha256(data).digest()
    offset = 0
    for i, (table, count) in enumerate(zip(tables, (1, 2, 1, 1))):
        struct.pack_into("<III", header, 80 + i*12, offset, count, len(table)//count)
        offset += len(table)
    header[12:44] = hashlib.sha256(header).digest()
    return bytes(geometry), bytes(header + data).ljust(4096, b"\0")


class MetadataTests(unittest.TestCase):
    def parse(self, raw):
        geo = m.geometry(fixture()[0])
        return m.metadata(raw, geo, 2**30)

    def test_linear_and_zero_extent(self):
        parts, _ = self.parse(fixture()[1])
        self.assertEqual(parts, {"vendor": [
            {"offset": 1048576, "bytes": 8192},
            {"offset": None, "bytes": 4096}]})

    def test_geometry_and_checksum_corruption(self):
        geo = fixture()[0]
        for offset in (0, 4, 8, 40, 44, 48):
            bad = bytearray(geo)
            bad[offset] ^= 1
            with self.assertRaises(ValueError):
                m.geometry(bad)

    def test_metadata_corruption_and_truncation(self):
        raw = fixture()[1]
        for offset in (0, 4, 8, 12, 44, 48, 80, 128, 200):
            bad = bytearray(raw)
            bad[offset] ^= 1
            with self.assertRaises(ValueError):
                self.parse(bad)
        with self.assertRaises(ValueError):
            self.parse(raw[:-1])

    def test_out_of_bounds_even_with_valid_checksums(self):
        for sector, source in ((0, 0), (2**30//512, 0), (2048, 1)):
            with self.assertRaises(ValueError):
                self.parse(fixture(sector, source)[1])

    def test_rejects_mismatched_device_size(self):
        g, raw = fixture()
        with self.assertRaises(ValueError):
            m.metadata(raw, m.geometry(g), 2**30 + 4096)

    def test_forbids_non_metadata_buffered_reads(self):
        rescue = m.Rescue("/invalid/adb", "fixture")
        with self.assertRaises(ValueError):
            rescue.bytes(0, 2**30)
        with self.assertRaises(ValueError):
            rescue.read(1, 512, None)


if __name__ == "__main__":
    unittest.main()
