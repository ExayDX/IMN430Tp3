#include "ConvexHull.h"
#include "Region.h"

#include <algorithm>

#include <iostream>
using std::cout;
using std::endl;
using std::cin;

#include <map>
using std::map;

#include <fstream>
#include <string>
#include <sstream>
#include <cctype>

#include <cassert>

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
    #include <GL/glu.h>
    #include <GL/gl.h>
#endif


ConvexHull::ConvexHull()
{

}

ConvexHull::ConvexHull(const char* filename)
{
	if (!loadFile(filename))
		cout << "Unable to load file..." << endl;
}

ConvexHull::~ConvexHull(){}


bool ConvexHull::loadFile(const char* filename)
{
	bool success = false;

	if (filename)
	{
		pointList.clear();

		std::ifstream ifs(filename, std::ifstream::in);
		
		success = ifs.is_open(); 
		if (success)
		{
			int counter = 0; //limits the number of points considered
			while (ifs.good() && !ifs.eof() && counter != NB_POINTS_LIMIT)
			{
				++counter;
				DCEL::Vertex pt;
				ifs >> pt;
				pointList.push_back(pt);
			}

			ifs.close();
		}
	}

	return success;
}

bool ConvexHull::isCoplanar(const vect& ab, const vect& ac, const vect& ad){
	return abs(ab.cross(ac).dot(ad)) > std::numeric_limits<double>::epsilon();
}

//FIXME: a tester
void ConvexHull::initializeConflictsGraph()
{
    //for(auto fIter = Clist.begin(); fIter != Clist.end(); ++fIter)
    //{
    //    for(auto pIter = pointList.begin(); pIter != pointList.end(); ++pIter)
    //    {
    //        if ( faceIsVisible(&(*pIter), *fIter) )
    //        {
    //            Pconflit[*fIter].push_back(&(*pIter));
    //            Fconflit[&(*pIter)].push_back(*fIter);
    //        }
    //    }
    //}
}


//TO DO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void ConvexHull::fuseRegions(DCEL::Region* r1, DCEL::Region* r2)
{

}



bool ConvexHull::faceIsVisible(DCEL::Vertex* pt, DCEL::Region* r)
{
	DCEL::Vertex p1 = *(r->getBound()->getOrigin());
	DCEL::Vertex p2 = *(r->getBound()->getEnd());
	DCEL::Vertex p3 = *(r->getBound()->getNext()->getEnd());

	vect pt2face(*pt, p1);

	vect p12p2(p1, p2);
	vect p22p3(p2, p3);
	vect rNormal(p12p2.cross(p22p3));
	
	//If the normal is in the opposite direction of pt2centroid; 
	if (pt2face.dot(rNormal) < 0)
	{
		return true;
	}
	return false;
}

void ConvexHull::createFirstTetraedron(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3, DCEL::Vertex* p4)
{	
	//Creating region 1
	//Get a counter-clockwise orientation
	std::vector<DCEL::Vertex*> face1 = sortPointsCCw(p1, p2, p3);
	
	DCEL::HalfEdge* e12 = new DCEL::HalfEdge(face1[0]);
	DCEL::HalfEdge* e21 = new DCEL::HalfEdge(face1[1]);
	e12->setTwin(e21);

	DCEL::HalfEdge* e23 = new DCEL::HalfEdge(face1[1]);
	DCEL::HalfEdge* e32 = new DCEL::HalfEdge(face1[2]);
	e23->setTwin(e32);

	DCEL::HalfEdge* e31 = new DCEL::HalfEdge(face1[2]);
	DCEL::HalfEdge* e13 = new DCEL::HalfEdge(face1[0]);
	e31->setTwin(e13);

	e12->setNext(e23);
	e23->setNext(e31);
	e31->setNext(e12);

	DCEL::Region* r1 = new DCEL::Region(e12);

	e12->setRegion(r1);
	e23->setRegion(r1);
	e31->setRegion(r1);
	
	//Creating region 2
	//Because we use twin edges from the first region which is counter clockwise
	//we are sure reigion 2 will be oriented counter clockwise too. 

	DCEL::Region* r2 = new DCEL::Region(e21);

	DCEL::HalfEdge* e14 = new DCEL::HalfEdge(e21->getEnd());
	DCEL::HalfEdge* e41 = new DCEL::HalfEdge(p4);
	e14->setTwin(e41);

	DCEL::HalfEdge* e42 = new DCEL::HalfEdge(p4);
	DCEL::HalfEdge* e24 = new DCEL::HalfEdge(e21->getOrigin());
	e42->setTwin(e24);

	e21->setNext(e14);
	e14->setNext(e42);
	e42->setNext(e21);

	e21->setRegion(r2);
	e14->setRegion(r2);
	e42->setRegion(r2);

	//Creating region 3
	DCEL::Region* r3 = new DCEL::Region(e32);

	DCEL::HalfEdge* e43 = new DCEL::HalfEdge(p4);
	DCEL::HalfEdge* e34 = new DCEL::HalfEdge(e32->getOrigin());

	e32->setNext(e24);
	e24->setNext(e43);
	e43->setNext(e32);

	e32->setRegion(r3);
	e24->setRegion(r3);
	e43->setRegion(r3);

	//Creating region 4
	DCEL::Region* r4 = new DCEL::Region(e13);

	e13->setNext(e34);
	e34->setNext(e41);
	e41->setNext(e13);

	e13->setRegion(r4);
	e34->setRegion(r4);
	e41->setRegion(r4);

	//Including the created regions in Clist
	Clist.push_back(r1);
	Clist.push_back(r2);
	Clist.push_back(r3);
	Clist.push_back(r4);
}


