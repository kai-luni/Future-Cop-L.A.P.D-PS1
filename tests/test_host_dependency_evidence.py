"""Regression tests for conservative host-test evidence."""
import unittest
from tools.host_test_evidence import classify_graph, classify_address, read_image_range


class EvidenceTests(unittest.TestCase):
    def test_missing_dependency_is_not_clean(self):
        states = classify_graph({1, 2, 3}, {1: {2}, 2: {3}}, set(), {3})
        self.assertEqual(states, {1: 'unknown', 2: 'unknown', 3: 'unknown'})

    def test_hardware_wins_over_unknown(self):
        states = classify_graph({1, 2, 3}, {1: {2, 3}}, {2}, {3})
        self.assertEqual(states[1], 'hw')

    def test_cycle_without_missing_evidence_is_not_implicitly_safe(self):
        states = classify_graph({1, 2}, {1: {2}, 2: {1}}, set(), set())
        self.assertEqual(states, {1: 'unknown', 2: 'unknown'})

    def test_known_leaf_is_candidate_not_proof(self):
        self.assertEqual(classify_graph({1}, {}, set(), set())[1], 'candidate')

    def test_image_range_requires_valid_psx_header_and_loaded_bytes(self):
        import struct
        image = bytearray(0x804)
        image[:8] = b'PS-X EXE'
        struct.pack_into('<II', image, 0x18, 0x80010000, 4)
        image[0x800:] = b'\0\x01\0\x02'
        self.assertEqual(read_image_range(image, 0x80010001, 2), b'\x01\0')
        self.assertIsNone(read_image_range(image, 0x80010003, 2))
        self.assertIsNone(read_image_range(image[:-1], 0x80010000, 4))
        image[0] = 0
        self.assertIsNone(read_image_range(image, 0x80010000, 1))

    def test_address_classification_never_assumes_bss(self):
        image = bytes([0, 0, 2, 0])
        self.assertEqual(classify_address(0x1000, 2, image, 0x1000), 'image-zero')
        self.assertEqual(classify_address(0x1001, 2, image, 0x1000), 'image-nonzero')
        self.assertEqual(classify_address(0x1003, 2, image, 0x1000), 'unknown')
        self.assertEqual(classify_address(0x2000, 4, image, 0x1000), 'unknown')
        self.assertEqual(classify_address(0x1000, 0, image, 0x1000), 'unknown')
