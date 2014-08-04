//
//  main.cpp
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-07-16.
//

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
    #include <GL/glu.h>
    #include <GL/gl.h>
#endif

#include "DisplayWindow.h"

enum eWindowParam{
    WINDOW_WIDTH = 1280,
    WINDOW_HEIGTH = 720
};
static const char* WINDOW_TITLE = "Convex Hull";

int main(int argc, char * argv[]){
    glutInit(&argc, argv);
    
    ConvexHull convexHull("ananas.txt");
	
    if(convexHull.computeConvexHull()){
        DisplayWindow dw(WINDOW_WIDTH, WINDOW_HEIGTH, WINDOW_TITLE, &convexHull);
        glutMainLoop();
    }
    
    return 0;
}

