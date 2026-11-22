"""Regression tests for conservative hardware/unknown classification (Phase 1.1)."""
import pathlib
import unittest

import tools.host_hw_classification as hwc


def _fn(addr, out_degree=0, unresolved_indirect=0, bios_call=None, sig=None):
    return {
        "addr": addr,
        "name": f"func_{addr:08X}",
        "out_degree": out_degree,
        "unresolved_indirect": unresolved_indirect,
        "bios_call": bios_call,
        "sig": sig or {},
    }


class SyntheticClassificationTests(unittest.TestCase):
    def test_missing_static_edge_from_object_propagates_hw(self):
        # Static edges.json omits A -> B, but compiled object A has undefined func_B.
        a, b = 0x80010000, 0x80010100
        byaddr = {
            a: _fn(a),
            b: _fn(b, sig={"gte": 1}),
        }
        src_units = {a: pathlib.Path("A.c"), b: pathlib.Path("B.c")}
        obj_info = {
            "und": {a: {f"func_{b:08X}"}},
            "dfn_global": {a: {f"func_{a:08X}"}, b: {f"func_{b:08X}"}},
            "relocs": {a: {f"func_{b:08X}"}},
            "obj_hw": {},
            "obj_jalr": set(),
        }
        res = hwc.classify_conservative(byaddr, [], [], src_units, obj_info)
        self.assertEqual(res["state"][b], "hw")
        self.assertEqual(res["state"][a], "hw")
        self.assertEqual(res["missing_static_edges"][a], [b])

    def test_missing_static_edge_to_unknown_propagates_unknown(self):
        a, b = 0x80010000, 0x80010100
        byaddr = {
            a: _fn(a),
            b: _fn(b, unresolved_indirect=1),
        }
        src_units = {a: pathlib.Path("A.c"), b: pathlib.Path("B.c")}
        obj_info = {
            "und": {a: {f"func_{b:08X}"}},
            "dfn_global": {a: {f"func_{a:08X}"}, b: {f"func_{b:08X}"}},
            "relocs": {a: {f"func_{b:08X}"}},
            "obj_hw": {},
            "obj_jalr": set(),
        }
        res = hwc.classify_conservative(byaddr, [], [], src_units, obj_info)
        self.assertEqual(res["state"][b], "unknown")
        self.assertEqual(res["state"][a], "unknown")

    def test_object_jalr_and_indirect_call_ptr_are_never_candidate(self):
        a, b, c = 0x80010000, 0x80010100, 0x80010200
        byaddr = {a: _fn(a), b: _fn(b), c: _fn(c)}
        src_units = {x: pathlib.Path(f"{x:08X}.c") for x in (a, b, c)}
        indirect_raw = [
            {"func": b, "kind": "jalr", "classification": "call_ptr:static_slot", "targets": [c]},
        ]
        obj_info = {
            "und": {},
            "dfn_global": {x: {f"func_{x:08X}"} for x in (a, b, c)},
            "relocs": {},
            "obj_hw": {},
            "obj_jalr": {a},
        }
        res = hwc.classify_conservative(byaddr, [], indirect_raw, src_units, obj_info)
        self.assertEqual(res["state"][a], "unknown")
        self.assertEqual(res["state"][b], "unknown")
        self.assertEqual(res["state"][c], "candidate")

    def test_obj_cop0_and_scratchpad_lui_and_mmio_sym_classify_as_hw(self):
        a, b, c, d = 0x80010000, 0x80010100, 0x80010200, 0x80010300
        byaddr = {x: _fn(x) for x in (a, b, c, d)}
        src_units = {x: pathlib.Path(f"{x:08X}.c") for x in (a, b, c, d)}
        disasm = (
            f"{a:08X}.o:     file format elf32-tradlittlemips\n"
            "   0:\t40046000 \tmfc0\ta0,$12\n"
            f"{b:08X}.o:     file format elf32-tradlittlemips\n"
            "   0:\t3c1d1f80 \tlui\tsp,0x1f80\n"
            f"{d:08X}.o:     file format elf32-tradlittlemips\n"
            "   0:\t0040f809 \tjalr\tv0\n"
        )
        obj_hw, obj_jalr = hwc.parse_objdump_disasm(disasm)
        obj_info = {
            "und": {c: {"D_1F801070"}},
            "dfn_global": {x: {f"func_{x:08X}"} for x in (a, b, c, d)},
            "relocs": {c: {"D_1F801070"}},
            "obj_hw": obj_hw,
            "obj_jalr": obj_jalr,
        }
        res = hwc.classify_conservative(byaddr, [], [], src_units, obj_info)
        self.assertEqual(res["state"][a], "hw")
        self.assertEqual(res["state"][b], "hw")
        self.assertEqual(res["state"][c], "hw")
        self.assertEqual(res["state"][d], "unknown")

    def test_export_mismatch_is_tracked_separately_from_hw(self):
        a, b = 0x80096748, 0x8002B8B0
        byaddr = {a: _fn(a), b: _fn(b, out_degree=1)}
        edges_raw = [{"from": b, "to": a, "kind": "direct"}]
        src_units = {a: pathlib.Path("80096748.c"), b: pathlib.Path("8002B8B0.c")}
        obj_info = {
            "und": {b: {"func_80096748"}},
            "dfn_global": {a: {"gDivModUnsigned64"}, b: {"func_8002B8B0"}},
            "relocs": {b: {"func_80096748"}},
            "obj_hw": {},
            "obj_jalr": set(),
        }
        res = hwc.classify_conservative(byaddr, edges_raw, [], src_units, obj_info)
        self.assertEqual(res["state"][a], "candidate")
        self.assertEqual(res["state"][b], "candidate")
        self.assertIn(a, res["export_mismatches"])
        self.assertEqual(res["export_blocked"][b], [a])

    def test_nonstandard_relocated_symbol_and_compile_error_and_cycle_are_unknown(self):
        a, b, c, d = 0x80010000, 0x80010100, 0x80010200, 0x80010300
        byaddr = {x: _fn(x) for x in (a, b, c, d)}
        edges_raw = [
            {"from": c, "to": d, "kind": "direct"},
            {"from": d, "to": c, "kind": "direct"},
        ]
        src_units = {x: pathlib.Path(f"{x:08X}.c") for x in (a, b, c, d)}
        obj_info = {
            "und": {a: {"mystery_ext_sym"}},
            "dfn_global": {x: {f"func_{x:08X}"} for x in (a, c, d)},
            "relocs": {a: {"mystery_ext_sym"}},
            "obj_hw": {},
            "obj_jalr": set(),
        }
        res = hwc.classify_conservative(
            byaddr, edges_raw, [], src_units, obj_info, compile_errors={b: "err"}
        )
        self.assertEqual(res["state"][a], "unknown")
        self.assertEqual(res["state"][b], "unknown")
        self.assertEqual(res["state"][c], "unknown")
        self.assertEqual(res["state"][d], "unknown")

    def test_hw_precedes_unknown(self):
        a, b = 0x80010000, 0x80010100
        byaddr = {a: _fn(a, unresolved_indirect=1), b: _fn(b, bios_call="A0:3F")}
        edges_raw = [{"from": a, "to": b, "kind": "direct"}]
        src_units = {a: pathlib.Path("A.c"), b: pathlib.Path("B.c")}
        obj_info = {
            "und": {},
            "dfn_global": {a: {f"func_{a:08X}"}, b: {f"func_{b:08X}"}},
            "relocs": {},
            "obj_hw": {},
            "obj_jalr": set(),
        }
        res = hwc.classify_conservative(byaddr, edges_raw, [], src_units, obj_info)
        self.assertEqual(res["state"][a], "hw")


class LiveRepoClassificationRegressionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        _, cls.byaddr, cls.src_units, cls.result = hwc.run_classification()

    def test_zero_compile_errors_and_partition_complete(self):
        self.assertEqual(self.result["compile_errors"], {})
        for addr in self.src_units:
            self.assertIn(self.result["state"][addr], {"candidate", "hw", "unknown"})

    def test_key_units_classified_conservatively(self):
        st = self.result["state"]
        # Direct COP0 & scratchpad/MMIO units missed by analysis.json must be hw
        self.assertEqual(st[0x8008C9C0], "hw")
        self.assertEqual(st[0x8008C890], "hw")
        self.assertEqual(st[0x80026D9C], "hw")
        self.assertEqual(st[0x80065CA8], "hw")
        # Static-clean callers with missing object edges to unknown or object jalr must be unknown
        self.assertEqual(st[0x80049564], "unknown")
        self.assertEqual(st[0x800616A0], "unknown")
        self.assertEqual(st[0x8004DE80], "unknown")
        self.assertEqual(st[0x80082D30], "unknown")
        # These export-mismatch units are candidates, not verified hardware-free.
        self.assertEqual(st[0x80096748], "candidate")
        self.assertEqual(st[0x8007D138], "candidate")
        self.assertIn(0x80096748, self.result["export_mismatches"])
        self.assertIn(0x8007D138, self.result["export_mismatches"])
        self.assertEqual(self.result["export_mismatches"][0x80096748]["exported_globals"], ["gDivModUnsigned64"])
        self.assertEqual(self.result["export_mismatches"][0x8007D138]["exported_globals"], ["D_8007D138"])

    def test_no_candidate_unit_has_unresolved_or_object_jalr(self):
        for addr in self.src_units:
            if self.result["state"][addr] == "candidate":
                self.assertEqual(self.result["hw_reasons"].get(addr, []), [])
                self.assertEqual(self.result["incomplete_reasons"].get(addr, []), [])
                self.assertEqual(self.byaddr[addr].get("unresolved_indirect", 0), 0)


if __name__ == "__main__":
    unittest.main()
