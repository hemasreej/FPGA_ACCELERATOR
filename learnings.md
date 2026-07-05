#### 04-07-2026 
installment of required tools for the development of the project 
python
cmake
ninja
greenpak
open ocd on chip debugger
gdb  GNU Debugger

`openOCD is the software that connects to the microcontroller, then GDB is the software that lets you control and inspect your program while it's running.`

while project making for the ocd
`cd $env:USERPROFILE\.pico-sdk\openocd\0.12.0+dev\scripts`
& $env:USERPROFILE\.pico-sdk\openocd\0.12.0+dev\openocd.exe `
  -f interface/cmsis-dap.cfg `
  -f target/rp2040.cfg`

  05/07/2026

  sleep_ms() - it is blocking code, so the delay().. but sleep_ms is kinda good that delay
  and  non blocking  method is using subtraction of intervals 