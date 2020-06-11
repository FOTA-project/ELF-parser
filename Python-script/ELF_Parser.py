import sys, os, re
from elftools.common.py3compat import bytes2str
from elftools.elf.elffile import ELFFile

def ParseElfFile(elfFileArg):
    FileName=elfFileArg
    with open(FileName, 'rb') as f:
        e = ELFFile(f)
        TextPhysicalAddress = 0
        APPSize = 0
        EntryPoint = e['e_entry']
        DataPhysicalAddress = 0
        DataSize = 0
        
        print('Entry point = ' + hex(e['e_entry']))

        for i in range(int(e.num_segments())):
            segment = e.get_segment(i)
            physicalAddress = hex(segment.header['p_paddr'])
            offest = hex(segment.header['p_offset'])
            size = hex(segment.header['p_filesz'])
            APPSize += segment.header['p_filesz']

            name = ''
            if i == 0:
                name = '.text'
                TextPhysicalAddress = segment.header['p_paddr']
            elif i == 1:
                name = '.data'
                DataPhysicalAddress = segment.header['p_paddr']
                DataSize = segment.header['p_filesz']
            elif i == 2:
                name = '.bss'

            print('Section name = ' + name)
            print('Section physical address = ' + physicalAddress)
            print('Section offest = ' + offest)
            print('Section size = ' + size+'\n')
              
        # the text section
        #textSec = e.get_section_by_name('.text')
        textSec = e.get_segment(0)
        textval = textSec.data()
        
        #data section
        datasec = e.get_segment(1)
        dataval = datasec.data()

        file = open('TEXT_FILE.txt', 'wb')
        file.write(textval)
        file.close

        file = open('DATA_FILE.txt', 'wb')
        file.write(dataval)
        file.close

        print("EntryPoint = " + hex(EntryPoint))
        print("APPSize = " + hex(APPSize))
        print("TextPhysicalAddress = " + hex(TextPhysicalAddress))
        print("DataPhysicalAddress = " + hex(DataPhysicalAddress))
        print("DataSize = " + hex(DataSize))
        
        file = open('INFO_FILE.txt', 'wb')
        file.write(int_to_bytes(TextPhysicalAddress))
        file.write(int_to_bytes(APPSize))
        file.write(int_to_bytes(EntryPoint))
        file.write(int_to_bytes(DataPhysicalAddress))
        file.write(int_to_bytes(DataSize))
        file.close

def int_to_bytes(x: int) -> bytes:
    return x.to_bytes(4, byteorder="little", signed=False)

if __name__ == '__main__':
  main()

