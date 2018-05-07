#include "main.h"
#include "glut.h"

#include <Windows.h>
#include <Ole2.h>

#include <Kinect.h>

#include <Leap.h>
#include <freeglut.h>

using namespace Leap;


#include <iostream>
#include <string>
#include <thread>

using namespace std;

// OpenGL Variables
GLuint textureId;
GLubyte data[width*height*4];

// Kinect variables
IKinectSensor* sensor;         // Kinect sensor
IDepthFrameReader* reader;     // Kinect depth data source

bool waitCond = true;



class SampleListener : public Listener {
public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);

};

//Leap Motion SampleListener method, takes in a controller object which is our Leap Motion in this case
//and tells Connected when there is a Leap object connected to the system. It starts the controller.
void SampleListener::onConnect(const Controller& controller) {
    
	std::cout << "Connected" << std::endl;
    
}

//Leap Motion receives appx. 115 FPS. Here it is described what the program is supposed to do with each of these frames. In our case, we are reading hands to understand the gestures done with them. Getting a list of gestures in each frame. Then checking if there is a gesture that is in our design.
void SampleListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();
	Leap::GestureList gestures = frame.gestures();

	Leap::Hand hand = frame.hands().frontmost();


	for (Leap::GestureList::const_iterator gl = gestures.begin();

		gl != frame.gestures().end(); gl++){

		if ((*gl).type() == Leap::Gesture::TYPE_CIRCLE){
			Leap::CircleGesture cGesture = Leap::Gesture::invalid();
			cGesture = Leap::CircleGesture(*gl);
			if (cGesture.radius()>90){
				std::cout << "Circle." << std::endl;
				exit(0);

			}
		}
		else if ((*gl).type() == Leap::Gesture::TYPE_SWIPE){
			Leap::SwipeGesture swipeGesture = Leap::Gesture::invalid();
			swipeGesture = Leap::SwipeGesture(*gl);
			Leap::Vector swipeDirection = swipeGesture.direction();

			if (swipeGesture.pointable().hand().palmNormal().x < 0 || swipeGesture.pointable().hand().palmNormal().x > 0){
				if (swipeDirection.x < 0 && swipeGesture.position().x < -40){
					std::cout << "Left direction" << std::endl;
					
				}
				if (swipeDirection.x > 0 && swipeGesture.position().x > 40){
					std::cout << "Right direction" << std::endl;
					
				}
			}
		}
		else{
			Leap::Pointable ptbl = (*gl).pointables().frontmost();
			if (ptbl.ZONE_TOUCHING){
				std::cout << "Key Tap buddy." << std::endl;
				if (waitCond){
					glutLeaveMainLoop();
					waitCond = false;
				}
				
			}
		}
	}
}

//drawText method for initial messaging in the system.
void drawText(char *string, float x, float y)
{
	char *c;
	glRasterPos2f(x, y);

	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}

