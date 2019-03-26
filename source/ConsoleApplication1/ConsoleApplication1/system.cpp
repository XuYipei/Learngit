// NOTICE: 模拟器主循环在 System::Update

#include <cassert>
#include <cstdio>

#include <fstream>

#include "System.hpp"

// Fix Visual Studio `sscanf_s` issue
#ifdef _WIN32
#define sscanf sscanf_s
#endif

Object::Object(int _id) : id(_id) {}

Object::~Object() {}

Road::Road(int _id, int _length, int _maxSpeed, int _laneCount, int _startId,
	int _endId, bool _isDuplex)
	: Object(_id), length(_length), maxSpeed(_maxSpeed), laneCount(_laneCount),
	startId(_startId), endId(_endId), isDuplex(_isDuplex) {}

Road::~Road() {}

void Road::Init() {
	startNode = system->GetNode(startId);
	endNode = system->GetNode(endId);
	cars.resize(isDuplex ? 2 * laneCount : laneCount);
	for (auto &lane : cars)
		lane.resize(length);
}

void Road::Reset() {
	for (int i = 0; i < GetLaneCount(); i++) for (auto &e : cars[i])
		e = nullptr;
}

auto Road::operator[](const size_t i) -> std::vector<Car*>& {
	return cars[i];
}

auto Road::GetLaneCount() -> int {
	return isDuplex ? laneCount * 2 : laneCount;
}

auto Road::GetMaxCapacity() -> int {
	return laneCount * length;
}

auto Road::GetCarCount(RoadDirection dir) -> int {
	assert(!(!isDuplex && dir == END_TO_START));
	int beg = dir == START_TO_END ? 0 : laneCount;
	int end = beg + laneCount, ret = 0;
	for (int i = beg; i < end; i++) for (auto p : cars[i])
		if (p) ret++;
	return ret;
}

auto Road::GetCapacity(RoadDirection dir) -> int {
	return GetMaxCapacity() - GetCarCount(dir);
}

auto Road::GetEntranceCount(RoadDirection dir) -> int {
	assert(!(!isDuplex && dir == END_TO_START));
	int beg = dir == START_TO_END ? 0 : laneCount;
	int end = beg + laneCount, ret = 0;
	for (int i = beg; i < end; i++) for (auto p : cars[i])
		if (!p) ret++;
		else break;
	return ret;
}

auto Road::GetEntrance(RoadDirection dir) -> int {
	assert(!(!isDuplex && dir == END_TO_START));
	int beg = dir == START_TO_END ? 0 : laneCount;
	int end = beg + laneCount;
	for (int i = beg; i < end; i++)
		if (!cars[i][0]) return i - beg + 1;
	return 0;
}

auto Road::GetLane(RoadDirection dir, int laneId) -> std::vector<Car*>& {
	return cars[(dir == END_TO_START ? laneCount : 0) + laneId - 1];
}

auto Road::GetCar(RoadDirection dir, int laneId, int pos) -> Car* {
	return GetLane(dir, laneId)[pos];
}

auto Road::GetDirection(Node *entrance) -> RoadDirection {
	return entrance == startNode ? START_TO_END : END_TO_START;
}

auto Road::CanGoThrough(Node *entrance) -> bool {
	return isDuplex || GetDirection(entrance) == START_TO_END;
}

auto Road::GetNextNode(Node *entrance) -> Node* {
	return GetDirection(entrance) == START_TO_END ? endNode : startNode;
}

Node::Node(int _id, int r1, int r2, int r3, int r4)
	: Object(_id), roadIds{ r1, r2, r3, r4 } {}

Node::~Node() {}

void Node::Init() {
	for (int i = 0; i < MAX_ROADS_PER_NODE; i++)
		if (roadIds[i] != -1) roads[i] = system->GetRoad(roadIds[i]);
		else roads[i] = nullptr;
}

void Node::Reset() {}

auto Node::operator[](const size_t i) -> Road* {
	return roads[i];
}

auto Path::operator[](const size_t i) -> Road* {
	return roads[i];
}

auto Path::GetSize() -> int {
	return roads.size();
}

auto Path::GetCurrentRoad() -> Road* {
	return roads[0];
}

auto Path::GetNextRoad() -> Road* {
	return roads.size() > 1 ? roads[1] : nullptr;
}

auto Path::GetPrevNode() -> Node* {
	Road &cur = *GetCurrentRoad(), &nxt = *GetNextRoad();
	if (cur.startId == nxt.startId || cur.startId == nxt.endId)
		return cur.endNode;
	return cur.startNode;
}

