# Spatial-Augmented-Reality-Sandbox
Spatial Augmented Reality Sandbox that allow users to play with the sand and the system responds in real time topological mapping. Additionally, the system is buttonless. Users start, change mode and end the program by hand gestures. In this model, Kinect is used for depth reading and Leap Motion for hand gesture recognition. 

Main.cpp File utilizes Kinect object for depth reading and Leap Motion object for hand gestures recognition.
In order to do this, you must have Kinect SDK and Leap Motion SDK downloaded on your computer and added properly in references.

Additionally, OpenGL's 'freeglut' library is used for Topological Mapping on the sand.
Make sure to add that to your system/program's references.

Lastly, glut.cpp and sdl.cpp files are overriden. Current program has specific needs for hand gesture reading and topological mapping. In order to meet those needs. We have added some methods and overridden some methods in glut. Do not hesitate to send e-mails for further questions about this.

