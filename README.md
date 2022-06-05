# Dimming-a-LED-with-the-help-of-an-analog-XY-joystick
A program to control the  brightness of a LED which depends on how the external joystick button is pressed or moved.

XY Joystick
The outputs X and Y each provide an output signal between 0 and 4 volts, depending on the
position of the joystick.
Medium rest position ≙ 2 V
Lever to the right ≙ 4 V
Lever to the left ≙ 0 V
Button switch at rest ≙ 0 V pressed ≙ 3.3 V

Scope of functions to be implemented:
1.) If the joystick is moved to the right, the brightness should increase slowly.
2.) If the joystick is moved to the left, the brightness should decrease slowly.
3.) The last brightness level is retained in the middle rest position.

JoyStick button:
4.) Press and release the button ≙ full brightness permanently.
Press and release the button again ≙ minimum brightness permanently.
Press and release the button again ≙ full brightness again permanently.
Regardless of this, the dimmer function must always be possible


Using the internal ADC, PWMs and  interruptsof the board TM4C1294 ,  we implement the functions above.

To adjust the brightness of the LED we use a PWM, with an adjustable duty cycle in the range between 5% to 95%.

For the realization of the PWM a timer is used, which sets the duty cycle of a 1KHz
square wave frequency in the range from 5% to 95%. This timer determines the low
time and the high time of the output signal. The sum of both times should always
remain the same
