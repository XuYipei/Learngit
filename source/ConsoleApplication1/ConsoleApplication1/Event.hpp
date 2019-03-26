#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <vector>
#include <functional>

/**
 * 表示一种事件
 *
 * @remark 模板参数列表是回调函数的参数类型列表。回调函数均无返回值
 *         如果回调函数是类里面的成员函数，可以使用 std::bind 来实现
 */
template <typename ... TArgs>
class Event {
public:
	typedef std::function<void(TArgs...)> TCallback;

	/**
	 * 添加事件监听器
	 *
	 * @param listener 监听回调函数
	 */
	void AddListener(const TCallback &listener) {
		_listeners.push_back(listener);
	}

	/**
	 * 删除事件监听器
	 *
	 * 该函数仅最多删除一个与传入参数地址相同的回调函数
	 *
	 * @param listener 要被删除的回调函数
	 * @remark 如果没有找到，则无操作
	 */
	void RemoveListener(const TCallback &listener) {
		typedef void (fnType)(TArgs...);
		auto *fn = listener.template target<fnType*>();
		for (auto iter = _listeners.begin(); iter != _listeners.end(); iter++) {
			auto *cur = listener.template target<fnType*>();
			if (fn == cur) {
				_listeners.erase(iter);
				return;
			}
		}
	}

	/**
	 * 清除所有监听器
	 */
	void ClearListeners() {
		_listeners.clear();
	}

	/**
	 * 触发事件。也就是按顺序通知所有监听器
	 *
	 * @params 回调函数的参数
	 */
	void Notify(TArgs... args) {
		for (auto &func : _listeners)
			func(args...);
	}

private:
	/// 所有监听器
	std::vector<TCallback> _listeners;
};

#endif  // __EVENT_HPP__