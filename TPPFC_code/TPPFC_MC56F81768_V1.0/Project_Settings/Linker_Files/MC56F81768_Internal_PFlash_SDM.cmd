##
##  Copyright 2020 NXP
##  SPDX-License-Identifier: BSD-3-Clause
##
#####################################################################
##
##  M56F81x6x linker file - Flash mapped into data space range 0x8000~0xbfff is not used 
##  
##  Notes:
##  - Source codes(reside in Flash) that need to run in RAM can be put into "codesInRam.text" section
##
##              Program_space
##              -------------
##    0x0000:
##              vector table
##    0x00E1:
##              -------------
##              -------------
##    0x01E0:
##              bootloader config area
##    0x01FF:
##            -------------
##            -------------
##    0x0200:
##              flash config field
##    0x0207:
##            -------------
##    0x0208:
##            > Codes running in Flash
##    0xBFFF:
##            -------------
##    0xC000:          
##            > Global variables' initial values, namely, "*.data" sections
##            > Size of ".data" sections wasted here
##            > Codes stored in Flash, running in RAM
##    0xFFFF:
##            -------------
##     **************************************************************
##
##              Data_space
##              -------------
##    0x0000:
##              > .data sections
##              > Codes running in RAM
##              > .bss sections
##              > Heap
##              > Stack
##    0x27FF:
##              -------------
##
#####################################################################

FLASH_PARTITION {
  # name : ORIGIN_P = addr, ORIGIN_X = addr, LENGTH = size [> filename]
  FlashImage   : ORIGIN_P = 0x000000, ORIGIN_X = 0x020000, LENGTH = 0x10000 > FlashImage.bin
  # FlashPart1 : ORIGIN_P = 0x000000, ORIGIN_X = 0x020000, LENGTH = 0x10000
}

MEMORY {
  # I/O registers area for on-chip peripherals
  .x_Peripherals (RW)   : ORIGIN = 0xC000, LENGTH = 0

  .p_Interrupts   (RWX): ORIGIN = 0x00000000, LENGTH = 0x000000E2
  .p_Code         (RWX): ORIGIN = 0x00000208, LENGTH = 0x0000BDF8 # shrink p-code by 48KWords for codes running in Flash
  .p_storage      (RWX): ORIGIN = 0x0000C000, LENGTH = 0x00004000 # pFlash subarea for ram-code storage
  .x_Data         (RW) : ORIGIN = 0x00000000, LENGTH = 0x00002800 # 10KWords for data in RAM mapped in data space
  .p_reserved_FCF (RWX): ORIGIN = 0x00000200, LENGTH = 0x00000008 # Flash Configuration Area
  .p_reserved_BCA (RWX): ORIGIN = 0x000001E0, LENGTH = 0x00000020 # Bootloader Configuration Area
  .p_ramSpace     (RX) : ORIGIN = 0x00060000, LENGTH = 0x00002800 # 10KWords of RAM mapped in program space

  # p_flash_ROM_data mirrors x_Data, mapping to origin and length
  # the "X" flag in "RX" tells the debugger flash p-memory.
  # the p-memory flash is directed to the address determined by AT
  # in the data_in_p_flash_ROM section definition
  .p_flash_ROM_data  (RX) : ORIGIN = 0x00000000, LENGTH = 0x00002800  # 10KWords for data with their initial value stored in Flash,
                                                                      # but actual addresses are in RAM of data space
}

KEEP_SECTION { interrupt_vectors.text }
KEEP_SECTION { reserved_FCF.text }
KEEP_SECTION { codesInRam.text }
KEEP_SECTION { reserved_BCA.text}

