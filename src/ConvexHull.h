#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include <vector>

#include "Vertex.h"
#include "vect.h"
#include <map>

class ConvexHull
{
	std::vector<DCEL::Vertex> pointList;
	std::vector<DCEL::Vertex> convexHullPoints;
	std::vector<DCEL::Region*> Clist;

	static const int NB_POINTS_LIMIT = 80; 
	std::map<DCEL::Vertex*, std::vector<DCEL::Region*> > Fconflit; 
	std::map<DCEL::Region*, std::vector<DCEL::Vertex*> > Pconflit; 

	void initializeConflictsGraph(); 
	bool faceIsVisible(DCEL::Vertex*, DCEL::Region*);
	DCEL::Region* createAFace(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3);
	std::vector<DCEL::Vertex*> sortPointsCCw(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3);
	std::vector<DCEL::Edge*> findHorizon(DCEL::Vertex* p); 

public :
	ConvexHull();
	ConvexHull(const char* filename); 
	~ConvexHull(); 

	bool loadFile(const char* filename); 
	void computeConvexHull();
	bool isCoplanar(const vect& ab, const vect& ac, const vect& ad);
};

#endif