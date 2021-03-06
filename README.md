# Magic Fire
## This study project is based on the code repository from Thomas Wolf: https://github.com/thomwolf/Magic-Sand 

The aim of the project is to create a fire simulation model that visualises the behaviour of fire depending on the topography, wind direction and speed. The model is addressing firefighters, who might use the model to get specialized in recognizing risks connected to these dependencies. The sandbox allows interaction and shows how the change of wind direction or wind speed affects fire spreading. 
Particular challenges of this study project were working based on existing code and learning a new programming language. 

Magic Fire is a software which may be operated on an augmented reality sandbox like the [Augmented Reality Sandbox](https://arsandbox.ucdavis.edu/).

## Main Features

To run the software the computer needs to be connected to a kinect sensor and a projector.
On the input side the kinect sensor creates a digital elevation model of the sandbox. The projector draws colours as a function of the input values, i.e. the sand level. Thus the sandbox is being transformed into in a colourful topographical map.

A fire simulation is included with factors such as vegetation, slope, wind and general topography influencing the spread of fire. The fire is being visualized in the sandbox and leaves burnt areas on its trail.

The user can control the wind direction and its speed as well as change the topography by shaping rivers and lakes or forming mountains within the sandbox. Risk zones for fire ignition areas with south aspects and slopes of at least 10 degrees can be rendered as an overlay after the calculation. 
Additionally fires may be placed manually anywhere within the extent of the sandbox. A wind arrow indicates the wind as it is dynamically rotated and scaled according to the settings of wind direction and speed. The simulation can be paused and resumed at any time.

Specified information about the dependencies and a quick start for editing the source code can be received from the [readme file](https://github.com/thomwolf/Magic-Sand/blob/master/README.md) by Thomas Wolf.

