"""Transcribe A1/A2 inputs and assertion values; never derive them from QEMU."""
import json
from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.portable_tests import ROOT, SCHEMA, digest, signature, validate

sys.path.insert(0, str(ROOT / "tests/host"))
import leaf_vectors as vectors

OUT = Path(__file__).with_name("a1-a2.v1.json")


def typed(kind, payload):
    if kind.startswith(("u", "s")):
        payload = str(payload)
    elif kind == "ptr" and payload is not None:
        region, offset = (payload, 0) if isinstance(payload, str) else payload
        payload = {"region": region, "offset": offset}
    elif kind == "bytes":
        payload = bytes(payload).hex()
    return {"type": kind, "value": payload}


def region(id, size, fill=0):
    return {"id": id, "size": size, "fill": typed("u8", fill),
            "symbol": id if id.startswith("D_") else None}


def cell(region, offset, kind, value):
    return {"region": region, "offset": offset, "value": typed(kind, value)}


def words(region, values, offset=0, kind="s32"):
    size = int(kind[1:]) // 8
    return [cell(region, offset + i * size, kind, value) for i, value in enumerate(values)]


def signed32(value):
    return (value + (1 << 31)) % (1 << 32) - (1 << 31)


def packed(x, y):
    return signed32(((y & 0xffff) << 16) | (x & 0xffff))


class Fixtures:
    def __init__(self):
        self.data = {"schema": SCHEMA, "version": 1, "byte_order": "little", "functions": {}, "cases": []}
        self.origins = {}
        for suite in ("a1", "a2"):
            source = (ROOT / f"tests/host/leaf_{suite}_harness.c").read_text()
            for addr, evidence, note in re.findall(r"/\* @leaf ([A-F0-9]{8}) (\w+) (.*?) \*/", source):
                self.origins[addr] = {"suite": suite, "evidence": evidence, "note": note}

    def add(self, name, *regions):
        self.case = {"id": name, "regions": list(regions), "steps": []}
        self.data["cases"].append(self.case)

    def step(self, addr, label, args=(), result=None, *, arrange=(), memory=(), repeat=1):
        symbol = f"func_{addr}"
        functions = self.data["functions"]
        if symbol not in functions:
            path, _, _, _, arg_types, return_type = signature(symbol)
            functions[symbol] = {"source": path.relative_to(ROOT).as_posix(), "sha256": digest(path),
                                 "args": arg_types, "return": return_type, "origin": self.origins[addr]}
        function = functions[symbol]
        self.case["steps"].append({
            "label": label, "function": symbol,
            "args": [typed(kind, value) for kind, value in zip(function["args"], args, strict=True)],
            "repeat": repeat, "arrange": list(arrange), "return": typed(function["return"], result),
            "memory": list(memory),
        })

    def pure(self, name, addr, rows):
        self.add(name)
        for label, args, result in rows:
            self.step(addr, label, args, result)

    def write(self):
        validate(self.data, check_sources=True)
        # Keep each step on a single line so a fixture change stays local in diffs.
        compact = lambda value: json.dumps(value, ensure_ascii=False, separators=(", ", ": "))
        lines = ['{', f'  "schema": "{SCHEMA}",', '  "version": 1,', '  "byte_order": "little",',
                 '  "functions": {']
        lines.append(",\n".join(f"    {compact(name)}: {compact(info)}"
                                 for name, info in self.data["functions"].items()))
        lines.extend(['  },', '  "cases": ['])
        cases = []
        for case in self.data["cases"]:
            steps = ",\n".join(f"        {compact(step)}" for step in case["steps"])
            cases.append(f'    {{\n      "id": {compact(case["id"])},\n'
                         f'      "regions": {compact(case["regions"])},\n      "steps": [\n{steps}\n      ]\n    }}')
        lines.extend([",\n".join(cases), '  ]', '}', ''])
        OUT.write_text("\n".join(lines), encoding="utf-8")


