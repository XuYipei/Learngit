#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <deque>
#include <vector>
#include <string>
#include <unordered_map>

#include "Event.hpp"
#include "MapWrapper.hpp"

// ǰ������
class Object;
class Road;
class Node;
class Path;
class Car;
class Request;
class System;

/**
 * @brief ÿ��·������� 4 ����·
 */
constexpr int MAX_ROADS_PER_NODE = 4;

/**
 * @brief �����п��ܵ���� ID
 *
 */
constexpr int MAX_ID = 50000;

/**
 * @brief ��ʾ˫�е��ķ���
 *
 * ����ö�ٷֱ��ʾ�ӵ�·�������㵽�յ㡢�յ㵽���
 */
enum RoadDirection {
	START_TO_END, END_TO_START
};

/**
 * @brief ��ʾת��ķ���
 *
 * RIGHT��LEFT��FORWARD �ֱ��ʾ�ҹա���պ�ֱ�У���ֵ����
 */
enum Turning {
	UNKNOWN = 0, RIGHT, LEFT, FORWARD
};

/**
 * @brief ��ʾһ������
 */
class Object {
public:
	/**
	 * @brief ����Ĭ�Ϲ��캯����Ҫ������ṩ id
	 */
	Object() = delete;

	/**
	 * @brief ���캯��
	 *
	 * @param id �������ȫ��Ψһ id
	 */
	Object(int _id);

	virtual ~Object();

	/// ��ʾ����� id
	int id;
	/// ��ʾ�������ڵĽ�ͨϵͳ
	System *system;

	/**
	 * @brief ��ʼ��
	 *
	 * @remark ��Ϊ�����ڹ���ʱϵͳ��û��׼���ã���˸ú�������ϵͳ׼���ú�
	 *         ͳһ���õġ��ú���ֻ���� System ��ʼ��ʱ����һ��
	 */
	virtual void Init() = 0;

	/**
	 * @brief ����״̬
	 *
	 * @remark �ṩ�� System::Reset ����
	 */
	virtual void Reset() = 0;
};

/**
 * @brief ��ʾһ����·
 */
class Road : public Object {
public:
	/**
	 * @brief ���캯��
	 *
	 * @params ��Ӧͬ���ĳ�Ա����
	 */
	Road(int _id, int _length, int _maxSpeed, int _laneCount, int _startId,
		int _endId, bool _isDuplex);

	~Road();

	/// ��·����
	int length;
	/// ·�ϵ��������
	int maxSpeed;
	/// ���򳵵������������˫���ߣ������ߵĳ�������һ��
	int laneCount;
	/// ��·����ʼ·�ڵ� id
	int startId;
	/// ��·����ֹ·�ڵ� id
	int endId;
	/// �Ƿ�Ϊ˫����
	bool isDuplex;

	/// ��·����ʼ·��
	Node *startNode;
	/// ��·����ֹ·��
	Node *endNode;
	/// ��·��ÿ��λ�õĳ�����Ϣ
	/// ���ĳ��λ����û�г�������Ϊ nullptr
	std::vector<std::vector<Car*>> cars;

	void Init();
	void Reset();

	/**
	 * @brief ����ĳһ������
	 *
	 * @param i �������±�
	 * @return ���ظó����Ӻ���ǰ������λ���ϵĳ�����Ϣ������Ϊ length��
	 *         0 �Ǹս����·�ĳ������±�Խ���ڸó�������ʻ����ԽԶ
	 * @remark �����±겻�ǳ������
	 */
	auto operator[](const size_t i)->std::vector<Car*>&;

	/**
	 * @brief ��ȡ�ܳ�������
	 *
	 * @return �ܳ��������������˫���ߣ���Ϊ laneCount ������
	 */
	auto GetLaneCount() -> int;

	/**
	 * @brief ��ȡ����������
	 *
	 * @return ���������ɵĳ���������ע��ֻ��һ������
	 */
	auto GetMaxCapacity() -> int;

	/**
	 * @brief ��ȡ�����ϵ�ǰ��������
	 *
	 * @param dir ָ������
	 * @return �÷����ϵĳ�������
	 */
	auto GetCarCount(RoadDirection dir) -> int;

	/**
	 * @brief ��ȡ������ʣ��λ������
	 *
	 * @param dir ָ������
	 * @return ���ظ÷����Ͽ�λ����
	 */
	auto GetCapacity(RoadDirection dir) -> int;

