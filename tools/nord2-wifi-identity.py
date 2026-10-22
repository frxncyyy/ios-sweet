#!/usr/bin/env python3
"""Publish the saved Nord 2 factory Wi-Fi identity before any user process.

Consume the existing, pinned private calibration in an offline ADT. No device
access or address literal; output remains a private device-specific artifact.
The native controller independently checks equality against its board address.
"""
import argparse
import hashlib
import os
from pathlib import Path

import adt

BOARD_BYTES = 6146
BOARD_SHA256 = "61ac3300aad6f65364f845894bb982b7c5d077346f788c29df019c93c4f3f89f"
CALIBRATION = "nord2-wifi-calibration"
IDENTITY = "mac-address-wifi0"
MAX_ADT_BYTES = 1024 * 1024


def single_child(parent, name):
    children = [child for child in parent.children if child.name == name]
    if len(children) != 1:
        raise ValueError("missing or duplicate required node: " + name)
    child = children[0]
    names = [key for key, _, _ in child.props]
    if len(names) != len(set(names)):
        raise ValueError("duplicate properties on required node: " + name)
    return child


def factory_address(board):
    if len(board) != BOARD_BYTES or hashlib.sha256(board).hexdigest() != BOARD_SHA256:
        raise ValueError("board calibration provenance mismatch")
    # Same offset and nonzero/unicast requirement as nni_address_select.
    mac = board[4:10]
    if len(mac) != 6 or not any(mac) or mac[0] & 1:
        raise ValueError("invalid factory Wi-Fi identity")
    return mac


def add(root):
    chosen = single_child(root, "chosen")
    arm_io = single_child(root, "arm-io")
    wlan = single_child(arm_io, "wlan")
    if wlan.prop("compatible") != b"nord2,mt6893-wifi\0":
        raise ValueError("native Wi-Fi service is not selected")
    calibration = [(flags, value) for key, flags, value in chosen.props if key == CALIBRATION]
    if len(calibration) != 1 or calibration[0][0] != 0:
        raise ValueError("missing or placeholder board calibration")
    mac = factory_address(calibration[0][1])
    previous = [(flags, value) for key, flags, value in chosen.props if key == IDENTITY]
    if previous:
        if previous != [(0, mac)]:
            raise ValueError("conflicting or placeholder factory identity")
    else:
        chosen.props.append((IDENTITY, 0, mac))
    return root


def transform(blob):
    if not blob or len(blob) > MAX_ADT_BYTES:
        raise ValueError("unexpected ADT extent")
    root, consumed = adt.parse(blob)
    if consumed != len(blob) or adt.serialize(root) != blob:
        raise ValueError("noncanonical or truncated ADT")
    result = adt.serialize(add(root))
    checked, end = adt.parse(result)
    if end != len(result) or adt.serialize(add(checked)) != result:
        raise ValueError("identity ADT round trip failed")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input_adt", type=Path)
    parser.add_argument("output_adt", type=Path)
    args = parser.parse_args()
    if args.input_adt.resolve() == args.output_adt.resolve():
        raise SystemExit("output must be distinct from input")
    with args.input_adt.open("rb") as source:
        result = transform(source.read(MAX_ADT_BYTES + 1))
    descriptor = os.open(args.output_adt, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
    with os.fdopen(descriptor, "wb") as output:
        output.write(result)
    print(f"identity ADT: {len(result)} bytes; factory address verified, value not logged")


if __name__ == "__main__":
    main()
