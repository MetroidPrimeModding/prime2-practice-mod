from dataclasses import dataclass
from src.DataReader import DataReader
from src.DataWriter import DataWriter
from typing import List

@dataclass
class SectionInfo:
    offset: int
    loading: int
    size: int

@dataclass
class DolHeader:
    text: List[SectionInfo]
    data: List[SectionInfo]
    bss_addr: int
    bss_size: int
    entry: int

    @staticmethod
    def parse(src: DataReader):
        text_offset = [src.read_u32(i * 4) for i in range(0, 7)]
        data_offset = [src.read_u32(0x1c + i * 4) for i in range(0, 11)]
        text_loading = [src.read_u32(0x48 + i * 4) for i in range(0, 7)]
        data_loading = [src.read_u32(0x64 + i * 4) for i in range(0, 11)]
        text_size = [src.read_u32(0x90 + i * 4) for i in range(0, 7)]
        data_size = [src.read_u32(0xAC + i * 4) for i in range(0, 11)]
        bss_addr = src.read_u32(0xD8)
        bss_size = src.read_u32(0xDC)
        entry = src.read_u32(0xE0)

        text = [SectionInfo(offset=text_offset[i], loading=text_loading[i], size=text_size[i]) for i in range(0, 7)]
        data = [SectionInfo(offset=data_offset[i], loading=data_loading[i], size=data_size[i]) for i in range(0, 11)]

        return DolHeader(
            text=text,
            data=data,
            bss_addr=bss_addr,
            bss_size=bss_size,
            entry=entry
        )

    def write(self, dest: DataWriter):
        for i in range(0, 7):
            dest.write_u32(i * 4, self.text[i].offset)
        for i in range(0, 11):
            dest.write_u32(0x1c + i * 4, self.data[i].offset)
        for i in range(0, 7):
            dest.write_u32(0x48 + i * 4, self.text[i].loading)
        for i in range(0, 11):
            dest.write_u32(0x64 + i * 4, self.data[i].loading)
        for i in range(0, 7):
            dest.write_u32(0x90 + i * 4, self.text[i].size)
        for i in range(0, 11):
            dest.write_u32(0xAC + i * 4, self.data[i].size)
        dest.write_u32(0xD8, self.bss_addr)
        dest.write_u32(0xDC, self.bss_size)
        dest.write_u32(0xE0, self.entry)

    def size_of_dol(self):
        max_end = 0
        for seg in self.text + self.data:
            end = seg.offset + seg.size
            if end > max_end:
                max_end = end
        return max_end

    def print(self):
        print("DOL Header:")
        for i, seg in enumerate(self.text):
            if seg.size == 0:
                continue
            print(f" Text Segment {i}: Offset: 0x{seg.offset:08X}, Loading: 0x{seg.loading:08X}, Size: 0x{seg.size:08X}, End: 0x{(seg.loading + seg.size):08X}")
        for i, seg in enumerate(self.data):
            if seg.size == 0:
                continue
            print(f" Data Segment {i}: Offset: 0x{seg.offset:08X}, Loading: 0x{seg.loading:08X}, Size: 0x{seg.size:08X}, End: 0x{(seg.loading + seg.size):08X}")
        print(f" BSS Address: 0x{self.bss_addr:08X}, BSS Size: 0x{self.bss_size:08X}, end: 0x{(self.bss_addr + self.bss_size):08X}")
        print(f" Entry Point: 0x{self.entry:08X}")