//The edge e must be an horizon edge. 
//The point p is a point being added to the hull
//ep2begin is an edge already created by a previous face. When it's the first face of a polygon to be created,
//it's obvioulsy null. 
// TO TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
DCEL::Region* ConvexHull::createAFace(DCEL::Vertex* p, DCEL::HalfEdge* e, DCEL::HalfEdge* ep2Begin, DCEL::HalfEdge* ep2End)
{
	DCEL::Region* regionF = new DCEL::Region(e);
	e->setRegion(regionF);

	DCEL::HalfEdge* eEnd2p = new DCEL::HalfEdge(e);
	eEnd2p->setRegion(regionF);

	if (ep2End == nullptr)
		DCEL::HalfEdge* ep2End = new DCEL::HalfEdge(p);

	eEnd2p->setTwin(ep2End); //will be ep2begin of the next face of the polygon

	if (ep2Begin == nullptr)
		DCEL::HalfEdge* ep2Begin = new DCEL::HalfEdge(p);
	ep2Begin->setRegion(regionF);

	e->setNext(eEnd2p);
	eEnd2p->setNext(ep2Begin);
	ep2Begin->setNext(e);

	return regionF; 
}


std::vector<DCEL::Vertex*> ConvexHull::sortPointsCCw(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3)
{ 
	std::vector<DCEL::Vertex*> sortedPoints; 

	sortedPoints.push_back(p1);
	sortedPoints.push_back(p2);
	sortedPoints.push_back(p3);

	//after only one iteration, the points will be sorted in x,
	//because there's only 3 points
	for (int i = 0; i < sortedPoints.size() -1; ++i)
	{
		if (sortedPoints[i]->x > sortedPoints[i + 1]->x) 
		{
			DCEL::Vertex* temp = sortedPoints[i];
			sortedPoints[i] = sortedPoints[i + 1];
			sortedPoints[i + 1] = temp; 
		}
	}

	//if the two last points are equal in x
	//Even if the points' coordinates are float, a problem will only occur if their value are
	//perfectly equals. 
	if (sortedPoints[1]->x == sortedPoints[2]->x &&
		sortedPoints[1]->y < sortedPoints[2]->y)
	{
		DCEL::Vertex* temp = sortedPoints[1];
		sortedPoints[1] = sortedPoints[2];
		sortedPoints[2] = temp;
	}

	//if the two first points are equal in x
	if (sortedPoints[0]->x == sortedPoints[1]->x &&
		sortedPoints[0]->y > sortedPoints[1]->y)
	{
		DCEL::Vertex* temp = sortedPoints[1];
		sortedPoints[1] = sortedPoints[2];
		sortedPoints[2] = temp;
	}

	return sortedPoints; 
}

//TO TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
std::vector<DCEL::Edge*> ConvexHull::findHorizon(DCEL::Vertex* p)
{
	std::vector<DCEL::Edge*> horizon;
	for (int i = 0; i < Fconflit[p].size(); ++i)
	{
		DCEL::HalfEdge* treatenEdge = Fconflit[p][i]->getBound()->getNext();
		bool backOnFirstEdge = false; 
		while (!backOnFirstEdge)
		{
			//If the treaten edge's twin points to an invisible face, 
			//the treaten edge is on the horizon line
			if (std::find(Fconflit[p].begin(), Fconflit[p].end(), treatenEdge->getTwin()->getRegion()) == Fconflit[p].end())
				horizon.push_back(treatenEdge); 
			 
			//If the treaten edge is the first one of the face,
			//All the face's edges have been treated 
			if (treatenEdge == Fconflit[p][i]->getBound())
				backOnFirstEdge; 

			treatenEdge = treatenEdge->getNext();
		}
	}

	//Organize horizon edges
	std::vector<DCEL::Edge*> organizedHorizon;
	organizedHorizon.push_back(horizon[0]);
	while (organizedHorizon.size() != horizon.size())
	{
		int initialSize = organizedHorizon.size();
		for (std::vector<DCEL::Edge*>::iterator it = horizon.begin(); it != horizon.end(); ++it)
		{
			if ((*it)->getOrigin() == organizedHorizon.back()->getEnd())
			{
				organizedHorizon.push_back(*it);
				break;
			}
		}
		
		//if the vector hasn't increased, there's a discontinuity in the horizon line founded 
		//and everything is going to collapse, causing apocalypse in the algorithm :(
		assert(organizedHorizon.size() != initialSize); 
	}

	return organizedHorizon;
}

