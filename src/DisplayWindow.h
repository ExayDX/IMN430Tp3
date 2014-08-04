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
            
            glutInitWindowPosition(0,0);
            glutInitWindowSize(wWidth, wHeigth);
            glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
            
            glutCreateWindow(name);
            
            glutDisplayFunc(display);
    }
    
    static void display(){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        
        if(_convexHull){
            _convexHull->display();
        }
        
        glutSwapBuffers();
    }
    
private:
    //Desactivate operators
    DisplayWindow& operator =(const DisplayWindow&);
    
    //---- Members
    static int instance_count;
    size_type width;
    size_type heigth;
    const char* name;
    static ConvexHull* _convexHull;
};

#endif
