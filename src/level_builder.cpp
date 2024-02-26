#include "level_builder.hpp"
#include <iostream>


bool LevelBuilder::loadLevel(std::string name, Entity* camera)
{
	std::ifstream file(level_path(name).c_str());// *file = fopen(level_path(name).c_str(), "r");
	if (!file) {
		printf("Critical error opening level file. File: %s\n", name.c_str());
		return false;
	}

	char section[15];
	char palette[15];
	float data[2];

	file >> section;
	//std::cout << section << "\n";

	//readTuple(file, data);
	file >> data[0] >> data[1];
	*camera = createCamera(data[0], data[1]);
	//std::cout << data[0] << ", " << data[1] << "\n";

	file >> section;
	//std::cout << section << "\n";
	file >> palette;
	//std::cout << palette << "\n";
	int paletteIndex = palettes.at(palette);
	switch (paletteIndex) {
		case 0:
			createForestBackgrounds(file);
			file >> section;
			//std::cout << section << "\n";
			createForestObstacles(file);
			break;
		case 1:
			createLevel2Background(file);
			file >> section;
			createForestObstacles(file);
			break;
		default:
			std::cout << "NO SUCH PALETTE\n";
	}

	std::cout << "Loaded level: " << name << "\n";
	file.close();
	return true;
}

const void LevelBuilder::createForestBackgrounds(std::ifstream& file)
{
	createBackground(renderer, -2);
	createBackground(renderer, -1);
	createBackground(renderer, 0);
	
	//extra elements
	createLights1(renderer, { 600, 120 });
	createLights2(renderer, { 600, 120 });
	createBush(renderer, { 1200 / 2, 150 });
	createGrass(renderer, { 1200 / 2, 800 - 100 });
	createGround(renderer, { 1200 / 2, 800 - 50 });
}

const void LevelBuilder::createLevel2Background(std::ifstream& file)
{
	createBackgroundLevel2(renderer, -3);
	createBackgroundLevel2(renderer, -2);
	createBackgroundLevel2(renderer, -1);
	createBackgroundLevel2(renderer, 0);

	createGrass(renderer, { 1200 / 2, 800 - 100 });
	createGround(renderer, { 1200 / 2, 800 - 50 });
	

}
const void LevelBuilder::createForestObstacles(std::ifstream& file)
{
	float data[2];
	Entity temp = Entity();
	while (file >> data[0] >> data[1]) {
		// temp entity for storing tree reference
		// in order to let beehive have correct tree reference, please place beehive immediately after the tree it spawns on
		
		//std::cout << data[0] << ", " << data[1] << "\n";
		switch ((int) data[0]) {//what variant of obstacle is it - we should make some sort of master list or smthn
			case 1:
				temp = createTreeLeaves(renderer, data[1]);
				break;
			case 2:
				temp = createTreeBark(renderer, data[1]);
				break;
			case 3:
				createBeeHive(renderer, vec2(data[1] - 20, 440),temp);
				break;
			case 4: 
				temp = createBushLevel2(renderer, data[1]);
				break;
			case 5:
				temp = createTallTreeLevel2(renderer, data[1]);
				break;
			default:
				createTreeLeaves(renderer, data[1]);
		}

	}
}