#!/usr/bin/env python3
"""Check actual ELF/vector linkage; does not claim to validate physical hardware."""
import argparse
from pathlib import Path
import struct
import subprocess
import tempfile


def require(condition, message):
    if not condition:
        raise SystemExit(message)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument("--nm", default="arm-none-eabi-nm")
    parser.add_argument("--objcopy", default="arm-none-eabi-objcopy")
    args = parser.parse_args()
    symbols = {}
    for line in subprocess.check_output(
            [args.nm, "--defined-only", str(args.elf)], text=True).splitlines():
        fields = line.split()
        if len(fields) == 3:
            address, kind, name = fields
            symbols[name] = (int(address, 16), kind)

    with tempfile.TemporaryDirectory() as directory:
        vector_path = Path(directory) / "vectors.bin"
        subprocess.run([args.objcopy, "-O", "binary", "-j", ".isr_vector",
                        str(args.elf), str(vector_path)], check=True)
        vector = vector_path.read_bytes()
    words = struct.unpack(f"<{len(vector) // 4}I", vector)
    require(words[0] == 0x20005000, "Initial MSP must be top of 20 KiB RAM")
    require(symbols["g_pfnVectors"][0] == 0x08000000, "Vector table must start in Flash")
    handlers = {1: "Reset_Handler", 2: "NMI_Handler", 3: "HardFault_Handler",
                4: "MemManage_Handler", 5: "BusFault_Handler", 6: "UsageFault_Handler",
                11: "SVC_Handler", 14: "PendSV_Handler", 15: "SysTick_Handler",
                16 + 30: "TIM4_IRQHandler"}
    for index, name in handlers.items():
        address, kind = symbols[name]
        require(words[index] == (address | 1), f"Vector {index} does not route to {name}")
        if name != "Reset_Handler":
            require(kind == "T", f"{name} must override the weak default handler")
    for name in ("HAL_InitTick", "HAL_TIM_PeriodElapsedCallback",
                 "vApplicationGetIdleTaskMemory", "vApplicationStackOverflowHook"):
        require(symbols.get(name, (0, ""))[1] == "T", f"Missing strong hook: {name}")
    for name in ("pvPortMalloc", "vPortFree", "malloc", "_sbrk", "_write"):
        require(name not in symbols, f"Unexpected heap/stdio dependency: {name}")

    binary = args.elf.with_suffix(".bin").read_bytes()
    require(binary[:len(vector)] == vector, "BIN must include the ELF vector table")
    require(len(binary) <= 64 * 1024, "Binary exceeds 64 KiB Flash")
    require(symbols["_ebss"][0] + 2048 <= symbols["_estack"][0], "No room for MSP stack")
    print(f"PASS: {args.elf}: {len(handlers)} vectors, HAL/RTOS hooks, static allocation, "
          f"Flash/RAM bounds; BIN={len(binary)} bytes")


if __name__ == "__main__":
    main()