	/**
	 * @brief ��ȡ��ĳ����������·ʱ���ó�λ����
	 *
	 * ��ÿ����·���ҵ���������Ͻ��������ĵ�һ�����������������λ�ö��ǿ��õġ�
	 * �ú�������ĳ��������ÿ����·�Ŀ��ó�λֵ���ܺ�
	 *
	 * @param dir ָ������
	 * @return ���ؿɽ����λ������
	 */
	auto GetEntranceCount(RoadDirection dir) -> int;

	/**
	 * @brief ȡ�����
	 *
	 * @param dir ָ������ķ���
	 * @return ������Խ����·���򷵻ؽ�Ҫ����ĳ�����š����򷵻� 0
	 */
	auto GetEntrance(RoadDirection dir) -> int;

	/**
	 * @brief ��ȡĳ���������ض���ŵĳ�������
	 *
	 * @param dir ָ������
	 * @param laneId �������
	 * @return ����������λ�õĳ�������
	 * @remark ������Ŵ� 1 ��ʼ��һֱ�� laneCount
	 */
	auto GetLane(RoadDirection dir, int laneId)->std::vector<Car*>&;

	/**
	 * @brief ��ȡ�õ�·���ض�λ�õĳ���
	 *
	 * @param dir ָ������
	 * @param laneId �������
	 * @param pos λ�õ��±�
	 * @return ����������λ�õĳ�������
	 * @remark ������Ŵ� 1 ��ʼ��һֱ�� laneCount��λ���±�� 0 ��ʼ
	 */
	auto GetCar(RoadDirection dir, int laneId, int pos)->Car*;

	/**
	 * @brief ��ȡ����
	 *
	 * @param entrance �����·��·��
	 * @return ���ݽ����·�ڷ�����ȷ�ķ���
	 */
	auto GetDirection(Node *entrance)->RoadDirection;

	/**
	 * @brief ָʾ�ܷ��ĳ��·�ڽ��벢����·��ͨ��
	 *
	 * @param entrance ��ʾ�����·��
	 * @return true ��ʾ���ԴӸ�·�ڽ���ͨ��
	 * @return false ��ʾ���ܴӸ�·�ڽ���ͨ��
	 */
	auto CanGoThrough(Node *entrance) -> bool;

	/**
	 * @brief ��ȡ��ĳ��·�ڽ���ͨ�к󽫵������һ��·��
	 *
	 * @param entrance ����õ�·��·��
	 * @return Node* ��һ�������·��
	 */
	auto GetNextNode(Node *entrance)->Node*;
};

/**
 * @brief ��ʾһ��·��
 */
class Node : public Object {
public:
	/**
	 * @brief ���캯��
	 *
	 * @param _id ·�ڵ� id
	 * @param r1, r2, r3, r4 ��·��������������·������˳ʱ��˳�����
	 * @remark ���ĳ��������û�е�·������ -1
	 */
	Node(int _id, int r1, int r2, int r3, int r4);

	~Node();

	/// ·���ĸ�����ĵ�·�� id
	int roadIds[MAX_ROADS_PER_NODE];
	/// ·���ĸ�����ĵ�·���������������˳��
	/// ��˳ʱ��˳��洢�����ĳ��λ��Ϊ nullptr�����ʾ����û�����ӵ�·
	Road *roads[MAX_ROADS_PER_NODE];

	void Init();
	void Reset();

	/**
	 * @brief ��ȡ��·�������ĵ�·
	 *
	 * @param i ��·���±�
	 * @return ���ض�Ӧ�±�ĵ�·����������ڣ��򷵻� nullptr
	 */
	auto operator[](const size_t i)->Road*;
};

/**
 * @brief ��ʾһ��·��
 */
class Path {
public:
	/// ��¼·���Ͻ�Ҫ���ξ����ĵ�·
	std::deque<Road*> roads;
	/// ��¼�Ѿ��߹���·��
	std::vector<Road*> walked;

	/**
	 * @brief ����·���ϵĵ�·
	 *
	 * @param i ��·���±�
	 * @return ���ض�Ӧ�ĵ�·
	 */
	auto operator[](const size_t i)->Road*;

	/**
	 * @brief ��ȡ·���Ͼ����ĵ�·����
	 *
	 * @return ��·����
	 */
	auto GetSize() -> int;

	/**
	 * @brief ��ȡ��ǰ��·
	 *
	 * @return ���ص�·
	 */
	auto GetCurrentRoad()->Road*;

