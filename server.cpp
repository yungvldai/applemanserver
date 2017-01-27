#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <cstring>
#include <cstdlib>
#include "stdio.h"
#include "base.h"
#include "phys.h"
#include "game.h"

using namespace std;

string chr2str(char*);
void clean();
void send_state(TCPsocket,long long);
long long find_player_by_name(string);
void answer(TCPsocket,const char*);

char floating[] = 
"AppleMan-echo v.17_1/18";
string key = "a6:d3:f7:s1";

int main(int argc, char* argv[]) {

    const long req_size = 256;
    char request[req_size];
    string to_parse, instr, tmp;

    srand(time(NULL));

    system ("ls users >> users_base && echo \"*\" >> users_base");
    ifstream ub_in("users_base");
    string to_load;
    do {
        ub_in >> to_load;
        if (to_load != "*") {
            players.push_back(new player(to_load));
        }
    } while (to_load != "*");
    ub_in.close();
    system("rm users_base");

    SDL_Init (SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, 49001);
    TCPsocket server = SDLNet_TCP_Open(&ip);
    TCPsocket client;

    player *i, *j;
    bool have_i = false, have_j = false;

    while (1) {
        if(games.size() > 9999){
            games.clear();
        }
        for (int i = 0; i < games.size(); i++) {
            if(games[i].created)games[i].play();
        }
        SDL_Delay(5);
        client = SDLNet_TCP_Accept(server);
        if (client) {
            SDLNet_TCP_Recv(client, request, req_size);
            to_parse = chr2str(request);
            stringstream ios(to_parse);
            ios >> instr;
            if (instr == "floating") {
                SDLNet_TCP_Send(client, floating, strlen(floating) + 1);
            }
            if (instr == "break") {
                ios >> tmp;
                if (tmp == key) {
                    answer(client, "case::closed");
                    SDLNet_TCP_Close(client);
                    SDLNet_TCP_Close(server);
                    clean();
                    exit(0);
                }
                else {
                    answer(client, "case::wrong_key");
                }
            }
            if (instr == "game") {
                long long game_id;
                ios >> game_id;
                send_state(client, game_id);
            }
            if (instr == "login") {
                string user, password;
                string check_pass, check_login;
                ios >> user >> password;
                ifstream fin(("users/" + user + "/profile").c_str());
                if (!fin.is_open()) {
                    answer(client, "case::login_failed");
                } else {
                    fin >> check_login >> check_pass;
                    if (check_login == user && check_pass == password) {
                        answer(client, "case::login_success");
                    } else {
                        answer(client, "case::login_failed");
                    }
                    fin.close();
                }
            }
            if (instr == "abort"){
                string player_name, password;
                ios >> player_name >> password;
                if(i->name == player_name && i->password == password){
                    have_i = false;
                    answer(client,"case::abort_ok");
                }
                else answer(client,"case::wrong_password");
            }
            if (instr == "search") {
                string player_name, password;
                ios >> player_name >> password;
                long long player_id_ll = find_player_by_name(player_name);
                bool game_created = false;
                if (player_id_ll == -1) {
                    answer(client, "case::id_not_found");
                } else {
                    if (password == players[player_id_ll]->password) {
                        if (have_i && !have_j && players[player_id_ll]->name != i->name) {
                            j = players[player_id_ll];
                            have_i = false;
                            have_j = false;
                            games.push_back(game(i, j));
                            char buf[16];
                            sprintf(buf, "case::game_started %d",
                                    games[games.size() - 1].game_id);
                            SDLNet_TCP_Send(client, buf, strlen(buf) + 1);
                            game_created = true;
                        }
                        if (!have_i && !have_j && !game_created) {
                            i = players[player_id_ll];
                            have_i = true;
                            char buf[16];
                            sprintf(buf, "case::waiting_for_game %d",
                                    global_id);
                            SDLNet_TCP_Send(client, buf, strlen(buf) + 1);
                        }
                    } else {
                        answer(client, "case::wrong_password");
                    }
                }
            }
            if (instr == "signup") {
                string user, password;
                ios >> user >> password;
                ifstream fin(("users/" + user + "/profile").c_str());
                if (!fin.is_open()) {
                    system(("mkdir users/" + user).c_str());
                    ofstream fout(("users/" + user + "/profile").c_str());
                    fout << user << " " << password << " 0";
                    fout.close();
                    answer(client, "case::signup_success");
                    players.push_back(new player(user));
                } else {
                    answer(client, "case::signup_failed");
                    fin.close();
                }
            }
            if (instr == "move") {
                ios >> instr;
                string player_name, password;
                long long game_id;
                ios >> game_id >> player_name >> password;
                long long player_id_ll = find_player_by_name(player_name);
                if (player_id_ll == -1) {
                    answer(client, "case::id_not_found");
                } else {
                    if (password == players[player_id_ll]->password) {
                        if (instr == "left") {
                            players[player_id_ll]->speed_x = -speed;
                            players[player_id_ll]->direction = "left";
                        }
                        if (instr == "right") {
                            players[player_id_ll]->speed_x = speed;
                            players[player_id_ll]->direction = "right";
                        }
                        if (instr == "up") {
                            players[player_id_ll]->speed_y = -speed;
                            players[player_id_ll]->direction = "up";
                        }
                        if (instr == "down") {
                            players[player_id_ll]->speed_y = speed;
                            players[player_id_ll]->direction = "down";
                        }
                        answer(client, "case::move_ok");
                    } else {
                        answer(client, "case::wrong_password");
                    }
                }
            }
        }
        SDLNet_TCP_Close(client);
    }
    SDLNet_TCP_Close(server);
    clean();
    
    return 0;
}

void clean() {
    for (int i = 0; i < players.size(); i++) {
        delete players[i];
    }
    players.clear();
    games.clear();
    SDLNet_Quit();
    SDL_Quit();
}
string chr2str(char* a){
    string returnable = "";
    for (int i = 0; i < strlen(a); i++) {
        returnable += a[i];
    }
    return returnable;
}

void send_state(TCPsocket socket,long long game_id){
    if(game_id < global_id && game_id != 0){
        SDLNet_TCP_Send(socket, games[game_id - 1].state().c_str(),games[game_id - 1].state().length() + 1);
    }
    else{
        answer(socket,"case::id_not_found");
    }
}

long long find_player_by_name(string name){
    for(int i = 0;i < players.size();i++){
        if(players[i]->name == name){
            return i;
        }
    }
    return -1;
}

void answer(TCPsocket socket,const char* exc){
    SDLNet_TCP_Send(socket, exc,strlen(exc) + 1);
}