SECTIONS {
  
  .data_in_p_flash_ROM : AT(ADDR(.p_storage))
  {
    # the data sections flashed to pROM
    # save data start so we can copy data later to xRAM

    __xRAM_data_start = .;
    # do NOT remove this line, as it allocates a Word at address x:0 
    # so no other valid variable gets a NULL address
    WRITEH(0x0BAD);

    # .data sections
    * (.const.data.char)     # used if "Emit Separate Char Data Section" enabled
    * (.const.data)

    * (fp_state.data)
    * (rtlib.data)
    * (.data.char)        # used if "Emit Separate Char Data Section" enabled
    * (.data)

                  
    
    # save data end and calculate data block size
    . = ALIGN(2);
    __xRAM_data_end = .;
    __data_size = __xRAM_data_end - __xRAM_data_start;
    
    __pROM_code_start = ADDR(.p_storage) + __data_size;
  } > .p_flash_ROM_data          # this section is designated as p-memory
                                 # with X flag in the memory map
                                 # the start address and length map to
                                 # actual internal xRAM
  
  
   .ramFunctions : AT(__pROM_code_start)
  {
    . = . + __data_size; # the size of "__data_size" is wasted in Flash
    __pRAM_code_start = .;

    OBJECT(FGFLIB_CtrlPIpAW_F16_FAsm,GFLIB_SDM.lib) # this is an example of how to allocate functions in library to RAM
    OBJECT(FGFLIB_Ramp_F16_FAsm,GFLIB_SDM.lib)
    OBJECT(FGFLIB_Sin_F16_FAsm,GFLIB_SDM.lib)
    OBJECT(FGFLIB_Sqrt_F16l_FAsm,GFLIB_SDM.lib)
    OBJECT(FMLIB_DivSat_F32_FAsm,MLIB_SDM.lib)
    OBJECT(FMLIB_Div1Q_A32ll_FAsm,MLIB_SDM.lib)
    OBJECT(FMLIB_Div_F32_FAsm,MLIB_SDM.lib)
    OBJECT(FMLIB_Div1Q_F32_FAsm,MLIB_SDM.lib)
    
    * (rtlib.text) # suggest to put rtlib function into RAM if ISR performance needs to be improved
    * (codesInRam.text)
    # save address where for the data start in pROM
    . = ALIGN(2);
    __pRAM_code_end = .;
    __ramfunctions_size = __pRAM_code_end - __pRAM_code_start;
  } > .p_ramSpace
    # AT sets the download address
    # The codes are stored just next to data initial values in Flash
  
  .ApplicationCode :
  {
    F_Pcode_start_addr =.;

    # .text sections
    * (.text)
    
    * (startup.text)
    * (fp_engine.text)
    * (ll_engine.text)
    * (user.text)
    * (rtlib.text) 
    * (.data.pmem)
    * (.const.data.pmem)

    F_Pcode_end_addr = .;

    # save address where for the data start in pROM
    . = ALIGN(2);
    
  } > .p_Code
 

  .interrupt_vectors :
  {
    F_vector_addr = .;
    # interrupt vectors
    * (interrupt_vectors.text)
    Fvba_vector_addr = (F_vector_addr >> 4);
  } > .p_Interrupts

  .reserved_FCF :
  {
    F_FCF_addr = .;
    # reserved FCF - Flash Configuration Field
    * (reserved_FCF.text)
  } > .p_reserved_FCF
  
  .reserved_BCA :
  {
    F_BCA_addr = .;
    # reserved BCA - Bootloader Configuration Area
    * (reserved_BCA.text)
  } > .p_reserved_BCA

  .ApplicationData :
  {
    # save space for the pROM data and code copy
    . = __xRAM_data_start + __data_size + __ramfunctions_size;

    # .bss sections
    * (rtlib.bss.lo)
    * (rtlib.bss)
    . = ALIGN(4);
    F_Xbss_start_addr = .;
    _START_BSS = .;
    * (.bss.char)         # used if "Emit Separate Char Data Section" enabled
    * (.bss)
    . = ALIGN(2);         # used to ensure proper functionality of the zeroBSS hardware loop utility
    _END_BSS   = .;
    F_Xbss_length = _END_BSS - _START_BSS;

    /* Setup the HEAP address */
    . = ALIGN(4);
    _HEAP_ADDR = .;
    _HEAP_SIZE = 0x00000000;
    _HEAP_END = _HEAP_ADDR + _HEAP_SIZE;
    . = _HEAP_END;

    /* SETUP the STACK address */
    _min_stack_size = 0x00000100;
    _stack_addr = _HEAP_END;
    _stack_end  = _stack_addr + _min_stack_size;
    . = _stack_end;

    /* EXPORT HEAP and STACK runtime to libraries */
    F_heap_addr   = _HEAP_ADDR;
    F_heap_end    = _HEAP_END;
    F_Lstack_addr = _HEAP_END;
    F_StackAddr = _HEAP_END;
    F_StackEndAddr = _stack_end - 1;

    # runtime code __init_sections uses these globals:
                  
    F_Ldata_size     = __data_size;
    F_Ldata_RAM_addr = __xRAM_data_start;
    F_Ldata_ROM_addr = ADDR(.p_storage);
    
    F_Livt_size     = __ramfunctions_size;
    F_Livt_RAM_addr = __pRAM_code_start;
    F_Livt_ROM_addr = __pROM_code_start + __data_size;


    F_xROM_to_xRAM   = 0x0000;
    F_pROM_to_xRAM   = 0x0001;  # Enable loading data initial value to corresponding variables before entering main function
    F_pROM_to_pRAM   = 0x0001;  # Enable loading codes that residing in Flash but running in RAM to RAM before entering main function

    F_start_bss   = _START_BSS;
    F_end_bss     = _END_BSS;

    __DATA_END=.;
  } > .x_Data
}
