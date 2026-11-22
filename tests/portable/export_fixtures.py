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


def a2(f):
    """Transcription of tests/host/leaf_a2_harness.c.

    Each D_ global is an explicit region sized like its DGLOBAL storage; every
    case supplies exactly the globals the called objects reference, and every
    state read by a unit is either set by `arrange` or by an explicit region fill
    (the harness' lk_fill / explicit zeroing). Steps inside a case keep the
    harness order where later assertions depend on earlier calls.
    """
    p, s, add = f.pure, f.step, f.add

    def g(label, words, fill=0):
        return region(label, 4 * words, fill)

    def u32(label, value, offset=0):
        return cell(label, offset, "u32", value)

    def ptr(label, target, offset=0):
        return cell(label, offset, "ptr", target)

    # 80019720: seed vectors, the explicit zero state, and the published check.
    add("a2-park-miller", g("D_8009F214", 1))
    for seed in vectors.PM_SEEDS:
        z = vectors.park_miller(seed)
        s("80019720", f"seed {seed}", (), z, arrange=[u32("D_8009F214", seed)],
          memory=[cell("D_8009F214", 0, "s32", z)])
    s("80019720", "zero state remains zero", (), 0, arrange=[u32("D_8009F214", 0)],
      memory=[cell("D_8009F214", 0, "s32", 0)])
    z = 1
    for _ in range(10000):
        z = vectors.park_miller(z)
    assert z == 1043618065, z
    s("80019720", "published check value z10001 from z1", (), z,
      arrange=[u32("D_8009F214", 1)], repeat=10000)

    # 80018534: synthetic quarter table t[i] = i (no retail sine data).
    table = b"".join(i.to_bytes(2, "little") for i in range(0x401))
    add("a2-sine-lookup", g("D_8009A234", 0x201))
    for i, (angle, result, label) in enumerate((
        (0, 0, "0"), (0x100, 0x200, "first quadrant direct"), (0x3FF, 0x7FE, "end of first quadrant"),
        (0x400, 0x800, "quarter"), (0x500, 0x600, "second quadrant mirrored"), (0x800, 0, "half"),
        (0x900, -0x200, "third quadrant negated"), (0xC00, -0x800, "three quarters"),
        (0xF00, -0x200, "fourth quadrant"), (0x1100, 0x200, "wraps at 4096"),
        (0xFFFFFF00, -0x200, "negative angle wraps"),
    )):
        s("80018534", label, (angle,), result,
          arrange=[cell("D_8009A234", 0, "bytes", table)] if i == 0 else ())

    # 8001814C: sorted boundary table through a pointer global.
    add("a2-heap-boundary", g("D_8009F590", 1), g("D_8009F594", 1), region("t", 20))
    boundaries = words("t", [0, 10, 20, 30, 40])
    for i, (value, result, label, arrange) in enumerate((
        (25, 2, "inside", [ptr("D_8009F590", "t"), u32("D_8009F594", 5)] + boundaries),
        (20, 2, "exact first midpoint", ()), (30, 3, "exact upper midpoint", ()),
        (10, 2, "equal midpoint with duplicate boundaries", words("t", [10], 8)),
        (10, 1, "on boundary", words("t", [20], 8)),
        (9, 0, "just below", ()), (40, 4, "last boundary", ()), (1000, 4, "above -> last", ()),
        (-5, 0, "below -> 0", ()), (39, 3, "below last", ()),
        (15, 1, "count 2", [u32("D_8009F594", 2)]), (5, 0, "count 2 low", ()),
    )):
        s("8001814C", label, (value,), result, arrange=arrange)

    add("a2-heap-threshold", g("D_8009F594", 1), g("D_8009F5A0", 1))
    for count, threshold, result, label in ((3, 3, 1, "equal"), (2, None, 0, "below"),
                                            (4, None, 1, "above"), (-1, 0, 0, "signed compare")):
        arrange = [cell("D_8009F594", 0, "s32", count)]
        if threshold is not None:
            arrange.append(cell("D_8009F5A0", 0, "s32", threshold))
        s("80017B88", label, (), result, arrange=arrange)

    # 80017B28: id @0x88, next @0x8C.
    add("a2-instance-list", g("D_8009F588", 1), region("a", 0x90), region("b", 0x90), region("c", 0x90))
    chain = [u32("a", 7, 0x88), ptr("a", "b", 0x8C), u32("b", 9, 0x88), ptr("b", "c", 0x8C),
             u32("c", 9, 0x88), ptr("c", None, 0x8C), ptr("D_8009F588", "a")]
    s("80017B28", "head", (7,), "a", arrange=chain)
    s("80017B28", "first of duplicates", (9,), "b")
    s("80017B28", "missing", (5,), None)
    s("80017B28", "empty list", (7,), None, arrange=[ptr("D_8009F588", None)])

    # 80019BF0 / 80019AA8: group @0x18, name @0x1C, next @0x0C.
    add("a2-resource-registry", g("D_8009F5B8", 1), *(region(n, 0x20) for n in ("a", "b", "c", "probe")))
    registry = [ptr("a", "b", 0x0C), u32("a", 1, 0x18), u32("a", 100, 0x1C),
                ptr("b", "c", 0x0C), u32("b", 2, 0x18), u32("b", 100, 0x1C),
                ptr("c", None, 0x0C), u32("c", 2, 0x18), u32("c", 200, 0x1C), ptr("D_8009F5B8", "a")]
    for i, (group, name, result, label) in enumerate(((1, 100, "a", "a"), (2, 100, "b", "b"),
                                                      (2, 200, "c", "c (tail)"),
                                                      (1, 200, None, "group and name must both match"))):
        s("80019BF0", label, (group, name), result, arrange=registry if i == 0 else ())
    s("80019AA8", "registered", ("probe",), 1, arrange=[u32("probe", 2, 0x18), u32("probe", 200, 0x1C)])
    s("80019AA8", "not registered", ("probe",), 0, arrange=[u32("probe", 300, 0x1C)])
    s("80019AA8", "empty registry", ("probe",), 0, arrange=[ptr("D_8009F5B8", None)])

    # 8004D3D4 append, then 8004D69C tag search on the resulting list.
    add("a2-entity-list", g("D_8009FA40", 1), g("D_8009FA88", 1), region("a", 0x18), region("b", 0x18))
    s("8004D3D4", "first becomes head and tail", ("a",),
      arrange=[ptr("D_8009FA40", None), ptr("D_8009FA88", None), u32("a", 0xDEAD), u32("b", 0xDEAD)],
      memory=[ptr("D_8009FA40", "a"), ptr("D_8009FA88", "a"), u32("a", 0)])
    s("8004D3D4", "append b", ("b",),
      memory=[ptr("D_8009FA40", "a"), ptr("a", "b"), ptr("D_8009FA88", "b"), u32("b", 0)])
    tags = [cell("a", 0x14, "u16", 0x1234), cell("b", 0x14, "u16", 0xFFFF)]
    for i, (tag, result, label) in enumerate(((0x1234, "a", "a"), (0xFFFF, "b", "u16 tag 0xFFFF"),
                                              (-1, None, "-1 never equals a zero-extended tag"),
                                              (5, None, "missing"))):
        s("8004D69C", label, (tag,), result, arrange=tags if i == 0 else ())

    # 8004665C: counted free list, next @0x1C.
    add("a2-free-voice-pop", g("D_8009F73C", 1), g("D_8009F740", 1), region("a", 0x20), region("b", 0x20))
    s("8004665C", "pop a", (), "a",
      arrange=[ptr("a", "b", 0x1C), ptr("b", None, 0x1C), ptr("D_8009F73C", "a"), u32("D_8009F740", 2)],
      memory=[u32("D_8009F740", 1), ptr("D_8009F73C", "b")])
    s("8004665C", "pop b", (), "b", memory=[u32("D_8009F740", 0)])
    s("8004665C", "count 0 -> NULL even with a head", (), None, arrange=[ptr("D_8009F73C", "a")],
      memory=[ptr("D_8009F73C", "a")])

    # 8004AA90: next @0x48; the last node is kept.
    add("a2-fragment-pop", g("D_8009F798", 1), region("a", 0x4C), region("b", 0x4C))
    s("8004AA90", "pop a", (), "a",
      arrange=[ptr("a", "b", 0x48), ptr("b", None, 0x48), ptr("D_8009F798", "a")],
      memory=[ptr("D_8009F798", "b")])
    s("8004AA90", "last node is kept", (), None, memory=[ptr("D_8009F798", "b")])

    # Volume clamps.
    for case, addr, label, rows in (
        ("a2-sfx-volume", "800451F8", "D_8009F330",
         ((64, 64), (127, 127), (128, 127), (-1, 0), (0, 0), (0x7FFFFFFF, 127))),
        ("a2-voice-volume", "80045294", "D_8009F338",
         ((1, 1), (200, 127), (-2**31, 0), (127, 127))),
    ):
        add(case, g(label, 1, 0xEE))
        for volume, stored in rows:
            s(addr, f"volume {volume}", (volume,), memory=[cell(label, 0, "s32", stored)])

    # 8001F0FC / 8001F12C share the hook slot.
    add("a2-screen-mode-hook", g("D_8009FA20", 1), g("D_8009F99C", 1))
    s("8001F0FC", "disabled", (0x1234,), 3, arrange=[u32("D_8009FA20", 0), u32("D_8009F99C", 0)],
      memory=[u32("D_8009F99C", 0)])
    s("8001F0FC", "installed", (0x1234,), 0, arrange=[u32("D_8009FA20", 1)], memory=[u32("D_8009F99C", 0x1234)])
    s("8001F0FC", "occupied", (0x5678,), 3, memory=[u32("D_8009F99C", 0x1234)])
    s("8001F12C", "different callback", (0x5678,), 3, memory=[u32("D_8009F99C", 0x1234)])
    s("8001F12C", "removed", (0x1234,), 0, memory=[u32("D_8009F99C", 0)])
    s("8001F12C", "none installed", (0,), 3)

    add("a2-cd-callbacks", g("D_8009C8A8", 1), g("D_8009C8AC", 1))
    s("8008E194", "previous", (0x222,), 0x111, arrange=[u32("D_8009C8A8", 0x111)],
      memory=[u32("D_8009C8A8", 0x222)])
    s("8008E194", "previous again", (0,), 0x222)
    s("8008E1A8", "previous", (0x444,), 0x333, arrange=[u32("D_8009C8AC", 0x333)],
      memory=[u32("D_8009C8AC", 0x444), u32("D_8009C8A8", 0)])

    add("a2-draw-offset", g("D_8009DD90", 1))
    s("80091F60", "previous", (-7,), 5, arrange=[u32("D_8009DD90", 5)],
      memory=[cell("D_8009DD90", 0, "s32", -7)])

    add("a2-irq-mask", g("D_8009DD18", 1), region("mask", 2))
    s("800915FC", "old mask", (0x0401,), 0x00FF,
      arrange=[ptr("D_8009DD18", "mask"), cell("mask", 0, "u16", 0x00FF)],
      memory=[cell("mask", 0, "u16", 0x0401)])
    s("800915FC", "old again", (0xFFFF,), 0x0401)
    s("800915E4", "u16 read", (), 0xFFFF)
    s("800915E4", "reflects memory", (), 0x8001, arrange=[cell("mask", 0, "u16", 0x8001)])

    add("a2-draw-area", g("D_8009DDF4", 1), g("D_8009DDF6", 1))
    limits = lambda w, h: [cell("D_8009DDF4", 0, "u16", w), cell("D_8009DDF6", 0, "u16", h)]
    for i, (x, y, result, label) in enumerate((
        (100, 200, 0xE3000000 | (200 << 10) | 100, "inside"), (-5, -5, 0xE3000000, "negative clamps to 0"),
        (2000, 600, 0xE307FFFF, "clamp to 1023,511"), (1023, 511, 0xE307FFFF, "max inside"),
        (1024, 0, 0xE30003FF, "x = max clamps"),
    )):
        s("80093F78", label, (x, y), signed32(result), arrange=limits(1024, 512) if i == 0 else ())
    s("80093F78", "320x240 limits", (400, 300), signed32(0xE3000000 | (239 << 10) | 319),
      arrange=limits(320, 240))

    add("a2-spu-transfer-mode", g("D_8009EA34", 1, 0xEE), g("D_8009EEC0", 1, 0xEE))
    s("80098F48", "IO -> flag 1", (1,), memory=[u32("D_8009EEC0", 1), u32("D_8009EA34", 1)])
    s("80098F48", "DMA -> flag 0", (0,), memory=[u32("D_8009EEC0", 0)])
    s("80098F48", "unknown -> flag 0", (7,), memory=[u32("D_8009EEC0", 0), u32("D_8009EA34", 7)])
    s("80098F78", "after unknown mode -> 0", (), 0, memory=[u32("D_8009EA34", 0)])
    # The harness has no check after this call; its earlier assertion for the
    # same input is the observation v1 requires for a void step.
    s("80098F48", "IO again", (1,), memory=[u32("D_8009EEC0", 1)])
    s("80098F78", "round trip IO", (), 1)
    s("80098F78", "corrupt flag -> 0", (), 0, arrange=[u32("D_8009EEC0", 5)])

    add("a2-vsync-callback-slot", g("D_8009DD30", 8))
    s("80091C64", "slot 3", (3, 0x99), arrange=[u32("D_8009DD30", 1, 12)],
      memory=[u32("D_8009DD30", 0x99, 12), u32("D_8009DD30", 0, 8)])
    s("80091C64", "same value", (3, 0x99), memory=[u32("D_8009DD30", 0x99, 12)])

    add("a2-sound-voice-masks", g("D_800A07A8", 8))
    s("80021A70", "or", (0x5,), arrange=[u32("D_800A07A8", 0x10, 0x14), u32("D_800A07A8", 0, 0x18)],
      memory=[u32("D_800A07A8", 0x15, 0x14), u32("D_800A07A8", 0, 0x18)])
    s("80021A8C", "pending -> acknowledged", (0x11,),
      memory=[u32("D_800A07A8", 0x04, 0x14), u32("D_800A07A8", 0x11, 0x18)])
    s("80021AB4", "or high bit", (0x80000000,),
      memory=[u32("D_800A07A8", 0x80000011, 0x18), u32("D_800A07A8", 0x04, 0x14)])

    add("a2-module-read-progress", g("D_800A0B70", 0x11))
    progress = lambda done, total: ([cell("D_800A0B70", 0x3E, "u16", done)] if done is not None else []) + \
        ([cell("D_800A0B70", 0x40, "u16", total)] if total is not None else [])
    for done, total, result, label in ((512, 1024, 512, "half"), (1024, None, 1024, "done"),
                                       (1, 3, 341, "1/3 truncates"), (0xFFFF, 0xFFFF, 1024, "u16 max"),
                                       (None, 0, 0, "no total")):
        s("80028FD4", label, (), result, arrange=progress(done, total))

    add("a2-portrait-voice-flags", g("D_8009F6B8", 1))
    for before, after, label in ((0xFF, 0xEF, "bit5 set"), (0x10, 0x08, "bit5 clear"),
                                 (0x00, 0x08, "zero"), (0x30, 0x20, "bit4+5")):
        s("80029720", label, (), arrange=[u32("D_8009F6B8", before)], memory=[cell("D_8009F6B8", 0, "u8", after)])

    banks = bytearray(0x6E * 0x1C)
    for slot, key in ((0, 5), (1, 9), (3, 7)):
        banks[slot * 0x1C + 0x14], banks[slot * 0x1C + 0x16] = 1, key
    add("a2-sound-bank-search", g("D_800A7938", 770))
    s("800472E8", "first", (5,), 0, arrange=[cell("D_800A7938", 0, "bytes", banks)])
    s("800472E8", "second", (9,), 1)
    s("800472E8", "behind an unused slot", (7,), -1)
    for slot in range(0x6E):
        banks[slot * 0x1C + 0x14], banks[slot * 0x1C + 0x16] = 1, 1
    banks[0x6D * 0x1C + 0x16] = 2
    s("800472E8", "last slot", (2,), 0x6D, arrange=[cell("D_800A7938", 0, "bytes", banks)])
    s("800472E8", "all used, missing", (3,), -1)

    add("a2-sound-entry-reset", g("D_800A7938", 770, 0xAA))
    entries = [0xAA] * (3 * 0x1C)
    entries[0x1C + 0x10:0x1C + 0x1C] = [0, 0, 0, 1, 0, 0, 0xFF, 0xAA, 0, 0, 0, 0]
    s("80045504", "reset entry 1, keep neighbours", (1,), memory=[cell("D_800A7938", 0, "bytes", entries)])

    add("a2-sound-voice-mix", g("D_8009F738", 1), region("v", 8))
    mix = lambda *values: [cell("v", 4 + i, "s8", v) for i, v in enumerate(values)]
    s("800477D8", "volume, base, pan, extra", ("v", 10, 100, 7),
      arrange=[u32("v", 0x12345678), u32("v", 0, 4), u32("D_8009F738", 0)],
      memory=mix(100, 100, 10, 7) + [u32("v", 0x12345678)])
    s("800477D8", "halved volume", ("v", -10, 100, 0), arrange=[u32("D_8009F738", 1)], memory=mix(50, 100, -10))
    s("800477D8", "-1 >> 1 is arithmetic", ("v", 0, -1, 0), memory=mix(-1))

    add("a2-stream-spu-buffers", g("D_800A0968", 0x40, 0xAA))
    s("80022C68", "count 2", (), 0, arrange=[u32("D_800A0968", 2, 0x14)],
      memory=[u32("D_800A0968", 0, 0x44), u32("D_800A0968", 0, 0x68),
              u32("D_800A0968", 0xAAAAAAAA, 0x8C), u32("D_800A0968", 0xAAAAAAAA, 0x48)])
    # The harness ignores this return; v1 needs one. The @leaf note states
    # "return 0" for the unit, which is the only expectation used here.
    s("80022C68", "count 0", (), 0, arrange=[u32("D_800A0968", 0, 0x14), u32("D_800A0968", 5, 0x44)],
      memory=[u32("D_800A0968", 5, 0x44)])

    add("a2-text-palette", g("D_8009AAE0", 11, 0xAA))
    for index, rgb, colour, label in ((3, (31, 0, 0), 0x7C00, "blue"), (0, (0, 31, 0), 0x03E0, "green"),
                                      (0x13, (0, 0, 31), 0x001F, "red, last slot"),
                                      (1, (1, 2, 3), 0x0443, "mixed")):
        s("800493C0", label, (index, *rgb), memory=[cell("D_8009AAE0", 2 * index, "u16", colour)])
    s("800493C0", "index 0x14 rejected", (0x14, 31, 31, 31),
      memory=[cell("D_8009AAE0", 0x28, "u16", 0xAAAA), cell("D_8009AAE0", 4, "u16", 0xAAAA),
              cell("D_8009AAE0", 0x26, "u16", 0x001F)])

    add("a2-depth-cue-range", g("D_8009FA64", 1), region("near", 4), region("far", 4))
    s("800536B4", "near 608", ("near", "far"), arrange=[u32("D_8009FA64", 608), u32("near", 99), u32("far", 99)],
      memory=[cell("near", 0, "s32", 256), cell("far", 0, "s32", 0)])
    s("800536B4", "far", ("near", "far"), arrange=[u32("D_8009FA64", 0x1A000 + 192)],
      memory=[cell("near", 0, "s32", 44921), cell("far", 0, "s32", 256)])
    s("800536B4", "far at boundary", ("near", "far"), arrange=[u32("D_8009FA64", 0x1A000)],
      memory=[cell("far", 0, "s32", 0)])

    add("a2-effect-key-index", g("D_8009F7F8", 1), g("D_800A2300", 0x60))
    keys = [cell("D_800A2300", 0x30 * i, "u8", key) for i, key in enumerate((4, 8, 4, 200))]
    for i, (count, key, result, label) in enumerate(((3, 4, 0, "first"), (None, 8, 1, "second"),
                                                     (None, 200, -1, "beyond count"),
                                                     (None, 0x104, -1, "u8 key vs wider int"),
                                                     (4, 200, 3, "unsigned key 200"), (0, 4, -1, "empty"))):
        arrange = (keys if i == 0 else []) + ([u32("D_8009F7F8", count)] if count is not None else [])
        s("8005FF70", label, (key,), result, arrange=arrange)

    add("a2-radar-cell", g("D_8009FAA4", 1))
    for i, (time, value, result, label) in enumerate((
        (64, 0, 0, "zero"), (None, 5, 5, "(5*64)>>6 % 28"), (None, 30, 2, "30 % 28"), (None, -5, 22, "27 - 5"),
        (1024, 0x21, 0x1C, "odd blink phase"), (2048, 0x21, 0, "even blink phase"),
        (None, 0x20, 16, "0x20 cycle value"),
    )):
        s("80065744", label, (value,), result, arrange=[u32("D_8009FAA4", time)] if time is not None else ())

    add("a2-script-countdowns", g("D_8009F9A4", 1), region("t", 33 * 4))
    counts = [50] * 33
    counts[:5], counts[31] = [10, 5, 0, -3, 8], 100
    s("80069D0C", "step by 3", (3,), arrange=[ptr("D_8009F9A4", "t")] + words("t", counts),
      memory=words("t", [7, 0, 0, -3, 5]) + words("t", [97, 50], 31 * 4))

    add("a2-movie-buffer-size", g("D_8009F980", 1), g("D_8009F984", 1), g("D_8009F988", 1))
    s("800896D4", "size", (4, 100, 3), 100 + 4 * 2048 + 0x57C + 3 * 56,
      memory=[u32("D_8009F980", 4), u32("D_8009F984", 100), u32("D_8009F988", 3)])
    s("800896D4", "header only", (0, 0, 0), 0x57C)

    add("a2-heap-init", g("D_8009C7D0", 1), region("h", 0x80, 0xAA))
    s("8008CA30", "NULL base ignored", (None, 64), arrange=[u32("D_8009C7D0", 0x55)],
      memory=[u32("D_8009C7D0", 0x55)])
    s("8008CA30", "zero size ignored", ("h", 0), memory=[u32("D_8009C7D0", 0x55)])
    s("8008CA30", "size 0x43", ("h", 0x43),
      memory=[ptr("h", ("h", 56)), u32("h", 7, 4), ptr("h", "h", 56), u32("h", 0, 60),
              ptr("D_8009C7D0", "h"), u32("h", 0xAAAAAAAA, 64)])

    add("a2-pad-port-block", g("D_800A3C20", 0x80))
    for offset, result, label in ((0, 0x10, "port 1"), (0xF0, 0x20, "port 2"),
                                  (1, 0xFF, "inside block"), (0x1E0, 0xFF, "third block")):
        s("8008C3B4", label, (("D_800A3C20", offset),), result)
    for flags, offset, label in ((0, 0, "port 1"), (0x10, 0xF0, "port 2"),
                                 (0x0F, 0, "low nibble ignored"), (0xF00, 0, "bits above 0xF0 ignored")):
        s("8008C3EC", label, (flags,), ("D_800A3C20", offset))

    add("a2-pad-rumble", g("D_800A6FF8", 0x10))
    rumble = [cell("D_800A6FF8", off, "u8", v) for off, v in ((0x16, 0x11), (0x17, 0x12), (0x2E, 0x21), (0x2F, 0x22))]
    for i, (addr, port, result, label) in enumerate((
        ("8001EAEC", 0, 0x11, "port 0"), ("8001EAEC", 1, 0x21, "port 1"), ("8001EAEC", 5, 0x21, "clamped high"),
        ("8001EAEC", -1, 0x11, "clamped low"), ("8001EB2C", 0, 0x12, "port 0"), ("8001EB2C", 1, 0x22, "port 1"),
        ("8001EB2C", 2, 0x22, "clamped high"), ("8001EB2C", -9, 0x12, "clamped low"),
    )):
        s(addr, label, (port,), result, arrange=rumble if i == 0 else ())

    add("a2-pad-type", g("D_800A78F0", 0x12))
    pad = lambda off, v: cell("D_800A78F0", off, "u8", v)
    for arrange, port, result, label in (
        ([pad(1, 0x41)], 0, 4, "digital pad 41h"), ([pad(1, 0x73)], 0, 7, "analog pad 73h"),
        ([pad(1, 0x23)], 0, 0, "negcon 23h unsupported"), ([pad(1, 0x53)], 0, 0, "analog stick 53h unsupported"),
        ([pad(0, 0xFF), pad(1, 0x41)], 0, 0, "status FFh: no pad"),
        ([pad(0x24, 0), pad(0x25, 0x73)], 1, 7, "port 1 at +0x24"),
    ):
        s("8001EF34", label, (port,), result, arrange=arrange)

    add("a2-pad-port-state", g("D_8009C774", 1), g("D_8009C75C", 1), region("pp", 512))
    s("80089E40", "disabled", (0,), 0,
      arrange=[cell("pp", 0xE8, "u8", 8), cell("pp", 240 + 0xE8, "u8", 7),
               ptr("D_8009C75C", "pp"), u32("D_8009C774", 0)])
    s("80089E40", "port 0 state 8", (0,), 1, arrange=[u32("D_8009C774", 1)])
    s("80089E40", "low nibble ignored", (0x0F,), 1)
    s("80089E40", "port 1 state 7", (0x10,), 0)

    # The harness stores integers 0x1000+i; v1 can only observe pointers into
    # named regions, so entry i points at byte i of a synthetic "names" region.
    add("a2-cd-int-name", g("D_8009C94C", 7), region("names", 7), g("D_8001233C", 1))
    names = [ptr("D_8009C94C", ("names", i), 4 * i) for i in range(7)]
    for i, (index, result, label) in enumerate(((0, ("names", 0), "0"), (6, ("names", 6), "6"),
                                                (7, "D_8001233C", "7 -> fallback"),
                                                (0x103, ("names", 3), "only low byte used"))):
        s("8008E120", label, (index,), result, arrange=names if i == 0 else ())

    add("a2-ground-keys", g("D_8009F838", 1), g("D_800A31A0", 0x30))
    for i, (key, value) in enumerate(((0x111, 1), (0x222, 2), (0x333, 3))):
        s("8006314C", ["first", "second", "third"][i], (key, value), 1,
          arrange=[u32("D_8009F838", 0)] if i == 0 else (),
          memory=[u32("D_8009F838", 3), u32("D_800A31A0", 0x111), cell("D_800A31A0", 4, "u8", 1),
                  u32("D_800A31A0", 0x333, 16), cell("D_800A31A0", 20, "u8", 3)] if i == 2 else ())
    s("8006314C", "last free slot", (0x444, 4), 1, arrange=[u32("D_8009F838", 0x17)])
    s("8006314C", "full", (0x555, 5), 0, memory=[u32("D_8009F838", 0x18)])
    # Removal works on the three entries registered above, as in the harness.
    s("80063194", "swap-remove slot 0", (0x111,), arrange=[u32("D_8009F838", 3)],
      memory=[u32("D_8009F838", 2), u32("D_800A31A0", 0x333), cell("D_800A31A0", 4, "u8", 3),
              u32("D_800A31A0", 0x222, 8)])
    s("80063194", "missing key: no change", (0x999,), memory=[u32("D_8009F838", 2)])
    s("80063194", "removing the last clears its key", (0x222,),
      memory=[u32("D_8009F838", 1), u32("D_800A31A0", 0, 8), u32("D_800A31A0", 0x333)])
    s("80063194", "empty", (0x333,), memory=[u32("D_8009F838", 0)])


def main():
    fixtures = Fixtures()
    a1(fixtures)
    a2(fixtures)
    fixtures.write()


if __name__ == "__main__":
    main()
