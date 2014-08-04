//
//  DisplayWindow.h
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-08-03.
//

#ifndef imn430_tp3_window_h
#define imn430_tp3_window_h

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
    #include <GL/glu.h>
    #include <GL/gl.h>
#endif

#include "ConvexHull.h"

class DisplayWindow{
public:
    //---- Typedefs
    typedef unsigned int size_type;
    
    //---- Ctor
    DisplayWindow(const size_type wWidth,
                  const size_type wHeigth,
                  const char* wName,
                  ConvexHull* ch = nullptr)
        : width(wWidth), heigth(wHeigth), name(wName){
            
            //No time for singleton... just prevent further instanciation...
            assert(!instance_count++);
            
            _convexHull = ch;
            
            angle = 0.0f;//Camera angle of rotation
            xDir  = 0.0f;//Camera x direction
            zDir  = -1.0f;//Camera z direction
            xPos  = 0.0f;//Camera x position
            zPos  = 5.0f;//Camera z position
            
            glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
            glutInitWindowPosition(0,0);
            glutInitWindowSize(wWidth, wHeigth);
            
            glutCreateWindow(name);
            
            glutDisplayFunc(display);
            glutReshapeFunc(changeSize);
            glutKeyboardFunc(processNormalKeys);
            glutSpecialFunc(processSpecialKeys);
            
            glEnable(GL_DEPTH_TEST);
    }
    
    static void changeSize(int w, int h) {
        
        // Prevent a divide by zero, when window is too short
        // (you cant make a window of zero width).
        if (h == 0)
            h = 1;
        float ratio =  w * 1.0 / h;
        
        // Use the Projection Matrix
        glMatrixMode(GL_PROJECTION);
        
        // Reset Matrix
        glLoadIdentity();
        
        // Set the viewport to be the entire window
        glViewport(0, 0, w, h);
        
        // Set the correct perspective.
        gluPerspective(45.0f, ratio, 0.1f, 100.0f);
        
        // Get Back to the Modelview
        glMatrixMode(GL_MODELVIEW);
    }
    
    static void display(){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        
        gluLookAt(
            xPos, 1.0f, zPos,
            xPos + xDir, 1.0f, zPos + zDir,
            0.0f, 1.0f, 0.0f
        );
        
        if(_convexHull){
            _convexHull->display();
        }
        
        glutSwapBuffers();
    }
    
    static void processNormalKeys(unsigned char key, int x, int y){
        switch(key){
            case 'Q':
            case 'q':
                exit(0);
                break;
            case 27://ESC
                exit(0);
                break;
        }
    }
    
    static void processSpecialKeys(int key, int xx, int yy){
        static const float ratio = 0.1f;
        
        switch(key){
            case GLUT_KEY_LEFT:
                angle -= 0.01f;
                xDir = sin(angle);
                zDir = -cos(angle);
                break;
            case GLUT_KEY_RIGHT:
                angle += 0.01f;
                xDir = sin(angle);
                zDir = -cos(angle);
                break;
            case GLUT_KEY_UP:
                xPos += xDir * ratio;
                zPos += zDir * ratio;
                break;
            case GLUT_KEY_DOWN:
                xPos -= xDir * ratio;
                zPos -= zDir * ratio;
                break;
        }
    }
    
private:
    //Desactivate operators
    DisplayWindow& operator =(const DisplayWindow&);
    
    //---- Members
    //Window param
    static int instance_count;
    size_type width;
    size_type heigth;
    const char* name;
    
    //Camera param
    static float angle;
    static float xDir;
    static float zDir;
    static float xPos;
    static float zPos;
    
    //
    static ConvexHull* _convexHull;
};

#endif
