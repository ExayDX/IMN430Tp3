//
//  Region.h
//  IMN430_tp3
//
//  Created by Steven Aubertin on 2014-05-13.
//

#ifndef IMN430_tp1_Region_h
#define IMN430_tp1_Region_h

#include "EdgeIterator.h"
#include "Vertex.h"
#include <memory>
    using std::shared_ptr;


namespace DCEL {
    /*
        Region or face in a DCEL
     
        Traversing a Region
        for a giving edge e
     
        start = e
        while next(e) != start
            e = next(e)
     */
	class Region{
	public:
		//---- Constructors
		Region(DCEL::Vertex aSite, HalfEdge* obound = nullptr) :
			bound(obound),
			site(aSite){
		}

		Region(HalfEdge* obound = nullptr) :
			bound(obound){
		}

		//---- Accessors
		inline HalfEdge* getBound() const{
			return bound;
		}

		//---- Mutators
		inline void setBound(HalfEdge* bound){
			assert(bound);
			this->bound = bound;
		}

		inline DCEL::Vertex getSite() const{
			return site; 
		}

		inline void setSite(DCEL::Vertex aSite){
			site = aSite; 
		}
        
        //---- Iterators
    private:
        class RegionIterator : public EdgeIterator{
        public:
            //---- Constructors
            RegionIterator(const Region* region)
                : EdgeIterator(region->getBound()){
            }
            
            //---- Overriden Methods
            inline Edge* getNext(){
                Edge* n = this->next;
                
                //--- If not the end we go foward
                if(this->hasNext()){
                    this->next = this->next->getNext();
                }
                
                //--- If we are at the begining then it's the end
                if(this->isStart(this->next)){
                    this->next = nullptr;
                }
                
                return n;
            }
        };
    public:
        typedef RegionIterator iterator_type;
        iterator_type begin(){
            return iterator_type(this);
        }

        iterator_type end(){
            return iterator_type(nullptr);
        }

		void draw(){
			HalfEdge* edgeToDraw = bound;
			//TODO : construct the second condition saying if the end of an edge is the infinity. 
			while((edgeToDraw->getEnd() != bound->getOrigin())  /*&& (edgeToDraw->getEnd != BOUNDINGBOX)*/)
				edgeToDraw->draw();
				edgeToDraw = edgeToDraw->getNext(); 
		}; 

		//--Comparators
		struct CompareRegionY : public std::binary_function<Region*, Region*, bool>{
			bool operator()(const Region* r1, const Region* r2){
				if (r1->getSite().y != r2->getSite().y)
					return r1->getSite().y > r2->getSite().y;
				else
					return r1->getSite().x < r2->getSite().x;
			}
		};
        
    private:
        //---- Members
        HalfEdge* bound;
		DCEL::Vertex site; 
        
        //TODO: If we need to add hole to a region it will be here
        //std::List<HalfEdge*> holes; ou quelque chose comme ca
    };
}

#endif