	/**
	 * @brief ��ȡ·���ϵ���һ����·
	 *
	 * @return ������һ����·������������򷵻� nullptr
	 */
	auto GetNextRoad()->Road*;

	/**
	 * @brief ��ȡ��ǰ·�߽���ʱ��·��
	 *
	 * @return ���ؽ���ʱ��·��
	 */
	auto GetPrevNode()->Node*;

	/**
	 * @brief ��ȡ��ǰ·�߽�����һ����·��·��
	 *
	 * @return ·���ϵ���һ��·��
	 */
	auto GetNextNode()->Node*;

	/**
	 * @brief ��ȡ·���ڵ�ǰ��·�ϵķ���
	 *
	 * @return ����ڵ�·�ķ���
	 */
	auto GetCurrentDirection()->RoadDirection;

	/**
	 * @brief ��ȡ·������һ����·�ϵķ���
	 *
	 * @return �������һ����·�ķ���
	 */
	auto GetNextDirection()->RoadDirection;

	/**
	 * @brief ��ȡ������һ����·ʱ��ת��
	 *
	 * @return ת��ķ��򡣳�������ʱ���� UNKONWN
	 */
	auto GetTurning()->Turning;

	/**
	 * @brief ������һ����·
	 *
	 * @remark ʵ���Ͼ���ֱ��ɾȥ��ǰ��·
	 */
	void GoForward();
};

/**
 * @brief ��ʾһ������
 *
 * @remark ������Ϊ 1
 */
class Car : public Object {
public:
	/**
	 * @brief ���캯��
	 *
	 * @params ��Ӧͬ���ĳ�Ա����
	 */
	Car(int _id, int _maxSpeed, int _minStartTime);

	~Car();

	/// �����������ʻ�ٶ�
	int maxSpeed;
	/// ����ĳ���ʱ��
	int minStartTime;

	/// ������ʵ�ʳ���ʱ��
	int startTime;
	/// �����ĵ���ʱ��
	int endTime;
	/// �������ڵĳ������
	int laneId;
	/// ��ǰ������·���Ѿ���ʻ�ľ���
	int position;
	/// ��ǰ������·����ʻ�ķ���
	RoadDirection direction;
	/// ��ǰ������ʻ��·��
	Path *path = nullptr;
	///��ǰ�������ܿ��೤����
	int nowspeed;

	void Init();
	void Reset();

	/**
	 * @brief ��ȡ��Ҫ��ʻ������һ��·��
	 *
	 * @return �������·��
	 */
	auto GetNextNode()->Node*;

	/**
	 * @brief ��ȡ���뵱ǰ��·��·��
	 *
	 * @return �������·��
	 */
	auto GetPrevNode()->Node*;

	/**
	 * @brief ��ȡ��ǰ������ʻ�ĵ�·
	 *
	 * @return ���ص�·
	 */
	auto GetCurrentRoad()->Road*;

	/**
	 * @brief ��ȡ��һ��Ҫ��ʻ�ĵ�·
	 *
	 * @return ���ص�·����������ڣ��򷵻� nullptr
	 */
	auto GetNextRoad()->Road*;

	/**
	 * @brief ��ȡ�ڵ�ǰ�����ϵ�ǰһ���ĳ�
	 *
	 * @return �����������������ǰ��ǰ��û�����೵�����򷵻� nullptr
	 */
	auto GetNextCar()->Car*;

	/**
	 * @brief ��ȡ�ڵ�ǰ�����ϵĺ�һ���ĳ�
	 *
	 * @return �����������������ǰ������û�����೵�����򷵻� nullptr
	 */
	auto GetPrevCar()->Car*;

	/**
	 * @brief ��ȡ�ڵ�ǰ�����ϵ���ߵĳ�
	 *
	 * @return �����������������ǰ�����û�����೵�����򷵻� nullptr
	 * @remark ������������ʻ���򣬼�������ż�С�ķ���
	 */
	auto GetLeftCar()->Car*;

	/**
	 * @brief ��ȡ�ڵ�ǰ�����ϵ��ұߵĳ�
	 *
	 * @return �����������������ǰ���ұ�û�����೵�����򷵻� nullptr
	 * @remark �ұ����������ʻ���򣬼������������ķ���
	 */
	auto GetRightCar()->Car*;

	/**
	 * @brief ��ȡ����һ����·�ϵ���ʻ����
	 *
	 * @return ���ط���
	 */
	auto GetNextDirection()->RoadDirection;

