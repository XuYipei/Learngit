#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <vector>
#include <functional>

/**
 * ��ʾһ���¼�
 *
 * @remark ģ������б��ǻص������Ĳ��������б��ص��������޷���ֵ
 *         ����ص�������������ĳ�Ա����������ʹ�� std::bind ��ʵ��
 */
template <typename ... TArgs>
class Event {
public:
	typedef std::function<void(TArgs...)> TCallback;

	/**
	 * ����¼�������
	 *
	 * @param listener �����ص�����
	 */
	void AddListener(const TCallback &listener) {
		_listeners.push_back(listener);
	}

	/**
	 * ɾ���¼�������
	 *
	 * �ú��������ɾ��һ���봫�������ַ��ͬ�Ļص�����
	 *
	 * @param listener Ҫ��ɾ���Ļص�����
	 * @remark ���û���ҵ������޲���
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
	 * ������м�����
	 */
	void ClearListeners() {
		_listeners.clear();
	}

	/**
	 * �����¼���Ҳ���ǰ�˳��֪ͨ���м�����
	 *
	 * @params �ص������Ĳ���
	 */
	void Notify(TArgs... args) {
		for (auto &func : _listeners)
			func(args...);
	}

private:
	/// ���м�����
	std::vector<TCallback> _listeners;
};

#endif  // __EVENT_HPP__