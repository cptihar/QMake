#include "LabelHandler.h"

qmake::LabelHandler::LabelHandler(bool& reading_stopped)
	:m_reading_stopped(reading_stopped)
{
}

////////////////////////////////////////|
/// 
///	Processes the label's line
/// Extracts information regarding
///		-> Variable name
/// 
/// Runs on a seperate thread than main
/// 
/// Returns:
///		void
///
////////////////////////////////////////|
void qmake::LabelHandler::processLabel()
{
	while (1) {
		// Halt if there is no string to process.
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// Wait until we have something to process
			m_conditional_variable.wait(lock, [this]() {
				return !m_string_queue.empty() || m_reading_stopped;
				});

			if (m_reading_stopped && m_string_queue.empty()) {
				break; // If the reading was stopped and the queue is empty we break the loop
			}
		}

		// Parse through
		std::string line = m_string_queue.front();
		m_string_queue.pop();
		for (unsigned int i = 0; i < line.size(); ++i) {
			if (line[i] == '$') {
				// Extract variable
				m_variables.push_back(m_extractInfo(i, line));
			}
		}
		m_label_contents += line;
	}
}


/////////////////////////////////|
///
///	Pushes a line for processing	
///
/// Returns: void
///
/////////////////////////////////|
void qmake::LabelHandler::pushForProcess(std::string line)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex); // Lock for exclusive use
		m_string_queue.push(line);
	}
	m_conditional_variable.notify_one();
}


///////////////////////////////////////////////////|
/// 
/// Inserts the variable value to its place in the
/// label contents
/// 
/// Return: void
///
///////////////////////////////////////////////////|
void qmake::LabelHandler::insertVariable(const std::string& var_value, const std::string& var_name)
{
	auto position = m_label_contents.find("$"+var_name);
	m_label_contents.replace(position, var_name.size()+1, var_value);
}


//////////////////////////////////////////|
///
///	Extracts the variable from the string
///	Return: std::string
///
//////////////////////////////////////////|
std::string qmake::LabelHandler::m_extractInfo(unsigned int start, const std::string& line)
{
	unsigned int begin = start+1;
	++start;

	while (start < line.size()) {
		if (line[start] == ' ' || line[start] == '\t') {
			break;
		}
		++start;
	}
	return line.substr(begin, start - begin);
}
