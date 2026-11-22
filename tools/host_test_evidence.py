"""Conservative helpers for interpreting host-test dependency evidence.

A candidate has no known hardware path within the supplied graph. It is not
proof that the graph, the disassembly signatures, or the runtime inputs are
complete. Missing evidence and recursive cycles are reported as unknown.
"""


def classify_graph(nodes, edges, hardware, incomplete):
    """Classify nodes, giving known hardware paths precedence over unknowns."""
    nodes = set(nodes)
    hardware = set(hardware) & nodes
    unknown = (set(incomplete) & nodes) | {
        n for n in nodes if set(edges.get(n, ())) - nodes
    }
    changed = True
    while changed:
        changed = False
        for n in nodes - hardware:
            if set(edges.get(n, ())) & hardware:
                hardware.add(n)
                changed = True
    resolved = set(hardware)
    state = {n: 'hw' for n in hardware}
    while True:
        progress = False
        for n in nodes - resolved:
            children = set(edges.get(n, ()))
            if n in unknown:
                state[n] = 'unknown'
            elif children <= resolved:
                state[n] = ('unknown' if any(state[c] == 'unknown' for c in children)
                            else 'candidate')
            else:
                continue
            resolved.add(n)
            progress = True
        if not progress:
            break
    for n in nodes - resolved:
        state[n] = 'unknown'
    return state


def read_image_range(image, address, size):
    """Return loaded PS-X EXE bytes, or None if the extent is not present."""
    import struct
    if len(image) < 0x800 or image[:8] != b'PS-X EXE' or size <= 0:
        return None
    base, loaded_size = struct.unpack_from('<II', image, 0x18)
    offset = address - base
    if offset < 0 or offset + size > loaded_size or 0x800 + offset + size > len(image):
        return None
    return bytes(image[0x800 + offset:0x800 + offset + size])


def classify_address(address, size, image, base):
    """Describe bytes present in an image without assuming runtime BSS state."""
    offset = address - base
    if size <= 0 or offset < 0 or offset + size > len(image):
        return 'unknown'
    return 'image-nonzero' if any(image[offset:offset + size]) else 'image-zero'
