#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <deque>
#include <vector>
#include <string>
#include <unordered_map>

#include "Event.hpp"
#include "MapWrapper.hpp"

// 前置声明
class Object;
class Road;
class Node;
class Path;
class Car;
class Request;
class System;

/**
 * @brief 每个路口最多有 4 条道路
 */
constexpr int MAX_ROADS_PER_NODE = 4;

/**
 * @brief 数据中可能的最大 ID
 *
 */
constexpr int MAX_ID = 50000;

/**
 * @brief 表示双行道的方向
 *
 * 两个枚举分别表示从道路输入的起点到终点、终点到起点
 */
enum RoadDirection {
	START_TO_END, END_TO_START
};

/**
 * @brief 表示转弯的方向
 *
 * RIGHT、LEFT、FORWARD 分别表示右拐、左拐和直行，数值递增
 */
enum Turning {
	UNKNOWN = 0, RIGHT, LEFT, FORWARD
};

/**
 * @brief 表示一个物体
 */
class Object {
public:
	/**
	 * @brief 禁用默认构造函数，要求必须提供 id
	 */
	Object() = delete;

	/**
	 * @brief 构造函数
	 *
	 * @param id 该物体的全局唯一 id
	 */
	Object(int _id);

	virtual ~Object();

	/// 表示物体的 id
	int id;
	/// 表示物体所在的交通系统
	System *system;

	/**
	 * @brief 初始化
	 *
	 * @remark 因为物体在构造时系统还没有准备好，因此该函数是在系统准备好后
	 *         统一调用的。该函数只会在 System 初始化时调用一次
	 */
	virtual void Init() = 0;

	/**
	 * @brief 重置状态
	 *
	 * @remark 提供给 System::Reset 调用
	 */
	virtual void Reset() = 0;
};

/**
 * @brief 表示一条道路
 */
class Road : public Object {
public:
	/**
	 * @brief 构造函数
	 *
	 * @params 对应同名的成员变量
	 */
	Road(int _id, int _length, int _maxSpeed, int _laneCount, int _startId,
		int _endId, bool _isDuplex);

	~Road();

	/// 道路长度
	int length;
	/// 路上的最大限速
	int maxSpeed;
	/// 单向车道数量。如果是双行线，则两边的车道数量一样
	int laneCount;
	/// 道路的起始路口的 id
	int startId;
	/// 道路的终止路口的 id
	int endId;
	/// 是否为双行线
	bool isDuplex;

	/// 道路的起始路口
	Node *startNode;
	/// 道路的终止路口
	Node *endNode;
	/// 道路上每个位置的车辆信息
	/// 如果某个位置上没有车辆，则为 nullptr
	std::vector<std::vector<Car*>> cars;

	void Init();
	void Reset();

	/**
	 * @brief 访问某一条车道
	 *
	 * @param i 车道的下标
	 * @return 返回该车道从后往前的所有位置上的车辆信息，长度为 length。
	 *         0 是刚进入道路的车辆，下标越大在该车道上行驶距离越远
	 * @remark 车道下标不是车道编号
	 */
	auto operator[](const size_t i)->std::vector<Car*>&;

	/**
	 * @brief 获取总车道数量
	 *
	 * @return 总车道数量。如果是双行线，则为 laneCount 的两倍
	 */
	auto GetLaneCount() -> int;

	/**
	 * @brief 获取单向车总容量
	 *
	 * @return 返回能容纳的车辆总数。注意只有一个方向
	 */
	auto GetMaxCapacity() -> int;

	/**
	 * @brief 获取单向上当前车辆总数
	 *
	 * @param dir 指定方向
	 * @return 该方向上的车辆总数
	 */
	auto GetCarCount(RoadDirection dir) -> int;

	/**
	 * @brief 获取单向上剩余位置数量
	 *
	 * @param dir 指定方向
	 * @return 返回该方向上空位数量
	 */
	auto GetCapacity(RoadDirection dir) -> int;

