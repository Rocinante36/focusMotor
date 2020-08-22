# focusMotor
concept: use a NEMA 17 to control a telescope focuser 
useage design: rough in focus with mechanical focuser
fine adjustment with this module.

initial development on a modified CNCv4 board with a arduino nano, A4988, and keyes encoder 
Push switch to set speed, (or hold in and it steps through) 
rotate encoder to turn focuser
speeds 5-4-3-2-1 5=fastest 1:1 step movement.
1 = 1:1/16 movement 
7 segment LED to show speed. LM74HC595 serial chip to drive LED
the decimal point indicates motor holding on
motor holding releases when the speed switch is clicked or after 10 seconds.
I set the A4988 to a very low current it draws 0.14 holding, 0.04 idle

early design video
https://www.youtube.com/watch?v=n-8QGkv3ibM
