#ifndef __ISCHEDULER_HPP__
#define __ISCHEDULER_HPP__

#include "System.hpp"

/**
 * �滮�㷨�Ľӿ�
 */
class IScheduler {
public:
	/**
	 * ��ʼ���㷨
	 *
	 * @param sys ��Ҫ�㷨����Ľ�ͨϵͳ
	 */
	virtual void Init(System *sys) = 0;
};

/**
 * ���Ҫʵ�����㷨����Ҫʵ��һ�� IScheduler �����࣬��ʵ�� Init
 *
 * e.g.
 * class Scheduler : public IScheduler {
 *     // ...
 * };
 *
 * // �� main ������
 * Scheduler sch = new Scheduler;
 * sch->Init(system);
 * while (!system->Finished())
 *     system->Update();
 * // ...
 */

#endif  // __ISCHEDULER_HPP__