auto Path::GetNextNode() -> Node* {
	Road &cur = *GetCurrentRoad(), &nxt = *GetNextRoad();
	if (cur.startId == nxt.startId || cur.startId == nxt.endId)
		return cur.startNode;
	return cur.endNode;
}

auto Path::GetCurrentDirection() -> RoadDirection {
	return GetPrevNode() == GetCurrentRoad()->startNode ? START_TO_END : END_TO_START;
}

auto Path::GetNextDirection() -> RoadDirection {
	return GetNextNode() == GetNextRoad()->startNode ? START_TO_END : END_TO_START;
}

auto Path::GetTurning() -> Turning {
	auto cur = GetCurrentRoad(), nxt = GetNextRoad();
	Node &cross = *GetNextNode();
	for (int i = 0; i < MAX_ROADS_PER_NODE; i++) if (cross[i] == cur) {
		if (cross[(i + 1) % 4] == nxt) return LEFT;
		if (cross[(i + 3) % 4] == nxt) return RIGHT;
		return FORWARD;
	}
	return UNKNOWN;
}

void Path::GoForward() {
	walked.push_back(roads[0]);
	roads.pop_front();
}

Car::Car(int _id, int _maxSpeed, int _minStartTime)
	: Object(_id), maxSpeed(_maxSpeed), minStartTime(_minStartTime) {
	startTime = endTime = 0;
}

Car::~Car() {
	if (path) delete path;
}

void Car::Init() {}

void Car::Reset() {
	path->roads.clear();
	path->walked.clear();
	if (path) delete path;
	path = nullptr;
	startTime = endTime = 0;
}

auto Car::GetNextNode() -> Node* {
	auto &cur = *GetCurrentRoad();
	return direction == START_TO_END ? cur.endNode : cur.startNode;
}

auto Car::GetPrevNode() -> Node* {
	auto &cur = *GetCurrentRoad();
	return direction == START_TO_END ? cur.startNode : cur.endNode;
}

auto Car::GetCurrentRoad() -> Road* {
	return path->GetCurrentRoad();
}

auto Car::GetNextRoad() -> Road* {
	return path->GetNextRoad();
}

auto Car::GetNextCar() -> Car* {
	auto &cur = *GetCurrentRoad();
	auto &lane = cur.GetLane(direction, laneId);
	for (int p = position + 1; p < cur.length; p++)
		if (lane[p]) return lane[p];
	return nullptr;
}

auto Car::GetPrevCar() -> Car* {
	auto &lane = GetCurrentRoad()->GetLane(direction, laneId);
	for (int p = position - 1; p >= 0; p--)
		if (lane[p]) return lane[p];
	return nullptr;
}

auto Car::GetLeftCar() -> Car* {
	auto &cur = *GetCurrentRoad();
	for (int i = laneId - 1; i >= 1; i--) {
		auto p = cur.GetCar(direction, i, position);
		if (p) return p;
	}
	return nullptr;
}

auto Car::GetRightCar() -> Car* {
	auto &cur = *GetCurrentRoad();
	for (int i = laneId + 1; i <= cur.laneCount; i++) {
		auto p = cur.GetCar(direction, i, position);
		if (p) return p;
	}
	return nullptr;
}

auto Car::GetNextDirection() -> RoadDirection {
	return path->GetNextDirection();
}

auto Car::GetRemainLength() -> int {
	return GetCurrentRoad()->length - position - 1;
}

auto Car::GetNextEntrance() -> int {
	return GetNextRoad()->GetEntrance(path->GetNextDirection());
}

void Car::GoForward(int step) {
	auto &lane = GetCurrentRoad()->GetLane(direction, laneId);
	lane[position] = nullptr;
	position += step;
	nowspeed -= step;
	assert(lane[position] == nullptr);
	lane[position] = this;
}

void Car::GotoNextRoad(int newLane, int step) {
	GetCurrentRoad()->GetLane(direction, laneId)[position] = nullptr;
	auto &nxt = *GetNextRoad();
	laneId = newLane;
	position = step - 1;
	nowspeed -= step;
	direction = GetNextDirection();
	assert(nxt.GetLane(direction, laneId)[position] == nullptr);
	nxt.GetLane(direction, laneId)[position] = this;
	path->GoForward();
}

