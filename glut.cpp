#include "glut.h"
#include "main.h"
#include <freeglut.h>

void draw() {
   drawKinectData();
   glutSwapBuffers();
}

//Execute method for OpenGL Main loop and exit
void execute() {
    glutMainLoop();
    
	glutLeaveMainLoop();
}

bool init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width,height);
    glutCreateWindow("Kinect SDK Tutorial");
    glutDisplayFunc(draw);
    glutIdleFunc(draw);
	glutFullScreen();
    return true;
    
}
