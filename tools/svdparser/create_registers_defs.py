# SPDX-License-Identifier: MIT

################################
# Created by lebakassemmerl 2023 #
# E-Mail: hotschi@gmx.at       #
################################

# dependencies:
# pip install cmsis_svd
# pip install setuptools

from cmsis_svd.parser import SVDParser
import copy
from dataclasses import dataclass
import datetime
import os
import sys

@dataclass
class RegisterArray:
    cnt: int

@dataclass
class RegisterData:
    offset: int
    size: int
    name: str
    access: str
    array: RegisterArray

    def empty_array():
        return RegisterData(0, 0, "", "", RegisterArray(0))

    def end_addr(self):
        if self.array == None:
            return self.offset + (self.size // 8) - 1
        else:
            return self.offset + (self.size // 8) * self.array.cnt - 1

def pascal_case(text):
    if len(text) > 1:
        return f"{text[0].upper()}{text[1:].lower()}"
    else:
        return text.upper()

def fill_reserved_regs(prev_addr, new_addr, reg_size, reserved_num):
    reg_bytes = reg_size // 8
    diff = int(new_addr - (0 if prev_addr == 0 else prev_addr + 1))
    fill_bytes = diff // reg_bytes

    if diff == 0:
        return False, ""

    if (prev_addr == 0) and (new_addr == 0):
        return False, ""

    if diff % reg_bytes != 0:
        raise RuntimeError(f"Registers are not aligned: prev_addr: 0x{prev_addr:04X}, new_addr: 0x{new_addr: 08X}, size: {reg_size}bytes")

    return True, f"Reserved<uint{reg_size}_t> _reserved{reserved_num}[{fill_bytes}];"

def create_cpp_file(path, name, base_addr, reg_defs, bitfields):
    classname = f"{name}Registers"
    header = f"""/*
 * Created by lebakassemmerl {datetime.date.today().year}
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <type_traits>
#include <cstddef>
#include <cstdint>

#include "helpers.h"
#include "register.h"

#pragma pack(1)
class {classname} {{
public:
    {classname}() = delete;
    {classname}({classname}&) = delete;
    {classname}({classname}&&) = delete;
    ~{classname}() = delete;
"""

    middle = f"""
}};
#pragma pack()

static_assert(std::is_standard_layout<{classname}>::value, "{classname} isn't standard layout");

constexpr size_t {name.upper()}_BASE = 0x{base_addr:08X};
"""

    with open(path, "w") as f:
        f.write(header)
        for regdef in reg_defs:
            f.write(f"\n    {regdef}")
        f.write(middle)

        if len(bitfields) > 0:
            f.write(f"\nnamespace {name.lower()}regs {{")
            for bitfield in bitfields:
                f.write(f"\n    {bitfield}")

            f.write("\n}\n")

def parse_peripheral(name, abbrev, peripheral, outputdir):
    registers = []          # store the parsed register-data in here for post-processing
    reg_defs = []           # store the final register strings for the cpp file here
    bitfields = []          # store the final bitfield strings for the cpp file here
    reserved_num = int(0)   # count the number of reserved registers
    end_prev_reg = int(0)   # store the end of the previous register to calculate the reserved ones
    bitfield_arr_name = ""  # tmp variable to filter out multiple defined bitfields with '[%s]'

    def append_reg_array(reg_arr, regs):
        regs.append(copy.deepcopy(reg_arr))
        reg_arr.array.cnt = 0

    def strip_abbrev(text, abbrev):
        for a in abbrev:
            text = text.replace(a, "", 1)

        return text

    reg_array = RegisterData.empty_array()
    for reg in peripheral.registers:
        # parse the actual hardware registers
        regname = strip_abbrev(reg.display_name.lower(), abbrev)
        if regname == "int":
            regname = "intctl"

        if ("[" in reg.name) and ("]" in reg.name):
            # we got an array of equal registers
            if "[0]" in reg.name:
                if reg_array.array.cnt > 0:
                    append_reg_array(reg_array, registers)

                reg_array.name = regname.replace("[%s]", "x")
                reg_array.offset = reg.address_offset
                reg_array.size = reg.size
                reg_array.access = reg.access

            reg_array.array.cnt += 1
        else:
            if reg_array.array.cnt > 0:
                append_reg_array(reg_array, registers)

            registers.append(RegisterData(reg.address_offset, reg.size, regname, reg.access, None))

        # create bitfields, don't create the bitfields for IE, SETIFG, and CLRIFG registers since
        # they are redundant to the IFG registers
        if len(reg.fields) > 0\
            and ("Interrupt Enable" not in reg.description)\
            and ("Clear Interrupt Flag" not in reg.description)\
            and ("Set Interrupt Flag" not in reg.description):
            if regname[0] == "_":
                regname = regname[1:]

            if "[%s]" in regname:
                if bitfield_arr_name == regname:
                    continue
                else:
                    bitfield_arr_name = regname
                    regname = regname.replace("[%s]", "")

            bitfields.append(f"namespace {regname} {{")
            for field in reg.fields:
                bit_low = field.bit_offset
                bit_high = bit_low + field.bit_width - 1

                fieldname = strip_abbrev(field.name.lower(), abbrev)
                if fieldname[0] == "_":
                    fieldname = fieldname[1:]
                if fieldname[0].isnumeric():
                    fieldname = "_" + fieldname

                bitfields.append(
                    f"    constexpr BitField<uint{reg.size}_t> {fieldname}{{{bit_high}, {bit_low}}};")

            bitfields.append("}")

    if reg_array.array.cnt > 0:
        append_reg_array(reg_array, registers)

    registers.sort(key=lambda reg: reg.offset)

    # generate register definitions
    for reg in registers:
        tmp = ""
        if reg.access == "read-write":
            tmp += "ReadWrite<"
        elif reg.access == "read-only":
            tmp += "ReadOnly<"
        elif reg.access == "write-only":
            tmp += "WriteOnly<"
        elif reg.access == None:
            # probably a mixed register with some bits RW and some RO/WO -> use read-write
            tmp += "ReadWrite<"
        else:
            raise RuntimeError(f"Invalid register-access type: {reg.access}, reg: {reg.name}")

        pad, pad_str = fill_reserved_regs(end_prev_reg, reg.offset, reg.size, reserved_num)
        if pad:
            reg_defs.append(pad_str)
            reserved_num += 1

        if reg.array == None:
            tmp += f"uint{reg.size}_t> {reg.name};"
        else:
            tmp += f"uint{reg.size}_t> {reg.name}[{reg.array.cnt}];"

        reg_defs.append(tmp)
        end_prev_reg = reg.end_addr()

    create_cpp_file(
        f"{outputdir}/{name.lower()}_regs.h",
        name,
        peripheral.base_address,
        reg_defs,
        bitfields)

def main():
    OUT_DIR = "out"

    try:
        parser = SVDParser.for_xml_file("../../msp432.svd")
    except:
        parser = SVDParser.for_xml_file("msp432.svd")

    if not os.path.exists(OUT_DIR):
        os.mkdir(OUT_DIR)

    f_base = open(f"{OUT_DIR}/bases.txt", "w")

    for peripheral in parser.get_device().peripherals:
        # print("%s @ 0x%08x" % (peripheral.name, peripheral.base_address))

        name = pascal_case(peripheral.name)
        if "_" in peripheral.name:
            parts = peripheral.name.split("_")
            name = pascal_case(parts[0])
            if len(parts) > 1:
                name += pascal_case(parts[1])

        if peripheral.name == "ADC14":
            parse_peripheral("Adc", ["ADC14", "adc14"], peripheral, OUT_DIR)
        elif "AES256" in peripheral.name:
            parse_peripheral("Aes", ["aes256", "aes"], peripheral, OUT_DIR)
        elif "CAPTIO" in peripheral.name:
            idx = int(peripheral.name.replace("CAPTIO", ""))
            f_base.write(f"{peripheral.name}: 0x{peripheral.base_address:08X}\n")
            parse_peripheral(name.replace(f"Captio{idx}", "CapTIo"), ["captio"], peripheral, OUT_DIR)
        elif "COMP_E" in peripheral.name:
            idx = int(peripheral.name.replace("COMP_E", ""))
            f_base.write(f"{peripheral.name}: 0x{peripheral.base_address:08X}\n")
            parse_peripheral(name.replace(f"CompE{idx}", "Comp"), ["ce"], peripheral, OUT_DIR)
        elif "EUSCI_A" in peripheral.name:
            idx = int(peripheral.name.replace("EUSCI_A", ""))
            f_base.write(f"{peripheral.name}: 0x{peripheral.base_address:08X}\n")
            parse_peripheral(name.replace(f"EusciA{idx}", "UsciA"), ["ucax", "uc"], peripheral, OUT_DIR)
        elif "EUSCI_B" in peripheral.name:
            idx = int(peripheral.name.replace("EUSCI_B", ""))
            f_base.write(f"{peripheral.name}: 0x{peripheral.base_address:08X}\n")
            parse_peripheral(name.replace(f"EusciB{idx}", "UsciB"), ["ucbx", "uc"], peripheral, OUT_DIR)
        elif "REF_A" in peripheral.name:
            parse_peripheral("Ref", ["ref"], peripheral, OUT_DIR)
        elif "RTC_C" in peripheral.name:
            parse_peripheral("Rtc", ["rtcc", "rtc"], peripheral, OUT_DIR)
        elif "TIMER_A" in peripheral.name:
            idx = int(peripheral.name.replace("TIMER_A", ""))
            f_base.write(f"{peripheral.name}: 0x{peripheral.base_address:08X}\n")
            parse_peripheral("TimerA", [f"timera{idx}", "TA", "tax", "ta"], peripheral, OUT_DIR)
        elif peripheral.name == "WDT_A":
            parse_peripheral("Wdt", ["wdt"], peripheral, OUT_DIR)
        else:
            parse_peripheral(name, [name], peripheral, OUT_DIR)

    f_base.close()

    return 0

if __name__ == "__main__":
    sys.exit(main())