	/**
	 * @brief ��ȡ�ڵ�ǰ��·�ϵ�ʣ��·��
	 *
	 * @return ����ʣ��·��
	 */
	auto GetRemainLength() -> int;

	/**
	 * @brief ��ȡ������һ����·ʱ�ĳ������
	 *
	 * @return ���س�����š������������һ����·���򷵻� 0
	 */
	auto GetNextEntrance() -> int;

	/**
	 * @brief �ڵ�ǰ��·�ϼ�����ʻ
	 *
	 * @param step ��ʻ�ľ���
	 * @remark ע�ⲻ���߳���ǰ�ĵ�·��������Ҫ���б�֤���ߵ������û����������
	 */
	void GoForward(int step);

	/**
	 * @brief �߹���һ��·�ڵ�����һ����·��
	 *
	 * @param newLane ����һ����·���ϵĳ������
	 * @param step ����һ����·����ʻ�ľ���
	 * @remark ��Ҫ���б�֤���ߵ������û����������
	 */
	void GotoNextRoad(int newLane, int step);

	/**
	 * @brief �߹���һ��·�ڵ�����һ����·��
	 *
	 * @param step ����һ����·�ϵ���ʻ�ľ���
	 * @remark �������� GotoNextRoad(int newLane, int step) �ĸ�������
	 */
	void GotoNextRoad(int step);

	/**
	 * @brief ��ȡ������ʻ��ʱ��
	 *
	 * @return ������ʻʱ��
	 */
	auto GetTimeUsed() -> int;
};

/**
 * @brief ��ʾһ��·�ߵ�ѯ��
 */
class Request {
public:
	/**
	 * @brief ���캯��
	 *
	 * @params ��Ӧͬ���ĳ�Ա����
	 */
	Request(int _startId, int _endId, Car *_car);

	/// ·�ߵ����·�� id
	int startId;
	/// ·�ߵ��յ�·�� id
	int endId;
	/// ·�ߵ����·��
	Node *startNode;
	/// ·�ߵ��յ�·��
	Node *endNode;
	/// ��ʻ�ĳ������滮·�ߵ��㷨�����޸� car->path
	Car *car;
};

/**
 * @brief ��ʾ������ͨ����ϵͳ�����
 */
class System {
public:
	/**
	 * @brief ���캯��
	 *
	 * @param nodesData ·�ڵ������ļ�
	 * @param roadsData ��·�������ļ�
	 * @param carsData �����������ļ�
	 */
	System(const std::string &nodesData,
		const std::string &roadsData,
		const std::string &carsData);

	/**
	 * @brief ��������
	 *
	 */
	~System();

	/**
	 * @brief �ڿ�ʼ����ǰ����
	 */
	Event<> onScheduleBegin;

	/**
	 * @brief �ڽ������Ⱥ󴥷�
	 */
	Event<> onScheduleFinish;

	/**
	 * @brief ��ÿ�θ���ǰ����
	 */
	Event<> beforeUpdate;

	/**
	 * @brief ���޷������ƶ�����·�ĳ���ʱ����
	 *
	 * @remark ��ʱ׼����δ��·�ĳ�����
	 */
	Event<> onNoUpdates;

	/**
	 * @brief ��ÿ�θ������ʱ����
	 */
	Event<> afterUpdate;

	/**
	 * @brief ������������·ʱ����
	 *
	 * @remark �������û����·���Ѿ������������ʱ�䣬�ͻᴥ�����¼�
	 */
	Event<Car*> onCarCanStart;

	/**
	 * @brief ����������ʱ����
	 */
	Event<Car*> onCarStarted;

	/**
	 * @brief ��������·���ƶ�ʱ����
	 */
	Event<Car*> onCarMoved;

	/**
	 * @brief �����������յ�ʱ����
	 */
	Event<Car*> onCarArrived;

	/**
	 * @brief ��Ҫ����滮·��ʱ�������¼�
	 *
	 * ���磬���� onCarCanStart ʱ��� listener ����ͨ���������¼������󵼺�·�ߡ�
	 * ����滮��·�ߵ��㷨��Ҫ��������¼�
	 * �¹滮��·����Ҫ���� Request �� car ��Ա�� path
	 * ����㷨û���޸� path���� path ���� nullptr��
	 * ��ô��ʾ�㷨��û�мƻ��ó�����·
	 */
	Event<Request*> onNewRequest;

	/**
	 * @brief �����ض� id ������
	 *
	 * @return ���ض�Ӧ�����塣����� id �����ڣ��򷵻� nullptr
	 */
	auto operator[](int id)->Object*;

