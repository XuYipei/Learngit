#ifndef __ISCHEDULER_HPP__
#define __ISCHEDULER_HPP__

#include "System.hpp"

/**
 * 规划算法的接口
 */
class IScheduler {
public:
	/**
	 * 初始化算法
	 *
	 * @param sys 需要算法服务的交通系统
	 */
	virtual void Init(System *sys) = 0;
};

/**
 * 如果要实现新算法，需要实现一个 IScheduler 的子类，并实现 Init
 *
 * e.g.
 * class Scheduler : public IScheduler {
 *     // ...
 * };
 *
 * // 在 main 函数中
 * Scheduler sch = new Scheduler;
 * sch->Init(system);
 * while (!system->Finished())
 *     system->Update();
 * // ...
 */

#endif  // __ISCHEDULER_HPP__