	/**
	 * @brief 获取从某个方向进入道路时可用车位数量
	 *
	 * 对每条道路，找到这个方向上进入遇到的第一辆车，这辆车后面的位置都是可用的。
	 * 该函数返回某个方向上每条道路的可用车位值的总和
	 *
	 * @param dir 指定方向
	 * @return 返回可进入的位置总数
	 */
	auto GetEntranceCount(RoadDirection dir) -> int;

	/**
	 * @brief 取得入口
	 *
	 * @param dir 指定进入的方向
	 * @return 如果可以进入道路，则返回将要进入的车道编号。否则返回 0
	 */
	auto GetEntrance(RoadDirection dir) -> int;

	/**
	 * @brief 获取某个方向上特定编号的车道数据
	 *
	 * @param dir 指定方向
	 * @param laneId 车道编号
	 * @return 车道上所有位置的车辆数据
	 * @remark 车道编号从 1 开始，一直到 laneCount
	 */
	auto GetLane(RoadDirection dir, int laneId)->std::vector<Car*>&;

	/**
	 * @brief 获取该道路上特定位置的车辆
	 *
	 * @param dir 指定方向
	 * @param laneId 车道编号
	 * @param pos 位置的下标
	 * @return 车道上所有位置的车辆数据
	 * @remark 车道编号从 1 开始，一直到 laneCount。位置下标从 0 开始
	 */
	auto GetCar(RoadDirection dir, int laneId, int pos)->Car*;

	/**
	 * @brief 获取方向
	 *
	 * @param entrance 进入道路的路口
	 * @return 根据进入的路口返回正确的方向
	 */
	auto GetDirection(Node *entrance)->RoadDirection;

	/**
	 * @brief 指示能否从某个路口进入并且在路上通行
	 *
	 * @param entrance 表示进入的路口
	 * @return true 表示可以从该路口进入通行
	 * @return false 表示不能从该路口进入通行
	 */
	auto CanGoThrough(Node *entrance) -> bool;

	/**
	 * @brief 获取从某个路口进入通行后将到达的下一个路口
	 *
	 * @param entrance 进入该道路的路口
	 * @return Node* 下一个到达的路口
	 */
	auto GetNextNode(Node *entrance)->Node*;
};

/**
 * @brief 表示一个路口
 */
class Node : public Object {
public:
	/**
	 * @brief 构造函数
	 *
	 * @param _id 路口的 id
	 * @param r1, r2, r3, r4 与路口相连的四条道路，按照顺时针顺序给出
	 * @remark 如果某个方向上没有道路，则传入 -1
	 */
	Node(int _id, int r1, int r2, int r3, int r4);

	~Node();

	/// 路口四个方向的道路的 id
	int roadIds[MAX_ROADS_PER_NODE];
	/// 路口四个方向的道路，按照输入给定的顺序
	/// 即顺时针顺序存储。如果某个位置为 nullptr，则表示这里没有连接道路
	Road *roads[MAX_ROADS_PER_NODE];

	void Init();
	void Reset();

	/**
	 * @brief 获取与路口相连的道路
	 *
	 * @param i 道路的下标
	 * @return 返回对应下标的道路。如果不存在，则返回 nullptr
	 */
	auto operator[](const size_t i)->Road*;
};

/**
 * @brief 表示一条路线
 */
class Path {
public:
	/// 记录路线上将要依次经过的道路
	std::deque<Road*> roads;
	/// 记录已经走过的路径
	std::vector<Road*> walked;

	/**
	 * @brief 访问路线上的道路
	 *
	 * @param i 道路的下标
	 * @return 返回对应的道路
	 */
	auto operator[](const size_t i)->Road*;

	/**
	 * @brief 获取路线上经过的道路条数
	 *
	 * @return 道路条数
	 */
	auto GetSize() -> int;

	/**
	 * @brief 获取当前道路
	 *
	 * @return 返回道路
	 */
	auto GetCurrentRoad()->Road*;

	/**
	 * @brief 获取路线上的下一条道路
	 *
	 * @return 返回下一条道路。如果不存在则返回 nullptr
	 */
	auto GetNextRoad()->Road*;