void Car::GotoNextRoad(int step) {
	GotoNextRoad(GetNextRoad()->GetEntrance(GetNextDirection()), step);
}

auto Car::GetTimeUsed() -> int {
	return endTime - startTime;
}

Request::Request(int _startId, int _endId, Car *_car)
	: startId(_startId), endId(_endId), car(_car) {}

System::System(const std::string &nodesData,
	const std::string &roadsData,
	const std::string &carsData) {
	ReadNodesFromFile(nodesData);
	ReadRoadsFromFile(roadsData);
	ReadCarsFromFile(carsData);

	for (auto &e : _nodes) {
		e.second->system = this;
		e.second->Init();
	}
	for (auto &e : _roads) {
		e.second->system = this;
		e.second->Init();
	}
	for (auto &e : _cars) {
		e.second->system = this;
		e.second->Init();
	}
	for (auto e : _reqs) {
		e->startNode = GetNode(e->startId);
		e->endNode = GetNode(e->endId);
	}
}

System::~System() {
	for (auto &e : _nodes) delete e.second;
	for (auto &e : _roads) delete e.second;
	for (auto &e : _cars) delete e.second;
	for (auto e : _reqs) delete e;
}

auto System::operator[](int id) -> Object* {
	if (_nodes.count(id)) return _nodes[id];
	if (_roads.count(id)) return _roads[id];
	if (_cars.count(id)) return _cars[id];
	return nullptr;
}

auto System::GetNode(int id) -> Node* {
	return _nodes.count(id) ? _nodes[id] : nullptr;
}

auto System::GetRoad(int id) -> Road* {
	return _roads.count(id) ? _roads[id] : nullptr;
}

auto System::GetCar(int id) -> Car* {
	return _cars.count(id) ? _cars[id] : nullptr;
}

void System::ReadNodesFromFile(const std::string &path) {
	std::ifstream file(path);
	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		if (line.size() == 0 || line[0] == '#') continue;

		int id, r1, r2, r3, r4;
		sscanf(line.data(), "(%d,%d,%d,%d,%d)", &id, &r1, &r2, &r3, &r4);
		_nodes[id] = new Node(id, r1, r2, r3, r4);
	}
	file.close();
}

void System::ReadRoadsFromFile(const std::string &path) {
	std::ifstream file(path);
	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		if (line.size() == 0 || line[0] == '#') continue;

		int id, length, maxSpeed, laneCount, startId, endId, isDuplex;
		sscanf(line.data(), "(%d,%d,%d,%d,%d,%d,%d)",
			&id, &length, &maxSpeed, &laneCount, &startId, &endId, &isDuplex);
		_roads[id] = new Road(id, length, maxSpeed, laneCount, startId, endId, isDuplex);
	}
	file.close();
}

void System::ReadCarsFromFile(const std::string &path) {
	std::ifstream file(path);
	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		if (line.size() == 0 || line[0] == '#') continue;

		int id, startId, endId, maxSpeed, minStartTime;
		sscanf(line.data(), "(%d,%d,%d,%d,%d)",
			&id, &startId, &endId, &maxSpeed, &minStartTime);
		auto car = new Car(id, maxSpeed, minStartTime);
		_cars[id] = car;
		_reqs.push_back(new Request(startId, endId, car));
	}
	file.close();
}

void System::WriteAnswers(const std::string &path) {
	std::ofstream file(path);
	for (auto &car : Cars()) {
		file << '(' << car.id << ", " << car.startTime;
		for (auto r : car.path->walked)
			file << ", " << r->id;
		file << ")\n";
	}
	file.close();
}

auto System::Nodes() -> MapWrapper<Node> {
	return _nodes;
}

auto System::Roads() -> MapWrapper<Road> {
	return _roads;
}

auto System::Cars() -> MapWrapper<Car> {
	return _cars;
}

