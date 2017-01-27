#pragma once

#include "iostream"
#include "fstream"

class player{
	public:
		player(std::string _name){
			name = _name;
			load_full_info();
			speed_x = 0;
			speed_y = 0;
		}

		void load_full_info(){
			std::ifstream fin("users/" + name + "/profile");
			fin >> name >> password >> wins;
			fin.close();
		}

		void stop(){
			if(speed_x > 0)speed_x--;
			if(speed_x < 0)speed_x++;
			if(speed_y > 0)speed_y--;
			if(speed_y < 0)speed_y++;
		}


		long wins;
		int x,y,points,speed_x,speed_y;
		std::string direction;

		std::string name,password;
};
