#pragma once

#include "iostream"
#include "sstream"
#include "fstream"
#include "cmath"
#include "phys.h"
#include "stdio.h"

long long global_id = 1;

std::string tmp;
long pts;

class game{
	public:
		game(player *_a,player *_b){
			game_id = global_id;
			a = _a;
			b = _b;
			global_id++;
			reset();
			created = true;
		}

		void reset(){
			a->points = 0;
			b->points = 0;
			a->x = 200;
			a->y = 200;
			b->x = 1080;
			b->y = 520;
			a->direction = "stay";
			b->direction = "stay";
			apple_move();
		}

		void play(){
			if(created){
				a->stop();
				b->stop();

				a->x += a->speed_x;
				a->y += a->speed_y;
				b->x += b->speed_x;
				b->y += b->speed_y;

				if(phys::is_near(*a,apple)){
					a->points++;
					apple_move();
				}
				if(phys::is_near(*b,apple)){
					b->points++;
					apple_move();
				}
				if(phys::is_near(*a,*b)){
					if(a->x >= b->x){
						a->x += 70;
						b->x -= 70;
					}
					else {
						a->x -= 70;
						b->x += 70;
					}
				}
				if(a->points >= 25){
					std::ofstream fout(("users/" + a->name + "/profile"));
					std::ifstream fin(("users/" + a->name + "/profile"));
					fin >> tmp;
					fin >> tmp;
					fin >> pts;
					pts++;
					fout << a->name << " " << a->password << " " << pts;
					fout.close();
					fin.close();
					created = false;
				}
				if(b->points >= 25){
					std::ofstream fout(("users/" + b->name + "/profile"));
					std::ifstream fin(("users/" + b->name + "/profile"));
					fin >> tmp;
					fin >> tmp;
					fin >> pts;
					pts++;
					fout << b->name << " " << b->password << " " << pts;
					fout.close();
					fin.close();
					created = false;
				}
			}
		}

		std::string state(){
			if(created)
			{
				std::string ret;
				std::stringstream ios;
				ios << "case::game " 
					<< a->name << " " << a->x << " " << a->y << " " << a->direction << " " << a->points << " "
			    	<< b->name << " " << b->x << " " << b->y << " " << b->direction << " " << b->points << " "
			    	<< apple.x << " " << apple.y;
				getline(ios,ret);
				return ret;
			}
			return "case::game_closed";
		}

		void apple_move(){
			apple.x = 50 + rand() % 1180;
			apple.y = 50 + rand() % 620;
		}

		bool created;
		long long game_id;
		player *a,*b;
		item apple;
};