bool ConvexHull::computeConvexHull()
{
	if (pointList.size() >= 4)
	{
		//STEP 1
		//find 4 points not coplanar
		if (Clist.size() < 4)
		{
			auto aItt = pointList.begin();
			auto bItt = aItt+1;
			auto cItt = bItt+1;
			auto dItt = cItt+1;

			auto ab = vect(*aItt, *bItt);
			auto ac = vect(*aItt, *cItt);

			for (; dItt != pointList.end() && isCoplanar(ab, ac, vect(*aItt, *dItt)); ++dItt);
			if (dItt == pointList.end())
			{
				cout << "All points are coplanar. The convex hull can be computed with a 2D algorithm. Program is closing..." << endl;
				return false;
			}
			else
			{
				//STEP 2
				//Compute the convexHull made by the 4 points( C <- CH(p1,p2,p3,p4))
				createFirstTetraedron(&*aItt, &*bItt, &*cItt, &*dItt);

				pointList.erase(dItt);
				pointList.erase(cItt);
				pointList.erase(bItt);
				pointList.erase(aItt);
			}

			//STEP 3
			//Shuffle rests of the points
			std::random_shuffle(pointList.begin(), pointList.end());

			//STEP 4
			//Initalize the conflits Graph<aNpoint, aCface>
			initializeConflictsGraph();

			//STEP 5
			for (int r = 0; r < pointList.size(); ++r)
			{
				if (Fconflit[&pointList[r]].size() > 0)
				{
					//7. Add pr to C (done by the following steps)
					//9. Creer une liste ordonnee L formant l'horizon de pr
					std::vector<DCEL::Edge*> prHorizon = findHorizon(&pointList[r]);
					
					//10. Pour chaque arete e de L 
					for (int e = 0; e < prHorizon.size(); ++e)
					{
						//11. Relier e a pr en creant une face triangulaire f et l'ajouter a C
						DCEL::Region* regionF = nullptr;

						/// TO TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						if (e == 0)
							regionF = createAFace(&pointList[r], prHorizon[e]);
						else if (e == prHorizon.size()-1)
							regionF = createAFace(&pointList[r], prHorizon[e], prHorizon[e - 1]->getNext()->getTwin()), prHorizon[0]->getPrev();
						else
							regionF = createAFace(&pointList[r], prHorizon[e], prHorizon[e - 1]->getNext()->getTwin());

						Clist.push_back(regionF);

						//12. si f est coplanaire avec la face voisine f^t le long de e,
						
						vect vf1(*regionF->getBound()->getOrigin(), *regionF->getBound()->getEnd());
						vect vf2(*regionF->getBound()->getEnd(), *regionF->getBound()->getNext()->getEnd());
						vect normalf(vf1.cross(vf2));

						DCEL::Region* regionFT = prHorizon[e]->getTwin()->getRegion(); 
						vect vft1(*regionFT->getBound()->getOrigin(), *regionFT->getBound()->getEnd());
						vect vft2(*regionFT->getBound()->getEnd(), *regionFT->getBound()->getNext()->getEnd());
						vect normalft(vft1.cross(vft2));
						
						if (normalf == normalft)
						{
							//13.combiner f et f^t en une seule face dont la liste des conflits est la meme que pour f^t
							fuseRegions(regionF, regionFT);
						}
						else
						{
							//TO COMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							//15. creer un sommet pour f dans G
							//16. P(e) <- Pconflit(f1) U Pconflit(f2) ou f1 et f2 sont les 2 faces incidentes a e dans CH(P_r-1)
							std::vector<DCEL::Vertex*> P;

							//17.
							for (int i = 0; i < P.size(); ++i)
							{
								//18. si f est visible depuis p, ajouter (p,f) dans G
								if (faceIsVisible(P[i], regionF))
									Fconflit[P[i]].push_back(regionF);
							}
						}
					}
					
					//8. retirer les faces dans Fconflit(pr) de C
					for (int i = static_cast<int>(Fconflit[&pointList[r]].size() - 1); i >= 0; --i)
					{
						delete Fconflit[&pointList[r]][i];
						Fconflit[&pointList[r]][i] = nullptr;
					}
					Fconflit[&pointList[r]].clear();

					//19. Suprimer le sommet associe à pr de G
					Fconflit.erase(&pointList[r]);

					//TO COMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					//20. Suprimer les sommets correspondants aux faces dans Fconflit(pr) de G ainsi que leurs aretes incidentes
				}
			}
		}
	}
	else{
		cout << "Not enough points to create a convex hull, program is closing..." << endl;
        return false;
    }
    
    return true;
}

void ConvexHull::display(){
	//TODO : Using OPENGL and following the created DCEL...
}