	/**
	 * @brief 获取当前路线进入时的路口
	 *
	 * @return 返回进入时的路口
	 */
	auto GetPrevNode()->Node*;

	/**
	 * @brief 获取当前路线进入下一条道路的路口
	 *
	 * @return 路线上的下一个路口
	 */
	auto GetNextNode()->Node*;

	/**
	 * @brief 获取路线在当前道路上的方向
	 *
	 * @return 相对于道路的方向
	 */
	auto GetCurrentDirection()->RoadDirection;

	/**
	 * @brief 获取路线在下一条道路上的方向
	 *
	 * @return 相对于下一条道路的方向
	 */
	auto GetNextDirection()->RoadDirection;

	/**
	 * @brief 获取进入下一条道路时的转向
	 *
	 * @return 转弯的方向。出现意外时返回 UNKONWN
	 */
	auto GetTurning()->Turning;

	/**
	 * @brief 进入下一条道路
	 *
	 * @remark 实际上就是直接删去当前道路
	 */
	void GoForward();
};

/**
 * @brief 表示一辆汽车
 *
 * @remark 车身长度为 1
 */
class Car : public Object {
public:
	/**
	 * @brief 构造函数
	 *
	 * @params 对应同名的成员变量
	 */
	Car(int _id, int _maxSpeed, int _minStartTime);

	~Car();

	/// 车辆的最大行驶速度
	int maxSpeed;
	/// 最早的出发时间
	int minStartTime;

	/// 车辆的实际出发时间
	int startTime;
	/// 车辆的到达时间
	int endTime;
	/// 车辆所在的车道编号
	int laneId;
	/// 当前车辆在路上已经行驶的距离
	int position;
	/// 当前车辆在路上行驶的方向
	RoadDirection direction;
	/// 当前车辆行驶的路线
	Path *path = nullptr;
	///当前车辆还能开多长距离
	int nowspeed;

	void Init();
	void Reset();

	/**
	 * @brief 获取将要行驶到的下一个路口
	 *
	 * @return 返回这个路口
	 */
	auto GetNextNode()->Node*;

	/**
	 * @brief 获取进入当前道路的路口
	 *
	 * @return 返回这个路口
	 */
	auto GetPrevNode()->Node*;

	/**
	 * @brief 获取当前正在行驶的道路
	 *
	 * @return 返回道路
	 */
	auto GetCurrentRoad()->Road*;

	/**
	 * @brief 获取下一条要行驶的道路
	 *
	 * @return 返回道路。如果不存在，则返回 nullptr
	 */
	auto GetNextRoad()->Road*;

	/**
	 * @brief 获取在当前车道上的前一辆的车
	 *
	 * @return 返回这辆车。如果当前车前面没有其余车辆，则返回 nullptr
	 */
	auto GetNextCar()->Car*;

	/**
	 * @brief 获取在当前车道上的后一辆的车
	 *
	 * @return 返回这辆车。如果当前车后面没有其余车辆，则返回 nullptr
	 */
	auto GetPrevCar()->Car*;

	/**
	 * @brief 获取在当前车道上的左边的车
	 *
	 * @return 返回这辆车。如果当前车左边没有其余车辆，则返回 nullptr
	 * @remark 左边是相对于行驶方向，即车道编号减小的方向
	 */
	auto GetLeftCar()->Car*;

	/**
	 * @brief 获取在当前车道上的右边的车
	 *
	 * @return 返回这辆车。如果当前车右边没有其余车辆，则返回 nullptr
	 * @remark 右边是相对于行驶方向，即车道编号增大的方向
	 */
	auto GetRightCar()->Car*;

	/**
	 * @brief 获取在下一条道路上的行驶方向
	 *
	 * @return 返回方向
	 */
	auto GetNextDirection()->RoadDirection;

	/**
	 * @brief 获取在当前道路上的剩余路程
	 *
	 * @return 返回剩余路程
	 */
	auto GetRemainLength() -> int;

