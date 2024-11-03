#include "ResourceManager.h"


qmake::ResourceManager::ResourceManager()
	:m_resource_stream(std::ifstream(m_sourcePath(), std::ios::in))
{
}


/////////////////////////////////////|
/// 
/// Returns the loaded file
/// 
/// Return:
///		const std::string (the line)
/// 
/////////////////////////////////////|
std::ifstream& qmake::ResourceManager::getResource()
{
	return m_resource_stream;
}


///////////////////////////////////////////////////////|
/// 
///	Fetches the path from where the program was invoked
/// Attempts to open the file
/// 
/// Called by:
///		-> ResourceManager constructor
/// 
/// Returns:
///		std::filesystem::path
/// 
///////////////////////////////////////////////////////|
std::filesystem::path qmake::ResourceManager::m_sourcePath(void)
{
	auto path = std::filesystem::current_path();
	path += std::filesystem::path("\\QMake.qm");

	return path;
}
