#include "VariableHandler.h"



qmake::VariableHandler::VariableHandler(bool& reading_stopped)
	:m_reading_stopped(reading_stopped)
{
	// Set the values to 0
	for (unsigned char i = 0; i < TOTAL_E_FLAGS; ++i) {
		m_error_flags[i] = 0;
	}
}


///////////////////////////////////////////////////////////////|
///
/// Provides an interface for fetching variables and its values
///
/// Makes the necessary checks, sets error flags if necessary
/// IF the variable does not exist returns "-"
/// Otherwise returns the variable's value
/// 
/// Return type:
///		const std::string
/// 
///////////////////////////////////////////////////////////////|
const std::string qmake::VariableHandler::fetchVariableValue(const std::string& variable_name)
{
	if (m_variable_map.count(variable_name)) {
		return m_variable_map[variable_name];
	}

	m_error_flags[E_UNKNOWN_INDEX] = UNKNOWN_VARIABLE;
	return "";
}


////////////////////////////////////////////////////////////////|
/// 
/// Processes the line
///		-> Extracts the variable names, values
///		-> If it refers to the build tool caps the value length
///			(Extra security)
///	 
/// Executes until the reading process isn't finished
/// 
/// Connects to the main thread (multi threaded)
/// 
/// Returns:
///		void
/// 
////////////////////////////////////////////////////////////////|
void qmake::VariableHandler::processLine()
{
	while (1) {
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// Wait until we have something to process
			m_conditional_variable.wait(lock, [this]() {
				return !m_variable_queue.empty() || m_reading_stopped;
			});
			
			if (m_reading_stopped && m_variable_queue.empty()) {
				break; // If the reading was stopped and the queue is empty we break the loop
			}
		}
		std::string line = m_variable_queue.front();
		m_variable_queue.pop();
		

		// Process the line
		std::string var_name, var_value;
		bool found_equal = false;
		for (auto c : line) {
			if (c == '=') {
				found_equal = true;
				continue;
			}
			if (found_equal) {
				var_value.push_back(c);
			}
			else if (c != '\t' && c != ' ') {
				var_name.push_back(c);
			}
		}

		// Check for compiler / assembler definition. Special symbol : #
		if (var_name.compare(BUILDER_CONSTANT) == 0) {
			var_value = var_value.substr(0, 12);
		}

		// Avoid duplication
		if (!m_variable_map.count(var_name)) {
			if (var_value.empty()) {
				m_error_flags[E_MISTYPE] = MISTYPED_VARIABLE;
			}
			m_variable_map[var_name] = var_value;
		}


		else {
			m_error_flags[E_DUPLICATION] = DUPLICATED_VARIABLE;
		}
		//printf("%s = %s\n", var_name.c_str(), var_value.c_str());
	}

}


/////////////////////////////////////////////////|
/// 
/// Adds the line to the pipeline for processing
/// 
/// Doesn't do any checks
/// 
/// Returns:
///		void
/// 
/////////////////////////////////////////////////| 
void qmake::VariableHandler::insertVariableLine(const std::string line)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex); // Lock for exclusive use
		m_variable_queue.push(line);
	}
	m_conditional_variable.notify_one(); // Notify that data process could start
}