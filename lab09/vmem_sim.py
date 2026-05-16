import argparse
import random
import sys

PAGE_SIZE = 256
NUM_FRAMES = 100
VA_MAX = 0xFFFF
OCCUPIED_MIN = 10
OCCUPIED_MAX = 60
RANDOMIZE_RETRY_CAP = 100

FREE = "F"
OCCUPIED = "X"


def split_va(va):
    offset = va & 0xFF
    vpn = (va >> 8) & 0xFF
    return vpn, offset


def init_ram(num_virtual_pages, rng):
    required_free = max(10, num_virtual_pages)
    for attempt in range(1, RANDOMIZE_RETRY_CAP + 1):
        occupied_count = rng.randint(OCCUPIED_MIN, OCCUPIED_MAX)
        frames = [FREE] * NUM_FRAMES
        occupied_idxs = rng.sample(range(NUM_FRAMES), occupied_count)
        for i in occupied_idxs:
            frames[i] = OCCUPIED
        free_count = frames.count(FREE)
        if free_count >= required_free:
            return frames, attempt
    raise RuntimeError(
        f"Could not produce a RAM map with FREE >= {required_free} "
        f"after {RANDOMIZE_RETRY_CAP} retries."
    )


def print_ram(frames, seed_label):
    free_count = frames.count(FREE)
    occ_count = frames.count(OCCUPIED)
    print(f"PHYSICAL RAM ({NUM_FRAMES} frames) after random init ({seed_label}):")
    print(f"FREE={free_count} OCCUPIED={occ_count}")
    for row_start in range(0, NUM_FRAMES, 10):
        cells = [f"{i:>2}:{frames[i]}" for i in range(row_start, row_start + 10)]
        print(" ".join(cells))


def allocate_frame(frames):
    for i, state in enumerate(frames):
        if state == FREE:
            frames[i] = OCCUPIED
            return i
    return None


def load_process(frames, num_virtual_pages):
    free_count = frames.count(FREE)
    assert free_count >= num_virtual_pages, (
        f"Sanity check failed: FREE={free_count} < V={num_virtual_pages}"
    )
    page_table = [{"valid": False, "pfn": None} for _ in range(num_virtual_pages)]
    for i in range(num_virtual_pages):
        pfn = allocate_frame(frames)
        if pfn is None:
            for entry in page_table:
                entry["valid"] = False
                entry["pfn"] = None
            raise RuntimeError(f"Allocation failed at VPN {i}; rolled back page table.")
        page_table[i]["valid"] = True
        page_table[i]["pfn"] = pfn
    return page_table


def translate(va_raw, page_table, num_virtual_pages):
    try:
        va = int(va_raw, 0) if isinstance(va_raw, str) else int(va_raw)
    except ValueError:
        return {"raw": va_raw, "error": "PARSE_ERROR"}
    if va < 0 or va > VA_MAX:
        return {"raw": va_raw, "va": va, "error": "VA_OUT_OF_RANGE"}
    vpn, offset = split_va(va)
    if vpn >= num_virtual_pages:
        return {
            "raw": va_raw, "va": va, "vpn": vpn, "offset": offset,
            "error": "VPN_OUT_OF_RANGE",
        }
    pte = page_table[vpn]
    if not pte["valid"]:
        return {
            "raw": va_raw, "va": va, "vpn": vpn, "offset": offset,
            "error": "PAGE_NOT_MAPPED",
        }
    pa = pte["pfn"] * PAGE_SIZE + offset
    return {
        "raw": va_raw, "va": va, "vpn": vpn, "offset": offset,
        "pfn": pte["pfn"], "pa": pa,
    }


def format_result(r):
    raw = r["raw"]
    if r.get("error") == "PARSE_ERROR":
        return f"VA={raw} ERROR=PARSE_ERROR"
    va = r["va"]
    if r.get("error") == "VA_OUT_OF_RANGE":
        return f"VA={raw} ERROR=VA_OUT_OF_RANGE"
    head = f"VA=0x{va:04X} ({va})"
    if r.get("error") == "VPN_OUT_OF_RANGE":
        return f"{head} ERROR=VPN_OUT_OF_RANGE (vpn={r['vpn']})"
    if r.get("error") == "PAGE_NOT_MAPPED":
        return f"{head} VPN=0x{r['vpn']:02X} OFF=0x{r['offset']:02X} ERROR=PAGE_NOT_MAPPED"
    return (
        f"{head} VPN=0x{r['vpn']:02X} OFF=0x{r['offset']:02X} "
        f"PFN={r['pfn']} PA=0x{r['pa']:04X} ({r['pa']})"
    )


def read_addresses(path):
    addresses = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            addresses.append(line)
    return addresses


def main():
    parser = argparse.ArgumentParser(description="Paged address translation simulator.")
    parser.add_argument("-v", "--vpages", type=int, default=8,
                        help="NUM_VIRTUAL_PAGES (1..256). Default 8.")
    parser.add_argument("-f", "--file", default="example_addresses.txt",
                        help="Path to address list file (one VA per line, decimal or 0xhex).")
    parser.add_argument("-s", "--seed", type=int, default=None,
                        help="Optional RNG seed for reproducible RAM map.")
    args = parser.parse_args()

    if not (1 <= args.vpages <= 256):
        print("ERROR: NUM_VIRTUAL_PAGES must be in 1..256", file=sys.stderr)
        sys.exit(2)

    if args.seed is None:
        rng = random.Random()
        seed_label = "seed=time-based"
    else:
        rng = random.Random(args.seed)
        seed_label = f"seed={args.seed}"

    frames, attempts = init_ram(args.vpages, rng)
    print_ram(frames, seed_label)
    print(f"(randomization attempts used: {attempts})")
    print()

    page_table = load_process(frames, args.vpages)
    mapping = [f"VPN {i}->PFN {page_table[i]['pfn']}" for i in range(args.vpages)]
    print(f"Load process: V={args.vpages} -> {', '.join(mapping)}")
    print()

    addresses = read_addresses(args.file)
    for raw in addresses:
        result = translate(raw, page_table, args.vpages)
        print(format_result(result))


if __name__ == "__main__":
    main()