void drawStrokeText(char*string, int x, int y)
{
	char *c;
	glPushMatrix();
	glTranslatef(x, y + 8, 0);
	glScalef(0.36f,0.32f, 0);

	for (c = string; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glColor3f(0, 1, 0);
	drawStrokeText("Welcome to Sandbox! It's a Spatial AR program.", 70, 640);
	drawStrokeText("To start do a key tap motion and to quit do ", 70, 590);
	drawStrokeText("a circular motion with your hand. Enjoy!", 70, 540);
	
	glutSwapBuffers();
}

void reshape(int w, int h)
{

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
//Glut init.
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void textWritingMethod(int argc, char* argv[]){

	// initialize glut 
	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	// specify the display mode to be RGB and single buffering  
	// we use single buffering since this will be non animated 
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	// define the size 
	glutInitWindowSize(1900, 980);

	// the position where the window will appear 
	glutInitWindowPosition(0, 0);

	glutCreateWindow("OpenGL Fonts");

	glutDisplayFunc(render);
	glutIdleFunc(render);
	glutReshapeFunc(reshape);
	glutFullScreen();
	// enter the main loop 
	glutMainLoop();
}

//Kinect initialization for depth reading.
bool initKinect() {
    if (FAILED(GetDefaultKinectSensor(&sensor))) {
		return false;
	}
	if (sensor) {
		sensor->Open();
		IDepthFrameSource* framesource = NULL;
		sensor->get_DepthFrameSource(&framesource);
		framesource->OpenReader(&reader);
		if (framesource) {
			framesource->Release();
			framesource = NULL;
		}
		return true;
	} else {
		return false;
	}
}


//Once Kinect is initialized, this method gets depth information from Kinect. Depending on the depth information the method gives a certain RGB value to each pixel. Then later in OpenGL shader uses these data for texture mapping.
void getKinectData(GLubyte* dest) {
    IDepthFrame* frame = NULL;
    if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) {
        unsigned int sz;
		unsigned short* buf;
		frame->AccessUnderlyingBuffer(&sz, &buf);

		const unsigned short* curr = (const unsigned short*)buf;
		const unsigned short* dataEnd = curr + (width*height);
		double rR, rG, rB = 0.0;

		while (curr < dataEnd) {
			// Get depth in millimeters
			unsigned short depth = (*curr++);
			

			// B,G,R are all set to certain values depending on the depth information.
            //This if else branch basically creates a gradient function for color dissolve from red to blue.
				if (depth < 500){
					rR = 1;
					rG = 0;
					rB = 0;
					//cout << rR << " and " << rG << " and " << rB << endl;

					*dest++ = (double)rB * 255;
					*dest++ = (double)rG * 255;
					*dest++ = (double)rR * 255;
				}
				else if (depth > 500 && depth <= 600){
					rR = 1;
					rG = (double) (depth - 500) / 100;
					rB = 0;
					//cout << rR << " and " << rG << " and " << rB << endl;

					*dest++ = (double) rB*255;
					*dest++ = (double) rG*255;
					*dest++ = (double) rR*255;
				}
				else if (depth > 600 && depth <= 700){
					rR = (double) 1 - (depth - 600) / 100;
					rG = 1;
					rB = 0;
					//cout << rR << " and " << rG << " and " << rB << endl;

					*dest++ = (double)rB * 255;
					*dest++ = (double)rG * 255;
					*dest++ = (double)rR * 255;
				}

				else if (depth > 700 && depth <= 800){
					rR = 0; 
					rG = 1;
					rB = (double) (depth-700)/100;

					*dest++ = (double) rB * 255;
					*dest++ = (double) rG * 255;
					*dest++ = (double) rR * 255;
				}
				else if (depth > 800 && depth <= 900){
					rR = 0;
					rG = (double)1 - (depth - 800) / 100;
					rB = 1;

					*dest++ = (double)rB * 255;
					*dest++ = (double)rG * 255;
					*dest++ = (double)rR * 255;
				}

				/*else if (depth>700 && depth<850){
					*dest++ = 255;
					*dest++ = 0;
					*dest++ = 0;
				}*/
				else{
					*dest++ = (BYTE) depth % 256;
					*dest++ = (BYTE) depth % 256;
					*dest++ = (BYTE) depth % 256;
				}
			*dest++ = 0xff;
		}
    }
    if (frame) frame->Release();
}


//Shader. The worked depth information on each pixel is shaded in this method.
void drawKinectData() {
    glBindTexture(GL_TEXTURE_2D, textureId);
    getKinectData(data);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0, 0, 0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(width, 0, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(width, height, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0, height, 0.0f);
    glEnd();
}

int main(int argc, char* argv[]) {
    //Initialize Leap Moiton
	SampleListener listener;
	Controller controller;
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_SWIPE);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_INVALID);

	controller.addListener(listener);
	
    //Starting text message
	textWritingMethod(argc,argv);

	if (!init(argc, argv)) return 1;
    if (!initKinect()) return 1;

    // Initialize textures
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*) data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // OpenGL setup
    glClearColor(0,0,0,0);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);

    // Camera setup
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Main loop
	execute();

    return 0;
}