def a1(f):
    f.pure("signed-min", "800886C8", [
        ("1<2", (1, 2), 1), ("2>1", (2, 1), 1), ("signed", (-5, 3), -5),
        ("equal", (7, 7), 7), ("INT_MIN", (-2**31, 2**31-1), -2**31),
        ("INT_MAX vs -1", (2**31-1, -1), -1),
    ])
    f.pure("sqrt-q8", "800187C8", [(f"sqrt {v}", (v,), vectors.isqrt_q8(v)) for v in vectors.SQRT_INPUTS])
    f.pure("quarter-boundaries", "8003B65C", [(str(v), (v, 100), r) for v, r in (
        (0, 0), (25, 0), (26, 1), (49, 1), (50, 2), (74, 2), (75, 3), (1000, 3), (-5, 0))]
        + [("tiny range", (1, 2), 3)])
    f.pure("rgb-pack", "8001BC44", [
        ("pack", (0x12, 0x34, 0x56), 0x123456), ("black", (0, 0, 0), 0),
        ("white", (255, 255, 255), 0xffffff), ("masked", (0x1ff, 0x100, 0xabc), 0xff00bc),
    ])

    p, s, add = f.pure, f.step, f.add
    a, b, c = packed(0, 0), packed(10, 0), packed(0, 10)
    p("triangle-facing", "80049EF8", [
        ("ccw", (a,b,c), 1), ("cw", (a,c,b), 0),
        ("collinear", (a,packed(5,5),packed(10,10)), 1),
        ("negative ccw", (packed(-5,-5),packed(5,-5),packed(-5,5)), 1),
        ("negative cw", (packed(-5,-5),packed(-5,5),packed(5,-5)), 0),
        ("tiny cw", (a,packed(1,0),packed(0,-1)), 0),
    ])
    add("triangle-output", region("out", 16, 0x5a))
    s("80015AF4", "cw culled", (a,c,b,"out"), 1,
      memory=words("out", [0x5a5a5a5a]*4))
    s("80015AF4", "ccw written", (a,b,c,"out"), 0,
      memory=words("out", [a,b,c,0x5a5a5a5a]))
    s("80015AF4", "collinear kept", (a,packed(5,5),packed(10,10),"out"), 0,
      memory=words("out", [packed(10,10)], 8))
    for label, points, ret in (
        ("negative ccw", ((-5,-5),(5,-5),(-5,5)), 0),
        ("negative cw", ((-5,-5),(-5,5),(5,-5)), 1),
        ("wide ccw", ((-1000,-1000),(1000,-1000),(-1000,1000)), 0),
    ):
        s("80015AF4", label, (*[packed(*point) for point in points], "out"), ret)
    add("distance-xz", region("a", 12), region("b", 12))
    for x, z, ret in ((3,4,6),(10,0,10),(0,-10,10),(-10,10,15),(1,1,2)):
        s("8001A750", f"x={x}, z={z}", ("a","b"), ret, arrange=words("b", [x,1000,z]))
    s("8001A750", "offset", ("a","b"), 110,
      arrange=words("a", [100,0,-50])+words("b", [40,1000,30]))
    s("8001A750", "symmetric", ("b","a"), 110)
    s("8001A750", "same pointer", ("a","a"), 0)
    add("ease-vector", region("src", 12), region("dst", 12))
    for start, end, result in (((400,0,0),(0,100,-100),(100,75,-75)),
                               ((0,0,7),(-1,1,7),(-1,0,7))):
        s("8001A6F8", f"ease {end}", ("src","dst"), arrange=words("src", start)+words("dst", end),
          memory=words("dst", result)+words("src", start))

    add("rgb-scale", region("out", 12))
    for rgb, scale, result in ((0x102030,3,(48,96,144)),(0x102030,-2,(-32,-64,-96)),
                               (0xff000001,5,(0,0,5))):
        s("8001BC64", f"scale {rgb:#x} by {scale}", (rgb,scale,"out"), memory=words("out",result))
    s("8001BCD0", "accumulate", (0x010203,10,"out"),
      arrange=words("out",[1,2,3]), memory=words("out",[11,22,33]))
    s("8001BCD0", "undo", (0x010203,-10,"out"), memory=words("out",[1,2,3]))
    s("8001BCA4", "fixed-point", (0xaabbcc,"out"), memory=words("out",[0xaa0000,0xbb0000,0xcc0000],kind="u32"))
    s("8001BCA4", "ignore top byte", (0xff123456,"out"), memory=words("out",[0x120000,0x340000,0x560000],kind="u32"))
    s("8001BD2C", "wrap", (0xaabb02,"out"), arrange=words("out",[1,2,0xffff0000],kind="u32"),
      memory=words("out",[0xaa0001,0xbb0002,0x10000],kind="u32"))

    add("word-memory", region("w",32,0xee), region("src",12))
    s("8001C27C", "copy word", ("w",("w",4)), arrange=words("w",[0x11111111,0x12345678,0x33333333]),
      memory=words("w",[0x12345678,0x12345678,0x33333333]))
    s("8001C470", "clear middle word", (("w",4),), arrange=words("w",[-1]*3), memory=words("w",[-1,0,-1]))
    s("80018460", "header", (("w",4),), 0x1234, arrange=words("w",[0x1234,77,-8]))
    s("80018460", "signed header", (("w",12),), -8)
    s("8001E304", "null", (None,), -1)
    s("8001E304", "header", (("w",4),), 4096, arrange=words("w",[4096]))
    s("8001E304", "zero header", (("w",4),), 0, arrange=words("w",[0]))
    for addr, offset, count in (("80091B1C",4,3),("80091C90",8,2)):
        before = [0xee]*24
        after = before.copy()
        after[offset:offset+4*count] = [0]*(4*count)
        s(addr, "clear words and keep guards", (("w",offset),count),
          arrange=[cell("w",0,"bytes",before)], memory=[cell("w",0,"bytes",after)])
        zero_offset = 20 if addr == "80091B1C" else 0
        s(addr, "zero count", (("w",zero_offset),0),
          memory=[cell("w",zero_offset,"u32",0xeeeeeeee)])
    s("80028BA0", "copy 10 bytes as 2 words", ("w","src",10),
      arrange=words("src",[0x01020304,0x05060708,0x090a0b0c])+words("w",[0xeeeeeeee],8,"u32"),
      memory=words("w",[0x01020304,0x05060708,0xeeeeeeee],kind="u32"))
    s("80028BA0", "drop 3 byte remainder", (("w",16),"src",3), memory=words("w",[0xeeeeeeee],16,"u32"))
    for off, value in ((0,0xbeef),(2,0xffff)):
        s("80095FE8", "read unsigned halfword", (("w",off),), value,
          arrange=[cell("w",off,"u16",value)])

    add("byte-memory", region("buf",32,0xee))
    s("8001E4C0", "fill and return destination", (("buf",1),0x1a5,5), ("buf",1),
      memory=[cell("buf",0,"bytes",[0xee]+[0xa5]*5+[0xee])])
    s("8001E4C0", "zero count", (("buf",10),0,0), ("buf",10), memory=[cell("buf",10,"u8",0xee)])
    s("80095830", "clear 7 bytes", (("buf",3),0,7),
      arrange=[cell("buf",0,"bytes",[0xee]*16)], memory=[cell("buf",2,"bytes",[0xee]+[0]*7+[0xee])])
    s("80095830", "zero count", ("buf",0x7f,0), memory=[cell("buf",0,"u8",0xee)])
    s("80095830", "truncate to low byte", ("buf",-1,1), memory=[cell("buf",0,"u8",255)])
    sequence = list(range(16))
    reset = [cell("buf",0,"bytes",sequence)]
    for label, dest, src, count, result, head, arrange in (
        ("backward",2,0,6,2,[0,1,0,1,2,3,4,5,8],reset),
        ("forward",0,3,5,5,[3,4,5,6,7,5],reset),
        ("zero count",0,3,0,0,[3],()),
        ("equal pointers",4,4,3,4,[0,1,2,3,4],reset),
        ("exact copy",6,2,3,6,[0,1,2,3,4,5,2,3,4,9],()),
    ):
        s("8008D5D0", label, (("buf",dest),("buf",src),count), ("buf",result),
          arrange=arrange, memory=[cell("buf",0,"bytes",head)])

    add("fixed-pool", region("st",20), region("a",4,0x77), region("b",4,0x77))
    s("800195A4", "empty", ("st",), None, memory=words("st",[0],12))
    for node, old, count in (("a",None,1),("b","a",2)):
        s("800195D0", f"push {node}", ("st",node),
          memory=[cell(node,0,"ptr",old),cell("st",16,"ptr",node),cell("st",12,"s32",count)])
    for node, head, count in (("b","a",1),("a",None,0),(None,None,0)):
        s("800195A4", f"pop {node}", ("st",), node,
          memory=[cell("st",16,"ptr",head),cell("st",12,"s32",count)])
    add("tail-pop", region("node",12), region("t0",12), region("t1",12), region("old",12))
    s("8003BEF0", "no tail", ("node",), None, arrange=[cell("node",4,"ptr","old")],
      memory=[cell("node",4,"ptr","old")])
    s("8003BEF0", "detach and insert", ("node",), "t1",
      arrange=[cell("node",0,"u32",0x100),cell("node",8,"ptr","t1"),cell("t1",0,"ptr","t0")],
      memory=[cell("node",0,"u32",0x100),cell("node",4,"ptr","t1"),cell("node",8,"ptr","t0"),cell("t1",0,"ptr","old")])
    add("bank-offset-table", region("bank",68))
    for i, address in enumerate((("bank",64),None,("bank",8))):
        s("80021800", f"entry {i}", ("bank",i), address,
          arrange=words("bank",[64,0,8],16,"u32") if i == 0 else ())
    add("countdown-mirror", region("record",384))
    for old, new in ((5,9),(7,7),(0,-1)):
        s("8002B1AC", f"source {new}", ("record",),
          arrange=words("record",[old,1],0x10c,"s16")+words("record",[new],0x158,"s16"),
          memory=words("record",[new,new],0x10c,"s16")+words("record",[new],0x158,"s16"))
    add("font-glyph-offset", region("state",64), region("glyphs",2816))
    for i, offset in ((0,0),(1,33),(3,2805)):
        s("800489C8", f"index {i}", ("state",i), ("glyphs",offset),
          arrange=[cell("state",0x10,"ptr","glyphs"),cell("state",0x1c,"bytes",[0,3,0,255])] if i == 0 else ())
    add("first-list-entry", region("list",12), region("entries",24))
    start = [cell("list",0,"ptr","entries")]+[cell("entries",i*6,"u8",v) for i,v in enumerate([10,20,30,20])]
    for i, (count, key, result) in enumerate(((4,10,0),(4,20,1),(4,30,2),(4,99,0),(2,30,0),(0,10,0))):
        s("8007D39C", f"count {count}, key {key}", ("list",key), result,
          arrange=([cell("list",8,"u8",count)]+(start if i == 0 else [])))
    add("pad-ready", region("state",256))
    for ready, device, result in ((0,0xff,1),(1,0xff,0),(1,0x41,1)):
        s("8008C858", f"ready {ready}, device {device}", ("state",), result,
          arrange=[cell("state",0xe6,"u16",ready),cell("state",0x46,"u8",device)])

    add("glyph-name", region("src",8), region("dst",8,0xee), region("table",256))
    table = list(range(256))
    table[1] = ord("X")
    for i, (source, count, expected) in enumerate((
        (b"AB  ",4,[65,66,0,0,0,0xee]),(b"A \x01 ",4,[65,32,88,0,0,0xee]),
        (b"   ",3,[32,0,0,0,0xee]),(b"\0",0,[32,0xee]),
    )):
        s("8007AA9C", f"glyphs {i}", ("dst","src",count,"table"),
          arrange=[cell("src",0,"bytes",source),cell("dst",0,"bytes",[0xee]*8)]+
                  ([cell("table",0,"bytes",table)] if i == 0 else []),
          memory=[cell("dst",0,"bytes",expected)])
    add("dual-checksum", region("data",4), region("out",8))
    for data, n, pair in (([1,2,3],3,[9,6]),([1],0,[0,0]),([255],1,[255,510])):
        s("8007F688", f"checksum {n}", ("data",n,"out",("out",4)),
          arrange=[cell("data",0,"bytes",data)]+words("out",[0xdead,0xbeef]),
          memory=words("out",pair,kind="u32"))

    add("cd-time", region("a",4), region("b",4))
    for m, second, frame in vectors.CD_INPUTS:
        s("8008E720", f"{m}:{second}:{frame}", ("a",), vectors.cd_pos_to_int(m,second,frame),
          arrange=[cell("a",0,"bytes",[vectors.bcd(x) for x in (m,second,frame)]+[0x99])])
    for other, result in (([1,0x20,0x30],0),([1,0x20,0x31],1),([1,0x20,0x29],-1),
                          ([1,0x21,0x29],1),([1,0x19,0x74],-1),([2,0,0x74],1),([0,0x59,0x74],-1)):
        s("8001C3F8", f"MSF {other}", ("a","b"), result,
          arrange=[cell("a",0,"bytes",[1,0x20,0x30,0]),cell("b",0,"bytes",other+[0x55])])
    s("8001C3F8", "unsigned bytes", ("a","b"), -1,
      arrange=[cell("a",0,"bytes",[0,255]),cell("b",0,"bytes",[0,0])])

    add("pad-command-state", region("state",64,0xee), region("parameters",8))
    def state(off, t, v):
        return cell("state",off,t,v)
    s("8008AE70","bind actuator buffer",("state",0x12345678,0x9a),
      memory=[state(0x28,"u32",0x12345678),state(0x34,"u8",0x9a),state(0x24,"u32",0xeeeeeeee),state(0x35,"u8",0xee)])
    s("8008AE7C","set command and active bytes",("state",0x142,signed32(0xcafebabe),0x103),
      memory=[state(0x36,"u8",0x42),state(0x2c,"u32",0xcafebabe),state(0x35,"u8",3),state(0x37,"u8",0xee)])
    s("8008B4EC","rumble mapping",("state",),arrange=[state(0x20,"ptr","parameters")],
      memory=[state(0x36,"u8",0x4d),state(0x35,"u8",6),state(0x2c,"ptr","parameters")])
    for addr, cmd, parameter in (
        ("8008B71C",0x43,1),("8008B73C",0x45,None),("8008B750",0x4c,0xff),
        ("8008B770",0x46,2),("8008B790",0x47,0),("8008B7B0",0x4b,None),
    ):
        checks = [state(0x36,"u8",cmd),state(0x35,"u8",int(parameter is not None)),
                  state(0x2c,"ptr",("state",0x24) if parameter is not None else None)]
        if parameter is not None:
            checks += [state(0x24,"u8",parameter),state(0x25,"u8",0xee)]
        s(addr, f"command {cmd:#x}", ("state",) if parameter is None else ("state",parameter), memory=checks)
    for i, current in enumerate((0x45,0,0xf0)):
        s("8008C06C",f"consume {current:#x}",("state",),current,
          arrange=[state(0x36,"u8",current)] if i != 1 else (),
          memory=[state(0x36,"u8",0),state(0x37,"u8",current)])
    p("queued-command-kinds","8004DA58",[(hex(v),(v,),int(v in (0x3c,0x45,0x64,0x65)))
      for v in (0x3b,0x3c,0x45,0x46,0x63,0x64,0x65,0x66,0,0xffffffff)])

    # The packet table and guard expectations are from t_prims/check_prim.
    for addr, length, code, term in (
        ("80092488",6,0x30,None),("8009249C",9,0x34,None),("800924B0",5,0x28,None),
        ("800924C4",9,0x2c,None),("800924D8",8,0x38,None),("800924EC",12,0x3c,None),
        ("80092514",3,0x7c,None),("80092528",4,0x64,None),("8009253C",2,0x68,None),
        ("80092550",2,0x70,None),("80092564",2,0x78,None),("80092578",3,0x60,None),
        ("8009258C",3,0x40,None),("800925A0",4,0x50,None),("800925D4",7,0x58,28),("800925F4",6,0x4c,24),
    ):
        add(f"packet-{addr}", region("packet",40,0xa5))
        packet = bytearray([0xa5]*40)
        packet[3], packet[7] = length, code
        if term is not None:
            packet[term:term+4] = b"UUUU"
        s(addr,"packet header and guards",("packet",),memory=[cell("packet",0,"bytes",packet)])
    add("primitive-flags",region("p",8,0xa5))
    for addr, mask in (("80092410",2),("80092438",1)):
        for i, (enabled, code) in enumerate(((1,0x2c|mask),(-7,0x2c|mask),(0,0x2c),(0,255^mask))):
            s(addr,f"flag {enabled} iteration {i}",("p",enabled),
              arrange=[cell("p",7,"u8",0x2c if i == 0 else 255)] if i in (0,3) else (),
              memory=[cell("p",0,"bytes",[0xa5]*7+[code])])
    add("primitive-links",region("t",12))
    for word, result in ((0x04ffffff,1),(0x00fffffe,0),(0xffffffff,1),(0xff000000,0)):
        s("80092340",hex(word),("t",),result,arrange=words("t",[word],kind="u32"))
    for word, link, result in ((0x07abcdef,0x80123456,0x07123456),(0,0xffffffff,0x00ffffff)):
        s("800923D4","set link",("t",link),arrange=words("t",[word],kind="u32"),memory=words("t",[result],kind="u32"))
    for word, result in ((0x05000000,0x05ffffff),(0x09123456,0x09ffffff)):
        s("800923F8","terminate",("t",),arrange=words("t",[word],kind="u32"),memory=words("t",[result],kind="u32"))
    s("80092398","chain two primitives",("t",0x80abcdef,("t",4)),
      arrange=words("t",[0xaa123456,0x09ffffff],kind="u32"),memory=words("t",[0xaaabcdef,0x09123456],kind="u32"))
    p("texture-page","80092230",[(str(v),v,vectors.get_tpage(*v)) for v in vectors.TPAGE_INPUTS])
    p("clut-index","8009226C",[(str(v),v,vectors.get_clut(*v)) for v in vectors.CLUT_INPUTS])
    p("draw-mode","80093F58",[(str(v),v,r) for v,r in (
      ((0,0,0),0xe1000000),((1,0,0),0xe1000400),((0,1,0),0xe1000200),((2,-3,0x1ff),0xe10007ff),((0,0,0xffff),0xe10009ff))])
    p("draw-offset","800940A8",[(str(v),v,r) for v,r in (
      ((0,0),0xe5000000),((160,120),0xe503c0a0),((0xffffffff,0),0xe50007ff),((0x800,0x800),0xe5000000),((0,0x7ff),0xe53ff800))])
    add("texture-window",region("rect",8))
    s("800940C4","NULL",(None,),0)
    for values in vectors.TW_INPUTS:
        s("800940C4",str(values),("rect",),signed32(vectors.get_tw(*values)),arrange=words("rect",values,kind="s16"))
    add("gpu-draw-mask",region("packet",16,0xa5))
    for addr, flags, bits in (
      ("80093A4C",(0,0),0),("80093A4C",(1,0),2),("80093A4C",(0,1),1),("80093A4C",(5,-3),3),
      ("80093A74",(1,),1),("80093A74",(0,),0),("80093A74",(0x100,),1),
    ):
        s(addr,str(flags),("packet",*flags),memory=[cell("packet",0,"bytes",[0xa5]*3+[2])]+
          words("packet",[0xe6000000|bits,0],4,"u32"))
    add("scale-definition",region("record",16,0xa5))
    for values, result in (((600,300,218),(2,256,4096)),((-599,0,-1),(-1,0,-18)),((299,-1,1),(0,0,18))):
        s("800607B8",str(values),("record",),
          arrange=words("record",values[:2],6,"s16")+words("record",values[2:],12,"s16"),
          memory=words("record",result[:2],6,"s16")+words("record",result[2:],12,"s16")+
          [cell("record",0,"bytes",[0xa5]*6),cell("record",10,"bytes",[0xa5]*2)])


def main():
    fixtures = Fixtures()
    a1(fixtures)
    fixtures.write()


if __name__ == "__main__":
    main()
