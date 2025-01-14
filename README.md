# kiflashrom

A small personal project designed to read memory chips:
- `AT45DB081D` from the Chameleon Mini RevE
- `FM25V02` from the Chameleon Mini RevG
- `W25X20CL` from the Proxmark3 RDV4
- `W25Q16JV` from the Raspberry Pi Pico (RP2040)
- `W25Q32RV` from the Raspberry Pi Pico 2 (RP2350)

## Why?
- The legendary `flashrom` program has an architecture that seems inherently limited to the first 4 JEDEC banks:
  - https://github.com/flashrom/flashrom/blob/52a495b4437b2752650141b3435e82b7de211774/include/flash.h#L444-L450 ;
  - The `FM25V02` from Ramtron uses the 7th bank ;
  - As of today, there are 16 JEDEC banks, which could pose several issues.
- As a Windows user, I wanted a tool that could work without replacing drivers (e.g., `libusb` & related).  
- `FT232H` adapters are inexpensive and provide a usable and well-documented API, even supporting official SPI communication through `libmpsse` (surprisingly reliable, even at 30 MHz).  
- Why not?

## References
- `flashrom`: https://www.flashrom.org/  
- `FT232HQ` : https://ftdichip.com/products/ft232hq/  
- `libmpsse`: https://ftdichip.com/software-examples/mpsse-projects/libmpsse-spi-examples/  

## Licence
CC BY 4.0 licence - https://creativecommons.org/licenses/by/4.0/

## Author
Benjamin DELPY `gentilkiwi`, you can contact me on Twitter ( @gentilkiwi ) or by mail ( benjamin [at] gentilkiwi.com )
