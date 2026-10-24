#!/usr/bin/env python3
"""Restore the pinned, vendored upstream sources (Python standard library only)."""
import fnmatch
import hashlib
import io
import json
from pathlib import Path, PurePosixPath
import tarfile
import urllib.request

ROOT = Path(__file__).resolve().parent.parent


def main():
    manifest = json.loads((ROOT / "scripts/dependencies.lock.json").read_text())
    cache = ROOT / ".cache/dependencies"
    cache.mkdir(parents=True, exist_ok=True)
    for package in manifest["packages"]:
        archive = cache / (package["name"] + "-" + package["commit"] + ".tar.gz")
        if not archive.exists():
            url = f'https://codeload.github.com/{package["repo"]}/tar.gz/{package["commit"]}'
            print(f'Downloading {package["name"]} @ {package["commit"]}', flush=True)
            with urllib.request.urlopen(url, timeout=60) as response:
                data = response.read()
            archive.write_bytes(data)
        data = archive.read_bytes()
        digest = hashlib.sha256(data).hexdigest()
        if digest != package["sha256"]:
            raise SystemExit(f'Archive checksum mismatch: {archive}')
        matches = [0] * len(package["rules"])
        with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as source:
            for member in source.getmembers():
                if not member.isfile():
                    continue
                relative = "/".join(PurePosixPath(member.name).parts[1:])
                for index, (pattern, destination) in enumerate(package["rules"]):
                    # Match a single directory level; do not pull in other ports.
                    if (fnmatch.fnmatchcase(relative, pattern)
                            and relative.count("/") == pattern.count("/")):
                        target = ROOT / destination
                        if "*" in pattern:
                            target /= PurePosixPath(relative).name
                        target.parent.mkdir(parents=True, exist_ok=True)
                        target.write_bytes(source.extractfile(member).read())
                        matches[index] += 1
        if not all(matches):
            raise SystemExit(f'Unmatched source rules: {package["name"]}: {matches}')
        print(f'{package["name"]}: restored {sum(matches)} files', flush=True)


if __name__ == "__main__":
    main()
