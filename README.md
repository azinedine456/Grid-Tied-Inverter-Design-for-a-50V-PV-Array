# Grid-Tied-Inverter-Design-for-a-50V-PV-Array

This is the github reposity of an academic project at the EiCNAM. (LTspice model uses the [LTspiceControlLibrary-master](https://github.com/kanedahiroshi/LTspiceControlLibrary) library.) 

## Project Objectives 
- Developing the methodological skills necessary to effectively plan and manage a technical project.
- Designing an inverter leg that meets project specifications.
- Simulating a full inverter on LTspice.
- Experimenting with different controllers & evaluating current control system performance (Proportional / Hysterisis). 

## LTSpice Ciruict & Simulation 
<img width="1913" height="849" alt="image" src="https://github.com/user-attachments/assets/368bd81f-4006-4ca9-a723-0398acfd6563" />
<img width="1916" height="616" alt="image" src="https://github.com/user-attachments/assets/b619eb29-b2d7-4730-83e6-044b7098acc4" />
-High current peaks are due to  diode reverse recovery current => LC input filter needed.



## Results (Hysterisis Controller Implementation on Arduino)
<img width="800" height="480" alt="TEK00016" src="https://github.com/user-attachments/assets/09620835-e3fe-4911-a918-1831ca616ee6" />

-Pink: Current 
-Red: Current Reference
-Blue: Grid Voltage
