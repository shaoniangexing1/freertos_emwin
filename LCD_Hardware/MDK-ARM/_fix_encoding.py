#!/usr/bin/env python3
"""Convert GBK/GB2312 user sources to UTF-8 and fix common mojibake."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
USER_ROOTS = [
    ROOT / "MDK-ARM/Inf",
    ROOT / "MDK-ARM/BSP",
    ROOT / "MDK-ARM/App",
    ROOT / "MDK-ARM/Com",
    ROOT / "Core/Src",
    ROOT / "Core/Inc",
    ROOT / "MDK-ARM/Middel/freertos",
    ROOT / "MDK-ARM/Middel/emwin/Config",
    ROOT / "MDK-ARM/Middel/emwin/GUI_NEW_Block",
    ROOT / "MDK-ARM/DebugConfig",
]

MOJIBAKE_RE = re.compile(
    r"[\u00c0-\u00ff]{2,}|[\ufffd]|"
)


def decode_best(raw: bytes) -> tuple[str, str]:
    """Return (text, encoding_used)."""
    for enc in ("utf-8-sig", "utf-8"):
        try:
            t = raw.decode(enc)
            if "\ufffd" not in t and not MOJIBAKE_RE.search(t):
                if re.search(r"[\u4e00-\u9fff]", t) or not _looks_garbled(t):
                    return t, enc
        except UnicodeDecodeError:
            pass

    for enc in ("gbk", "gb2312", "cp936"):
        try:
            t = raw.decode(enc)
            return t, enc
        except UnicodeDecodeError:
            continue

    return raw.decode("utf-8", errors="replace"), "utf-8-replace"


def _looks_garbled(text: str) -> bool:
    if MOJIBAKE_RE.search(text):
        return True
    # GBK misread as latin-1 in comments
    for line in text.splitlines():
        if "//" in line or "/*" in line:
            part = line.split("//", 1)[-1] if "//" in line else line
            if re.search(r"[\u0080-\u024f]{4,}", part) and not re.search(
                r"[\u4e00-\u9fff]", part
            ):
                return True
    return False


def fix_mojibake(text: str) -> str:
    """Try latin-1 -> gbk recovery on comment-like garbled runs."""
    if re.search(r"[\u4e00-\u9fff]", text):
        return text

    def recover(match: re.Match) -> str:
        s = match.group(0)
        try:
            return s.encode("latin-1").decode("gbk")
        except (UnicodeDecodeError, UnicodeEncodeError):
            return s

    # Only touch lines that still look garbled
    out_lines = []
    for line in text.splitlines(keepends=True):
        if _looks_garbled(line):
            line = re.sub(r"[\x80-\xff]{3,}", recover, line)
        out_lines.append(line)
    return "".join(out_lines)


def collect_files() -> list[Path]:
    files: list[Path] = []
    for root in USER_ROOTS:
        if not root.exists():
            continue
        for p in root.rglob("*"):
            if p.suffix.lower() in (".c", ".h", ".cpp", ".hpp") and p.is_file():
                files.append(p)
    return sorted(set(files))


def main() -> None:
    changed = []
    for path in collect_files():
        raw = path.read_bytes()
        if not raw:
            continue
        text, enc = decode_best(raw)
        text = fix_mojibake(text)
        new_raw = text.encode("utf-8")
        if new_raw != raw:
            path.write_bytes(new_raw)
            changed.append((path, enc))
            print(f"CONVERT {path.relative_to(ROOT)} ({enc} -> utf-8)")

    print(f"Done. {len(changed)} file(s) updated.")


if __name__ == "__main__":
    main()
