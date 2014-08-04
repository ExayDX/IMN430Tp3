//
//  vect.h
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-07-16.
//

#ifndef imn430_tp3_vect_h
#define imn430_tp3_vect_h

#include <iostream>
using std::istream;
using std::ostream;

#include "Vertex.h"

//template<class T>
class vect : public DCEL::Vertex/*<T>*/{
public:
    //---- Ctor
    vect(const int& x, const int& y, const int& z)
        : Vertex(x, y, z){
    }
    vect(const Vertex& a, const Vertex& b)
        : Vertex(b.x - a.x, b.y - a.y, b.z - a.z){
    }
    vect(const vect& v)
        : Vertex(v){
    }

    //---- Public methods
    vect cross(const vect& b)const{
        return vect(
            (this->y * b.z) - (this->z * b.y),
            (this->x * b.z) - (this->z * b.x),
            (this->x * b.y) - (this->y * b.x)
        );
    }
    
    int dot(const vect& v)const{
        return (this->x * v.x + this->y * v.y + this->z * v.z);
    }
    
    unsigned int norm()const{
        const int x2 = this->x * this->x;
        const int y2 = this->y * this->y;
        const int z2 = this->z * this->z;
        
        return sqrt(x2 + y2 + z2);
    }
    
    vect& normalize(){
        vect normal(getNormalizedVec());
        this->swap(normal);
        return *this;
    }
    
    vect getNormalizedVec()const{
        const unsigned int n = norm();
        return vect(this->x / n, this->y / n, this->z / n);
    }
    
    void swap(vect& v){
        std::swap(x, v.x);
        std::swap(y, v.y);
        std::swap(z, v.z);
    }

	bool operator== (const vect& v)const{
		return ((this->x - v.x < std::numeric_limits<float>::epsilon()) && 
				(this->y - v.y < std::numeric_limits<float>::epsilon()) && 
				(this->z - v.z < std::numeric_limits<float>::epsilon()));
	}
};

#endif
