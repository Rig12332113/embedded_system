### STM32:
- Create a new empty program and copy the content of `arm_fir_example_f32.c` to your own `main.cpp`
- Add library mbed-dsp to the project
- Add library BSP_B-L475E-IOT01 as well to access on-board sensors
- Create a file named `mbed_app.json` to modify the printf setting
- Run and record the sensor input/FIR output data
### Matlab:
- Copy the recorded data in `Data.py`
- Run `dsp.m` to plot the result

![image](https://hackmd.io/_uploads/r1lcmc-6ma.png)
![image](https://hackmd.io/_uploads/SkON9Z6Q6.png)
