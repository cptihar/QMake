#ifndef LABEL_HANDLER_H
#define LABEL_HANDLER_H

#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>


namespace qmake {

	class LabelHandler {
		public:
			LabelHandler(bool& reading_stopped);
			~LabelHandler() = default;
			void processLabel();
			void pushForProcess(std::string line);
			void insertVariable(const std::string& var_value, const std::string& var_name);

			std::mutex&							getMutex() { return m_mutex; }
			std::condition_variable&			getCv() { return m_conditional_variable; };
			const std::vector<std::string>&		getVariables() const
			{	return m_variables;	}

			const std::string getContents() const 
			{ return m_label_contents; }

		private:
			std::vector<std::string>	m_variables;
			std::string					m_label_contents;
			std::queue<std::string>		m_string_queue;
			bool&						m_reading_stopped;
			std::mutex					m_mutex;
			std::condition_variable		m_conditional_variable;


		private:
			std::string m_extractInfo(unsigned int start, const std::string& line);
	};
}
#endif // LABEL_HANDLER_H