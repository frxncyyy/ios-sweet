#!/usr/bin/env python3
"""Test pre-userland identity packaging using synthetic board data only."""
import copy
import ctypes
import hashlib
import importlib.util
from pathlib import Path
import subprocess
import struct
import tempfile
from unittest.mock import patch

import adt

ROOT = Path(__file__).resolve().parents[1]


def module(name, file):
    spec = importlib.util.spec_from_file_location(name, ROOT / "tools" / file)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


identity = module("identity", "nord2-wifi-identity.py")
mkadt = module("nord2_adt", "mkadt-nord2.py")


def refuse(root):
    original = adt.serialize(root)
    try:
        identity.add(root)
    except ValueError:
        assert adt.serialize(root) == original
    else:
        raise AssertionError("invalid metadata accepted")


def main():
    board = bytearray(identity.BOARD_BYTES)
    board[4:10] = b"\x02\x10\x20\x30\x40\x50"
    root = mkadt.build(wifi_service=True)
    chosen = identity.single_child(root, "chosen")
    chosen.props.append((identity.CALIBRATION, 0, bytes(board)))
    # Real provenance rejects a synthetic board, irrespective of valid MAC.
    refuse(copy.deepcopy(root))
    with patch.object(identity, "BOARD_SHA256", hashlib.sha256(board).hexdigest()):
        old = adt.serialize(root)
        new = identity.transform(old)
        assert identity.transform(new) == new
        parsed, consumed = adt.parse(new)
        assert consumed == len(new)
        observed = identity.single_child(parsed, "chosen")
        assert observed.prop(identity.IDENTITY) == board[4:10]
        observed.props = [item for item in observed.props if item[0] != identity.IDENTITY]
        assert adt.serialize(parsed) == old  # Every unrelated node/byte retained.
        for replacement in (b"", bytes(6), bytes(5), bytes(7), b"different"):
            bad = copy.deepcopy(root)
            identity.single_child(bad, "chosen").props.append((identity.IDENTITY, 0, replacement))
            refuse(bad)
        for key in (identity.CALIBRATION, identity.IDENTITY):
            bad, _ = adt.parse(new)
            c = identity.single_child(bad, "chosen")
            value = c.prop(key)
            c.props.append((key, 0, value))
            refuse(bad)
            bad, _ = adt.parse(new)
            c = identity.single_child(bad, "chosen")
            c.props = [(k, 1 if k == key else f, v) for k, f, v in c.props]
            refuse(bad)
        missing = mkadt.build()
        identity.single_child(missing, "chosen").props.append((identity.CALIBRATION, 0, bytes(board)))
        refuse(missing)
        duplicate = copy.deepcopy(root)
        duplicate.children.append(copy.deepcopy(identity.single_child(duplicate, "chosen")))
        refuse(duplicate)
        for invalid in (old + b"trailing", old[:-1], b"", bytes(identity.MAX_ADT_BYTES + 1)):
            try:
                identity.transform(invalid)
            except (ValueError, IndexError, struct.error):
                pass
            else:
                raise AssertionError("invalid ADT accepted")
    # Cross-check the same factory bytes against the actual native selector.
    with tempfile.TemporaryDirectory(prefix="nord2-identity-") as directory:
        library = Path(directory) / "native.so"
        subprocess.run(["clang", "-shared", "-fPIC", "-Wall", "-Wextra", "-Werror",
                        str(ROOT / "shim/nord2-xnu/wifi_normal_init.c"),
                        "-o", str(library)], check=True)
        native = ctypes.CDLL(str(library))
        class Address(ctypes.Structure):
            _fields_ = [("mac", ctypes.c_uint8 * 6), ("source", ctypes.c_int)]
        native.nni_address_select.argtypes = [ctypes.c_void_p, ctypes.c_size_t,
                                             ctypes.c_void_p, ctypes.POINTER(Address)]
        for first in (0, 1, 2, 3, 0xfe, 0xff):
            for zero_tail in (False, True):
                value = bytearray(board)
                value[4:10] = bytes([first]) + (bytes(5) if zero_tail else board[5:10])
                raw = ctypes.create_string_buffer(bytes(value))
                actual = Address()
                accepted = native.nni_address_select(raw, len(value), None, ctypes.byref(actual)) == 0
                with patch.object(identity, "BOARD_SHA256", hashlib.sha256(value).hexdigest()):
                    try:
                        output = identity.factory_address(bytes(value))
                    except ValueError:
                        assert not accepted
                    else:
                        assert accepted and actual.source == 1 and output == bytes(actual.mac)
    print("PASS boot ADT identity: exact metadata/type/order, refusal, unchanged tree, native address agreement")


if __name__ == "__main__":
    main()
