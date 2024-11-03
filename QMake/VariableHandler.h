#ifndef VARIABLE_HANDLER_H
#define VARIABLE_HANDLER_H

#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "ErrorDefinitions.h"


#define MAP std::unordered_map<std::string, std::string>

#define TOTAL_E_FLAGS 3

#define E_UNKNOWN_INDEX	0
#define E_MISTYPE		1
#define E_DUPLICATION	2

namespace qmake {

	class VariableHandler {
		public:
			VariableHandler(bool& reading_stopped);
			~VariableHandler() = default;
			
			const std::string	fetchVariableValue(const std::string& variable_name);
			void				insertVariableLine(const std::string line);
			void				processLine();

			const unsigned short* getFlags() const
			{	return m_error_flags;	}

			const auto getSize() { return m_variable_queue.size(); }

			std::mutex&					getMutex() { return m_mutex; }
			std::condition_variable&	getConditionalVariable() { return m_conditional_variable; }

		private:
			std::queue<std::string>		m_variable_queue;
			MAP							m_variable_map;
			unsigned short				m_error_flags[TOTAL_E_FLAGS];
			bool&						m_reading_stopped;
			std::mutex					m_mutex;
			std::condition_variable		m_conditional_variable;

	};
}
#endif // ! VARIABLE_HANDLER_H