#!/bin/bash
# Shared helper for host test scripts: map an 8-hex unit address to its
# repo-relative path under $DECOMP/src/game/<group>/<name>.c.
#
# Loads the map once when sourced (provided DECOMP is already set) via a single
# python3 invocation so loops over dozens of units inherit the associative array
# without spawning python3 per unit.

declare -A _FC_UNIT_PATHS=()
_FC_UNIT_PATHS_ROOT=""

_fc_load_unit_paths() {
    local root="${DECOMP:?DECOMP must be set before calling unit_src}"
    if [[ "$_FC_UNIT_PATHS_ROOT" == "$root" && "${#_FC_UNIT_PATHS[@]}" -gt 0 ]]; then
        return 0
    fi
    _FC_UNIT_PATHS=()
    local addr rel
    while IFS=$'\t' read -r addr rel; do
        [[ -n "$addr" ]] && _FC_UNIT_PATHS["$addr"]="$rel"
    done < <(python3 "$root/tools/unit_paths.py" --root "$root" --dump-tsv)
    _FC_UNIT_PATHS_ROOT="$root"
}

unit_src() {
    local raw="$1"
    local key="${raw#func_}"
    key="${key^^}"
    _fc_load_unit_paths
    local rel="${_FC_UNIT_PATHS[$key]:-}"
    if [[ -z "$rel" ]]; then
        echo "unit_src: cannot resolve unit '$raw' under $DECOMP" >&2
        return 1
    fi
    printf '%s/%s\n' "$DECOMP" "$rel"
}

if [[ -n "${DECOMP:-}" ]]; then
    _fc_load_unit_paths
fi
