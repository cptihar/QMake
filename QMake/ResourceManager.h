#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

namespace qmake {
	
	class ResourceManager {
	
		public:
			ResourceManager();
			~ResourceManager() = default;

			const bool sourceExists() const
			{	return m_resource_stream.is_open();	}

			std::ifstream& getResource();


		private: // Variables
			std::ifstream m_resource_stream;


		private: // Functions
			std::filesystem::path m_sourcePath(void);
	};
}


#endif // !RESOURCE_MANAGER_H