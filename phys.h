#pragma once

#include "cmath"

#include "player.h"
#include "item.h"

#define near 75
#define speed 7

namespace phys{
	bool is_near(player a,player b){
		if(sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)) <= near){
			return true;
		}
		return false;
	}
	bool is_near(item a,player b){
		if(sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)) <= near){
			return true;
		}
		return false;
	}
	bool is_near(player a,item b){
		if(sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)) <= near){
			return true;
		}
		return false;
	}
	bool is_near(item a,item b){
		if(sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)) <= near){
			return true;
		}
		return false;
	}
	
}