int min(int x, int y) { if (x > y)return(y); return(x); }
void System::Update() {
	// NOTICE: 在必要的时候需触发对应事件
	beforeUpdate.Notify();

	// TODO: 模拟器更新过程：移动所有车辆
	while (1) {
		bool go = 0;

		for (auto &p : Roads()) {
			for (int i = 0; i <= p.length; i++)
				for (int j = 0; j < p.laneCount; j++) {
					for (auto c_ : p.cars[j]) {
						Car c = *c_;
						if (c.GetRemainLength() == 0 && c.GetNextRoad() == nullptr) continue;

						if (c.nowspeed + c.position <= p.length||c.GetNextRoad()==nullptr) {
							auto pr = c.GetPrevCar();
							int gf = min(c.GetRemainLength(),c.nowspeed);
							if (pr != nullptr) gf = min(gf, pr->position - c.position);
							c.GoForward(gf);
							continue;
						}

						
						int fr = -1, lne = 0;
						auto nxr = *(c.GetNextRoad());
						for (; lne < nxr.GetLaneCount(); lne++) {
							int mn = nxr.length + 1;
							for (auto cc_ : nxr.cars[lne]) 
								if ((*cc_).position < mn) mn = (*cc_).position;
							if (mn != 0) {fr = mn; break;}
						}
						if (lne >= nxr.GetLaneCount()) {
							c.GoForward(c.GetRemainLength());
							continue;
						}

						fr--;
						if (c.direction == FORWARD) {
							c.GoForward(c.GetRemainLength());
							c.nowspeed += min((*(c.GetNextRoad())).maxSpeed, c.maxSpeed) - min(p.maxSpeed, c.maxSpeed);
							c.GotoNextRoad(lne,min(fr,c.nowspeed));
						}


						int id = 0;
						auto nxn = *(c.GetNextNode());
						for (id = 0; id < 3; id++) if (nxn.roadIds[id] == p.id) break;

						if (c.direction == LEFT) {
							bool check = 1;
							Road r = *(nxn.roads[(id + 3) % 4]);
							for (auto lane : r.cars) {
								for (auto c_ : lane) {
									Car c__ = *c_;
									if (c__.direction == FORWARD && c__.GetRemainLength() < c__.nowspeed)
										check = 0;
								}
							}
							if (!check) continue;
							c.GoForward(c.GetRemainLength());
							c.nowspeed += min((*(c.GetNextRoad())).maxSpeed, c.maxSpeed) - min(p.maxSpeed, c.maxSpeed);
							c.GotoNextRoad(lne, min(fr,c.nowspeed));
						}
						if (c.direction == RIGHT) {
							bool check = 1;
							Road r = *(nxn.roads[(id + 1) % 4]);
							for (auto lane : r.cars) {
								for (auto c_ : lane) {
									Car c__ = *c_;
									if (c__.direction == FORWARD && c__.GetRemainLength() < c__.nowspeed)
										check = 0;
								}
							}
							r = *(nxn.roads[(id + 2) % 4]);
							for (auto lane : r.cars) {
								for (auto c_ : lane) {
									Car c__ = *c_;
									if (c__.direction == FORWARD && c__.GetRemainLength() < c__.nowspeed)
										check = 0;
								}
							}
							if (!check) continue;
							c.GoForward(c.GetRemainLength());
							c.nowspeed += c.nowspeed + min((*(c.GetNextRoad())).maxSpeed, c.maxSpeed) - min(p.maxSpeed, c.maxSpeed);
							c.GotoNextRoad(lne, min(fr,c.nowspeed));
						}
					}
				}
		}

		/*
			遍历道路
			s型遍历车:{

			}
			车能不能过路口 GetRemainSpeed()<Getnowspeed()
			车能不能开:
				直行
				左转:右边道路有没有直行的要过路口的车
				右转:左边对面道路有没有到右边道路的要过路口的车
			车GotoNextRoad(GetRemainSpeed()+Getnewspeed()-Getnowspeed())
		*/

		if (!go) break;
	}

	onNoUpdates.Notify();

	// TODO: 模拟器更新过程：加入新的车辆

	_time++;
//	afterUpdate.Notify();
}//1s内的开车

void System::RequestForAllCars(bool copyToWalked) {
	for (auto req : _reqs) {
		onNewRequest.Notify(req);
		if (copyToWalked) {
			auto p = req->car->path;
			for (auto r : p->roads) p->walked.push_back(r);
		}
	}
}

void System::Reset() {
	_time = 0;
	for (auto &e : _nodes) e.second->Reset();
	for (auto &e : _roads) e.second->Reset();
	for (auto &e : _cars) e.second->Reset();
}

auto System::Started() -> bool {
	return(1);
	// TODO:
}

auto System::Stopped() -> bool {
	return(1);
	// TODO: 判断系统是否已经停止调度
}

auto System::Finished() -> bool {
	// TODO：判断系统是否结束
	for (auto &e : Cars())
		if (e.endTime == 0)
			return false;
	return true;
}

auto System::GetCurrentTime() -> int {
	return _time;
}