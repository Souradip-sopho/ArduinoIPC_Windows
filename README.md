# ArduinoIPC_Windows

Free library for Interprocess Communication with Arduino in Windows. Contains library models for Hardware-in-Loop simulation with Arduino using Modelica and Modelica_DeviceDrivers in Windows. 

## Library description
The `ArduinoIPC_Windows` library is an open source Modelica package for Hardware-in-Loop simulations  involving Arduino platforms using Interprocess Communication in Windows.

Main features:
  * Support for Windows.
  * (Soft) real-time synchronization of a simulation.
  
Please note that the library is known to work with
* OpenModelica (partial support starting with OpenModelica v1.11.0, e.g. serial port).

## Prerequisites
  * `OpenModelica` (>= v1.11.0) (https://www.openmodelica.org/download/download-windows)
  * `Modelica_DeviceDrivers` (v1.7.1) (https://github.com/modelica/Modelica_DeviceDrivers/releases/tag/v1.7.1)
  * `Servomechanisms`(https://github.com/afrhu/Servomechanisms.git)


* Install and Run:
Launch OMEdit with admin priviledeges and load the package `HIL_ArduinoIPC`(HIL\_ArduinoIPC/package.mo) and `InterProcessCommunication`(InterProcessCommunication/package.mo). Also, load the `Servomechanisms` package. Load the Arduino platform with the `IPCArdNew.ino` code in ArduinoCode/IPCArdNew directory. Create any model using the package and simulate.

## Running Test Simulation
  Test the package using `HIL_Arduino_servomech.mo` test provided.
  * Load the Arduino platform with the `IPCArdNew.ino` code.
  * Compile the SerialSHM_Win.c file in HIL\_ArduinoIPC/Resources/Include using the following command
  ```
  $ gcc SerialSHM_Win.c -o SerialSHM_Win -lws2_32
  ```
  * Execute the SerialSHM_Win file in command prompt using the following command
  ```
  $ Serial_SHM.exe
  ```
  * Load the `HIL_Arduino_servomech.mo` test model present in package.
  * Set simulation flag '-rt=1' in OMEdit. Simulate the model. If no error occurs, the package is good to go.
  
For further information:  Visit https://build.openmodelica.org/Documentation/Modelica.html , 
                          https://build.openmodelica.org/Documentation/Servomechanisms.html

## Development and contribution
Main developers:
* [Souradip Pal](https://github.com/Souradip-sopho), contribution to the Windows specific code

Contributions in shape of [Pull Requests] are always welcome.