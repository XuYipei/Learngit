#include <cassert>

#include <iostream>

#include "System.hpp"

class Test {
public:
	void foo() {
		std::cout << "Function 'foo' called." << std::endl;
	}

	void bar() {
		std::cout << "Function 'bar' called." << std::endl;
	}
};

int main(int argc, char *argv[]) {
	std::cout << "Begin" << std::endl;

	if (argc < 5) {
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}

	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);

	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;

	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file

	System *sys = new System(crossPath, roadPath, carPath);

	// (10000, 15, 35, 6, 1)
	// 基本操作示例
	auto car = sys->GetCar(10000);
	assert(car != nullptr);
	assert(car->system == sys);
	assert(car->id == 10000);
	assert(car->maxSpeed == 6);

	// 遍历系统内的信息示例
	for (auto &p : sys->Nodes())
		std::cout << p.id << ", "
		<< p.roadIds[0] << ", "
		<< p.roadIds[1] << ", "
		<< p.roadIds[2] << ", "
		<< p.roadIds[3] << std::endl;

	// 事件系统示例
	Test instance;
	auto foo = std::bind(&Test::foo, instance);
	auto bar = std::bind(&Test::bar, instance);
	sys->beforeUpdate.AddListener(foo);
	sys->afterUpdate.AddListener(bar);
	sys->Update();
	sys->beforeUpdate.RemoveListener(foo);
	sys->Update();

	return 0;
}