	/**
	 * @brief ����ض� id ��·��
	 *
	 * @param id ָ���� id
	 * @return ���ض�Ӧ��·�ڡ������ id �����ڣ��򷵻� nullptr
	 */
	auto GetNode(int id)->Node*;

	/**
	 * @brief ����ض� id �ĵ�·
	 *
	 * @param id ָ���� id
	 * @return ���ض�Ӧ�ĵ�·������� id �����ڣ��򷵻� nullptr
	 */
	auto GetRoad(int id)->Road*;

	/**
	 * @brief ����ض� id �ĳ���
	 *
	 * @param id ָ���� id
	 * @return ���ض�Ӧ�ĳ���������� id �����ڣ��򷵻� nullptr
	 */
	auto GetCar(int id)->Car*;

	/**
	 * @brief �� cross.txt �ж�ȡ����·�ڵ���Ϣ
	 *
	 * @param path �ļ� cross.txt ��λ��
	 */
	void ReadNodesFromFile(const std::string &path);

	/**
	 * @brief �� road.txt �ж�ȡ���е�·����Ϣ
	 *
	 * @param path �ļ� road.txt ��λ��
	 */
	void ReadRoadsFromFile(const std::string &path);

	/**
	 * @brief �� car.txt �ж�ȡ���г�������Ϣ
	 *
	 * @param path �ļ� car.txt ��λ��
	 * @remark �����������յ��¼�� _reqs ��
	 */
	void ReadCarsFromFile(const std::string &path);

	/**
	 * @brief �����г�����ʻ����·��д���ļ�
	 *
	 * @param path �ļ�·����answer.txt)
	 */
	void WriteAnswers(const std::string &path);

	/**
	 * @brief ��ȡ����·��
	 *
	 * ��Ҫ���� for each �﷨�ı���
	 *
	 * @return ����һ���������ڱ����ķ�װ��
	 */
	auto Nodes()->MapWrapper<Node>;

	/**
	 * @brief ��ȡ���е�·
	 *
	 * ��Ҫ���� for each �﷨�ı���
	 *
	 * @return ����һ���������ڱ����ķ�װ��
	 */
	auto Roads()->MapWrapper<Road>;

	/**
	 * @brief ��ȡ���г���
	 *
	 * ��Ҫ���� for each �﷨�ı���
	 *
	 * @return ����һ���������ڱ����ķ�װ��
	 */
	auto Cars()->MapWrapper<Car>;

	/**
	 * @brief ����һ�θ���
	 */
	void Update();

	/**
	 * @brief Ϊÿһ̨����������һ��·��
	 *
	 * @param copyToWalked ���Ϊ true�������󵽵�·��ֱ��д�� Car::path->walked ��
	 *                     ����ֱ�������
	 * @remark ���������㷨��
	 */
	void RequestForAllCars(bool copyToWalked = false);

	/**
	 * @brief ��������ϵͳ��״̬
	 */
	void Reset();

	/**
	 * @brief ָʾϵͳ�ĵ����Ƿ�ʼ
	 *
	 * @return ���� true ��ʾ�Ѿ���ʼ�����򷵻� false
	 */
	auto Started() -> bool;

	/**
	 * @brief ָʾϵͳ�ĵ����Ƿ����
	 *
	 * @return true ��ʾ�����Ѿ��������������������
	 * @return false ��ʾ��Ȼ�����г�
	 * @remark ע��ֻҪϵͳ���ܼ����г�ʱ���Ѿ�����
	 */
	auto Stopped() -> bool;

	/**
	 * @brief ָʾ���г����Ƿ񵽴��յ�
	 *
	 * @return true ��ʾ�Ѿ����������г��������յ�
	 * @return false ��ʾ���ڳ�����δ�����յ�
	 */
	auto Finished() -> bool;

	/**
	 * @brief ��ȡ��ǰϵͳ���ȵ�ʱ��
	 *
	 * @return ���ص���ʱ��
	 */
	auto GetCurrentTime() -> int;

private:
	/// ϵͳ����ʱ��
	int _time;
	/// ϵͳ�����е�·
	std::unordered_map<int, Node*> _nodes;
	/// ϵͳ�����е�·
	std::unordered_map<int, Road*> _roads;
	/// ϵͳ�����г���
	std::unordered_map<int, Car*> _cars;
	/// �����������յ�
	std::vector<Request*> _reqs;
};

#endif  // __SYSTEM_HPP__