	/**
	 * @brief 获取进入下一条道路时的车道编号
	 *
	 * @return 返回车道编号。如果不存在下一条道路，则返回 0
	 */
	auto GetNextEntrance() -> int;

	/**
	 * @brief 在当前道路上继续行驶
	 *
	 * @param step 行驶的距离
	 * @remark 注意不能走出当前的道路，并且需要自行保证行走的这段内没有其它车辆
	 */
	void GoForward(int step);

	/**
	 * @brief 走过下一个路口到达下一条道路上
	 *
	 * @param newLane 在下一条道路的上的车道编号
	 * @param step 在下一条道路上行驶的距离
	 * @remark 需要自行保证行走的这段内没有其它车辆
	 */
	void GotoNextRoad(int newLane, int step);

	/**
	 * @brief 走过下一个路口到达下一条道路上
	 *
	 * @param step 在下一条道路上的行驶的距离
	 * @remark 本函数是 GotoNextRoad(int newLane, int step) 的辅助函数
	 */
	void GotoNextRoad(int step);

	/**
	 * @brief 获取汽车行驶的时间
	 *
	 * @return 返回行驶时间
	 */
	auto GetTimeUsed() -> int;
};

/**
 * @brief 表示一次路线的询问
 */
class Request {
public:
	/**
	 * @brief 构造函数
	 *
	 * @params 对应同名的成员变量
	 */
	Request(int _startId, int _endId, Car *_car);

	/// 路线的起点路口 id
	int startId;
	/// 路线的终点路口 id
	int endId;
	/// 路线的起点路口
	Node *startNode;
	/// 路线的终点路口
	Node *endNode;
	/// 行驶的车辆。规划路线的算法可以修改 car->path
	Car *car;
};

/**
 * @brief 表示整个交通调度系统的情况
 */
class System {
public:
	/**
	 * @brief 构造函数
	 *
	 * @param nodesData 路口的数据文件
	 * @param roadsData 道路的数据文件
	 * @param carsData 车辆的数据文件
	 */
	System(const std::string &nodesData,
		const std::string &roadsData,
		const std::string &carsData);

	/**
	 * @brief 析构函数
	 *
	 */
	~System();

	/**
	 * @brief 在开始调度前触发
	 */
	Event<> onScheduleBegin;

	/**
	 * @brief 在结束调度后触发
	 */
	Event<> onScheduleFinish;

	/**
	 * @brief 在每次更新前触发
	 */
	Event<> beforeUpdate;

	/**
	 * @brief 在无法继续移动已上路的车辆时触发
	 *
	 * @remark 此时准备让未上路的车出发
	 */
	Event<> onNoUpdates;

	/**
	 * @brief 在每次更新完毕时触发
	 */
	Event<> afterUpdate;

	/**
	 * @brief 当车辆可以上路时触发
	 *
	 * @remark 如果车辆没有上路且已经到达最早出发时间，就会触发此事件
	 */
	Event<Car*> onCarCanStart;

	/**
	 * @brief 当车辆出发时触发
	 */
	Event<Car*> onCarStarted;

	/**
	 * @brief 当车辆在路上移动时触发
	 */
	Event<Car*> onCarMoved;

	/**
	 * @brief 当车辆到达终点时触发
	 */
	Event<Car*> onCarArrived;

	/**
	 * @brief 需要请求规划路线时触发该事件
	 *
	 * 例如，监听 onCarCanStart 时间的 listener 可以通过触发该事件来请求导航路线。
	 * 负责规划的路线的算法需要监听这个事件
	 * 新规划的路线需要填入 Request 的 car 成员中 path
	 * 如果算法没有修改 path（即 path 还是 nullptr）
	 * 那么表示算法还没有计划让车辆上路
	 */
	Event<Request*> onNewRequest;

	/**
	 * @brief 访问特定 id 的物体
	 *
	 * @return 返回对应的物体。如果该 id 不存在，则返回 nullptr
	 */
	auto operator[](int id)->Object*;

