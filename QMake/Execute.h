#ifndef QMAKE_EXECUTE_H
#define QMAKE_EXECUTE_H

#include "ResourceManager.h"
#include "ErrorHandler.h"
#include "ErrorDefinitions.h"
#include "VariableHandler.h"
#include "LabelHandler.h"


#include <thread>
#include <string>
#include <vector>

namespace qmake {
	class Execute {
		public:
			Execute(char** argv, int argc);
			~Execute();
			void prepare();
			void execute();

		private: // Variables
			qmake::ErrorHandler			m_error_handler;
			char**						r_argv;
			const int					m_argc;
			std::string					m_executable_string;
			std::string					m_build_tool;
			bool						m_should_execute;
			std::vector<std::string>	m_enviroment_variables;



		private: // Function
			unsigned long	m_findFirstNormalCharacter(const std::string& string);
			bool			m_strContains(const std::string& str, const char character);
			void			m_extractFlags(const unsigned short* arr, unsigned int size);
			std::string		m_trimLine(const std::string& str);
	};
}
#endif // !QMAKE_EXECUTE_H