#include "Execute.h"

qmake::Execute::Execute(char** argv, int argc)
	:m_error_handler(qmake::ErrorHandler()), m_executable_string(""),
	m_should_execute(false), r_argv(argv), m_argc(argc), m_build_tool("")
{
	if (argc < 2) {
		m_error_handler.addMessage(ARGUMENT_MISSING);
	}
}

qmake::Execute::~Execute()
{
	m_error_handler.report_errors();
}


///////////////////////////////////////////////////////////////////////////////|
/// 
///	This function is responsible for making the preparations for the execution
///		-> Loads the variables
///		-> Finds the user defined label
///			-> If found, starts processing it
/// 
/// Multi threaded:
///		-> Variable processing and Label processing run on seperate threads
///	
/// If an error occured does the necessary administrative tasks
/// 
/// Returns:
///		void
///
///////////////////////////////////////////////////////////////////////////////|
void qmake::Execute::prepare()
{
	bool					label_active = false;
	bool					reading_stopped = false;
	bool					label_found = false;
	qmake::ResourceManager	rm;
	qmake::VariableHandler	vh(reading_stopped);
	qmake::LabelHandler		lh(reading_stopped);
	
	if (m_error_handler.flagAlreadySet(ARGUMENT_MISSING)) {
		return;
	}

	if (!rm.sourceExists()) {
		m_error_handler.addMessage(RESOURCE_MISSING);
		return;
	}


	std::string line;
	std::thread t_variable_process	(&qmake::VariableHandler::processLine, &vh);
	std::thread lh_variable_process	(&qmake::LabelHandler::processLabel, &lh);
	while (std::getline(rm.getResource(), line)) {

		line = m_trimLine(line);
		// Check if its a variable (=)
		if (m_strContains(line, '=')) {
			vh.insertVariableLine(line);
			label_active = false;
		}

		// Check if its a label (:)

		else if (m_strContains(line, ':')) {
			
			unsigned long begins_pos = m_findFirstNormalCharacter(line);
			auto colon_position = line.find_first_of(':');
			std::string label_name = line.substr(begins_pos, colon_position);

			if (label_name == r_argv[1]) {
				// push for process
				label_active = true;
				label_found = true;
			}

			else {
				label_active = false;
			}
		}

		// Push for proccess
		else if (line.size() > 1 && line[line.size() - 1] == '|' && label_found && label_active) {
			lh.pushForProcess(line.substr(0, line.size()-1));
		}
	}


	{
		std::lock_guard<std::mutex> lock(vh.getMutex());
		std::lock_guard<std::mutex> lock1(lh.getMutex());
	}

	reading_stopped = true;
	vh.getConditionalVariable().notify_all();
	lh.getCv().notify_all();
	t_variable_process.join();
	lh_variable_process.join();

	// Extract variable errors
	m_extractFlags(vh.getFlags(), TOTAL_E_FLAGS);

	// Check whether the label was valid
	if (!label_found) {
		m_error_handler.addMessage(NONEXISTANT_LABEL);
	}

	// Check whether a build tool was provided
	m_build_tool = vh.fetchVariableValue(BUILDER_CONSTANT);
	if (m_build_tool.empty()) {
		m_error_handler.addMessage(NOBUILDER_DEFINED);
	}
	
	for (auto& var : lh.getVariables()) {
		std::string var_value = vh.fetchVariableValue(var);

		if (var_value.empty()) {
			m_error_handler.addMessage(UNKNOWN_VARIABLE);
		}
		else {
			lh.insertVariable(var_value, var);
		}
	}
	
	m_executable_string = lh.getContents();
	m_should_execute = (m_error_handler.noMessages());
}


////////////////////////////////////////////|
/// 
/// If the preparation phase was sucessful:
///		-> Executes the code
///
/// Returns: void
/// 
////////////////////////////////////////////|
void qmake::Execute::execute()
{
	if (!m_should_execute) {
		return;
	}

	m_build_tool += ( ' ' + m_executable_string);
	system(m_build_tool.c_str());
}


//////////////////////////////////////////|
/// 
///  Finds the first character that is not 
///		-> ' '
///		-> '\t'
/// 
///  Returns:
///		unsigned long (the index)
/// 
//////////////////////////////////////////|
unsigned long qmake::Execute::m_findFirstNormalCharacter(const std::string& string)
{
	unsigned long position = 0;

	while (position < string.length()) {

		if (string[position] != ' ' && string[position] != '\t')
			break;
		++position;
	}
	
	return position;
}

/////////////////////////////////////////////////////////|
///
/// Check whether a string contains a specific character
/// 
/// Returns: bool
///	
/////////////////////////////////////////////////////////|
bool qmake::Execute::m_strContains(const std::string& str, const char character)
{

	for (auto c : str) {
		if (c == character)
			return true;
	}
	return false;
}


//////////////////////////////////////|
/// 
///	Helper for error flag extractions
///
/// Returns: void
/// 
//////////////////////////////////////|
void qmake::Execute::m_extractFlags(const unsigned short* arr, unsigned int size)
{
	for (unsigned int i = 0; i < size; ++i) {
		if (arr[i] != 0) {
			m_error_handler.addMessage(arr[i]);
		}
	}
}


//////////////////////////////////////////////////|
/// 
///	Trims additional tabs and spaces from the end
/// 
///	Returns:
///		std::string (trimmed string)
///
//////////////////////////////////////////////////|
std::string qmake::Execute::m_trimLine(const std::string& str)
{
	auto start = str.find_first_not_of(" \t");
	auto end = str.find_last_not_of(" \t");
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