	/**
	 * @brief 获得特定 id 的路口
	 *
	 * @param id 指定的 id
	 * @return 返回对应的路口。如果该 id 不存在，则返回 nullptr
	 */
	auto GetNode(int id)->Node*;

	/**
	 * @brief 获得特定 id 的道路
	 *
	 * @param id 指定的 id
	 * @return 返回对应的道路。如果该 id 不存在，则返回 nullptr
	 */
	auto GetRoad(int id)->Road*;

	/**
	 * @brief 获得特定 id 的车辆
	 *
	 * @param id 指定的 id
	 * @return 返回对应的车辆。如果该 id 不存在，则返回 nullptr
	 */
	auto GetCar(int id)->Car*;

	/**
	 * @brief 从 cross.txt 中读取所有路口的信息
	 *
	 * @param path 文件 cross.txt 的位置
	 */
	void ReadNodesFromFile(const std::string &path);

	/**
	 * @brief 从 road.txt 中读取所有道路的信息
	 *
	 * @param path 文件 road.txt 的位置
	 */
	void ReadRoadsFromFile(const std::string &path);

	/**
	 * @brief 从 car.txt 中读取所有车辆的信息
	 *
	 * @param path 文件 car.txt 的位置
	 * @remark 车辆的起点和终点记录在 _reqs 中
	 */
	void ReadCarsFromFile(const std::string &path);

	/**
	 * @brief 将所有车辆行驶过的路径写入文件
	 *
	 * @param path 文件路径（answer.txt)
	 */
	void WriteAnswers(const std::string &path);

	/**
	 * @brief 获取所有路口
	 *
	 * 主要用于 for each 语法的遍历
	 *
	 * @return 返回一个可以用于遍历的封装层
	 */
	auto Nodes()->MapWrapper<Node>;

	/**
	 * @brief 获取所有道路
	 *
	 * 主要用于 for each 语法的遍历
	 *
	 * @return 返回一个可以用于遍历的封装层
	 */
	auto Roads()->MapWrapper<Road>;

	/**
	 * @brief 获取所有车辆
	 *
	 * 主要用于 for each 语法的遍历
	 *
	 * @return 返回一个可以用于遍历的封装层
	 */
	auto Cars()->MapWrapper<Car>;

	/**
	 * @brief 进行一次更新
	 */
	void Update();

	/**
	 * @brief 为每一台车辆都请求一次路线
	 *
	 * @param copyToWalked 如果为 true，则请求到的路径直接写入 Car::path->walked 中
	 *                     用于直接输出答案
	 * @remark 仅供调试算法用
	 */
	void RequestForAllCars(bool copyToWalked = false);

	/**
	 * @brief 重置整个系统的状态
	 */
	void Reset();

	/**
	 * @brief 指示系统的调度是否开始
	 *
	 * @return 返回 true 表示已经开始，否则返回 false
	 */
	auto Started() -> bool;

	/**
	 * @brief 指示系统的调度是否结束
	 *
	 * @return true 表示调度已经结束，包括死锁的情况
	 * @return false 表示仍然可以行车
	 * @remark 注意只要系统不能继续行车时就已经结束
	 */
	auto Stopped() -> bool;

	/**
	 * @brief 指示所有车辆是否到达终点
	 *
	 * @return true 表示已经结束，所有车辆到达终点
	 * @return false 表示存在车辆还未到达终点
	 */
	auto Finished() -> bool;

	/**
	 * @brief 获取当前系统调度的时间
	 *
	 * @return 返回调度时间
	 */
	auto GetCurrentTime() -> int;

private:
	/// 系统调度时间
	int _time;
	/// 系统内所有道路
	std::unordered_map<int, Node*> _nodes;
	/// 系统内所有道路
	std::unordered_map<int, Road*> _roads;
	/// 系统内所有车辆
	std::unordered_map<int, Car*> _cars;
	/// 车辆的起点和终点
	std::vector<Request*> _reqs;
};

#endif  // __SYSTEM_HPP__
