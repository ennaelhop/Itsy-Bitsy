#include "world_system.hpp"
#include "world_init.hpp"
#include "health_system.hpp"
#include "level_builder.hpp"


#include "load.hpp"
#include <iostream>
#include <fstream>
using namespace std;

//int size = 0;

void Load::save_load(vec2 player_position, float player_health, float treeHealth[], float treePosX[], float treePosY[],
                     std::string level) {
//    printf("pos x:  %f\n", player_position.x);
//    printf("pos y:  %f\n", player_position.y);
//    printf("health player :  %f\n", player_health);
    ofstream MyFile("saved.txt");
    if(MyFile.good()) {
        MyFile.write((char *) &player_position.x, sizeof(float));
        MyFile.write((char *) &player_position.y, sizeof(float));
        MyFile.write((char *) &player_health, sizeof(float));
        for (int i = 0; i < 20; i++) {
            if (((int) treeHealth[i] ) == 0) {
                size = i;
                break;
            } else {
                MyFile.write((char *) &treeHealth[i], sizeof(float));
                MyFile.write((char *) &treePosX[i], sizeof(float));
                MyFile.write((char *) &treePosY[i], sizeof(float));
//                printf("tree health  %f\n", treeHealth[i]);
//                printf("x pos tree  %f\n", treePosX[i]);
//                printf("y pos tree  %f\n", treePosY[i]);
            }
        }
        MyFile.write(level.c_str(), sizeof(char)*level.size());
        MyFile.close();
        createTEXT(TEXT_ASSETS::SAVED, {player_position.x, player_position.y - 100}, 30, 300);
    }
}


bool Load::load_save(std::string name, Entity* camera, RenderSystem* renderer) {
    this->renderer = renderer;
    int length;
    int newLength;
    if (size == 0){
        length = 20;
    }else{
        length = size;
    }
    float player_x, player_y, player_h= 0.0;
    //float treeH[length];
    //float treeX[length];
    //float treeY[length];
    std::vector<float>treeH(length);
    std::vector<float>treeX(length);
    std::vector<float>treeY(length);
    std::string level;
    ifstream in("saved.txt", std::ios_base::binary);
    if (in.good()) {
        in.read((char *) &player_x, sizeof(float));
        in.read((char *) &player_y, sizeof(float));
        in.read((char *) &player_h, sizeof(float));
        spider_pos = {player_x, player_y};
        spider_health = (int) player_h;
//        std::cout << "Reading floating point number: " << std::fixed << player_x << std::endl;
//        std::cout << "Reading floating point number: " << std::fixed << player_y << std::endl;
//        std::cout << "Reading floating point number: " << std::fixed << player_h << std::endl;

        for (int i = 0; i < length; ++i) {
            in.read((char *) &treeH[i], sizeof(float));
            in.read((char *) &treeX[i], sizeof(float));
            in.read((char *) &treeY[i], sizeof(float));
            if (!(( treeH[i] <= 80.f) && (0.f < treeH[i]))) {
                newLength = i;
                break;
            }
            if (size != 0){
                newLength = size;
            }
//            std::cout << "health: " << std::fixed << treeH[i] << std::endl;
//            std::cout << "X pos: " << std::fixed << treeX[i] << std::endl;
//            std::cout << "Y pos: " << std::fixed << treeY[i] << std::endl;
        }
        in.read((char *) &health_bar_pos, sizeof(float));
        std::getline(in, level);
//        std::cout << "level:" << level << std::endl;
        in.close();
    }


    std::ifstream file(level_path(name).c_str());// *file = fopen(level_path(name).c_str(), "r");
    if (!file) {
        printf("Critical error opening level file. File: %s\n", name.c_str());
        return false;
    }
    char section[15];
    char palette[15];
    float data[2];
    file >> section;
    file >> data[0] >> data[1];
    *camera = createCamera(data[0], data[1]);
    file >> section;
    file >> palette;
    int paletteIndex = palettes.at(palette);
    switch (paletteIndex) {
        case 0:
            createForestBackgrounds(file);
            file >> section;
            break;
        default:
            std::cout << "NO SUCH PALETTE\n";
    }
    for (int i = 0; i < newLength; ++i) {
        Entity tree = createTreeBark(renderer, treeX[i]);
        registry.damageable.get(tree).health = treeH[i];
    }
    std::cout << "Loaded Save: " << name << "\n";
    file.close();
    return true;
}

const void Load::createForestBackgrounds(std::ifstream& file)
{
    createBackground(renderer, -2);
    createBackground(renderer, -1);
    createBackground(renderer, 0);
    createLights1(renderer, { 600, 120 });
    createLights2(renderer, { 600, 120 });
    createBush(renderer, { 1200 / 2, 150 });
    createGrass(renderer, { 1200 / 2, 800 - 100 });
    createGround(renderer, { 1200 / 2, 800 - 50 });
}

int Load::get_health_saved(){
    return spider_health;
}

vec2 Load::get_position_saved(){
    return spider_pos;
}