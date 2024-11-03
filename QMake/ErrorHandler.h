#ifndef QMAKE_ERROR_H
#define QMAKE_ERROR_H

#include <stdio.h>
#include <stdlib.h>

#define MSG_RESOURCE_NOT_FOUND			"QMake.qm file was not found. Consider adding a QMake.qm file."
#define MSG_ARGUMENT_NOT_GIVEN			"No command line argument was given."
#define MSG_DUPLICATED_VAR				"Duplicated variable was detected."
#define MSG_UNKNOWN_VAR					"Unknown variable was detected."
#define MSG_MISTYPED_VAR				"Mistyped variable was detected."
#define MSG_LABEL_NO_EXIST				"The provided label does not exist."
#define MSG_NO_BUILDER_DEFINED			"No build tool was defined. Add #BUILDER = your tool"
#define MSG_NOT_ENOUGH_ENV_ARGUMENTS	"Not enough enviroment variables to start an embedded execution."


#define UNEXPECTED_ERROR() printf("[QMake error] Unexpected problem occured.")

namespace qmake {
	
	static struct SimpleVector {
		unsigned short* buffer;
		unsigned short size;
		unsigned short capacity;
	};
	
	class ErrorHandler {
		public:
			ErrorHandler();
			~ErrorHandler();

			void		report_errors() const;
			const bool	shouldExit() const
			{	return m_exit_immediately;	}

			const bool flagAlreadySet(const unsigned short code) const
			{	return (m_error_flags & code) == code;	}

			void		addMessage(const unsigned short error_flag);
			const bool	noMessages() const
			{	return m_error_messages.size == 0;	}

		private: // Variables
			struct SimpleVector	m_error_messages;
			unsigned short		m_error_flags;
			bool				m_exit_immediately;
			char				m_message_table[8][100] =
			{MSG_RESOURCE_NOT_FOUND, MSG_ARGUMENT_NOT_GIVEN, MSG_DUPLICATED_VAR,
				MSG_UNKNOWN_VAR, MSG_MISTYPED_VAR, MSG_LABEL_NO_EXIST, 
				MSG_NO_BUILDER_DEFINED, MSG_NOT_ENOUGH_ENV_ARGUMENTS};
 
		private: // Functions
			bool				m_initErrorMessages();
			void				m_pushBackMessage(unsigned short message_code);
			void				m_memCpy(unsigned short* dst, const unsigned short* src, unsigned short size);
			void				m_resizeVector(void);
			unsigned short		m_calculateOffset(unsigned short value);
	};
}
#endif // !QMAKE_ERROR_H