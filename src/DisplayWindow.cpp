//
//  DisplayWindow.cpp
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-08-03.
//

#include "DisplayWindow.h"

ConvexHull* DisplayWindow::_convexHull = nullptr;
int DisplayWindow::instance_count = 0;

float DisplayWindow::angle;
float DisplayWindow::xDir;
float DisplayWindow::zDir;
float DisplayWindow::xPos;
float DisplayWindow::zPos;
