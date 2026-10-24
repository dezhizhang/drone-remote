#!/usr/bin/env python3
"""Export the native Make compile recipes as a compilation database."""
import argparse
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parent.parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--make", default="make")
    parser.add_argument("--build", choices=("debug", "release"), default="debug")
    parser.add_argument("--compiler", default="arm-none-eabi-gcc")
    args = parser.parse_args()
    compiler = shutil.which(args.compiler)
    if compiler is None:
        raise SystemExit(f"Compiler not found: {args.compiler}")

    # Use a target that does not depend on compdb, avoiding recursive generation.
    # Command-line Make overrides also arrive through MAKEFLAGS/MAKEOVERRIDES.
    result = subprocess.run(
        [args.make, "--no-print-directory", "--always-make", "--dry-run",
         "firmware", f"BUILD={args.build}", f"CC={compiler}"],
        cwd=ROOT, text=True, capture_output=True, check=True,
    )
    entries = []
    for line in result.stdout.splitlines():
        command = shlex.split(line)
        if not command or command[0] != compiler or "-c" not in command:
            continue
        source = command[command.index("-c") + 1]
        output = command[command.index("-o") + 1]
        entries.append({"directory": str(ROOT), "file": str(ROOT / source),
                        "output": str(ROOT / output), "arguments": command})
    if not entries:
        raise SystemExit("No compile commands found in the Make build plan")

    destination = ROOT / "compile_commands.json"
    content = json.dumps(entries, indent=2) + "\n"
    if not destination.exists() or destination.read_text() != content:
        # Readers must never see a partial database during a parallel build.
        with tempfile.NamedTemporaryFile(mode="w", dir=ROOT, delete=False) as stream:
            temporary = Path(stream.name)
            try:
                stream.write(content)
                stream.flush()
                os.replace(temporary, destination)
            finally:
                temporary.unlink(missing_ok=True)
    print(f"compile_commands.json: {len(entries)} compile commands ({args.build})")


if __name__ == "__main__":
    main()
