#include "ErrorHandler.h"

qmake::ErrorHandler::ErrorHandler()
	:m_error_flags(0), m_exit_immediately(false)
{
	if (m_initErrorMessages() == false) {
		m_exit_immediately = true;
	}
}

qmake::ErrorHandler::~ErrorHandler()
{
	// Cleanup
	free(m_error_messages.buffer);
	m_error_messages.buffer = NULL;
	m_error_messages.capacity = 0;
	m_error_messages.size = 0;
}


/////////////////////////////////////////|
///
///	Prints out all the errors
///	Doesn't need to check for NUll stuff
/// 
///	Return:
///		void
/// 
/////////////////////////////////////////|
void qmake::ErrorHandler::report_errors() const
{
	if (m_error_messages.buffer != NULL) {
		if (m_error_messages.size == 0) {
			printf("\x1B[1m\x1B[91m[QMake]\033[0m Execution was \x1B[1m\x1B[92msuccessful\033[0m.\n");
			return;
		}

		printf("\x1B[1m\x1B[91m[QMake]\033[0m Execution \x1B[1m\x1B[91mfailed\033[0m.\n");
		for (unsigned char i = 0; i < m_error_messages.size; ++i) {
			printf("\x1B[1m\x1B[91m[QMake]\033[0m %s\n", m_message_table[m_error_messages.buffer[i]]);
		}

	}
}

///////////////////////////////////////////////////|
/// 
/// External function for error message pushbacks
/// 
/// Return:
///		void
///
///////////////////////////////////////////////////|
void qmake::ErrorHandler::addMessage(const unsigned short error_flag)
{
	// Check whether the error exists
	if (flagAlreadySet(error_flag)) {
		return;
	}

	m_error_flags |= error_flag;

	// Calculate simple hash
	unsigned short hash_value = m_calculateOffset(error_flag);
	m_pushBackMessage(hash_value);
}


/////////////////////////////////////////
/// 
/// Initializes the errorMessages C style vector
/// 
/// Returns:
///		false = malloc fucked up
///		 true = all good
/// 
/////////////////////////////////////////
bool qmake::ErrorHandler::m_initErrorMessages()
{
	m_error_messages.buffer = (unsigned short*)malloc(sizeof(unsigned short) * 2);
	if (m_error_messages.buffer == NULL) {
		UNEXPECTED_ERROR();
		m_exit_immediately = true;
		return false;
	}
	m_error_messages.capacity = 2;
	m_error_messages.size = 0;
	return true;
}


/////////////////////////////////////|
///
/// Handles message push back logic
/// 
/// Return:
///		void
/// 
/////////////////////////////////////|
void qmake::ErrorHandler::m_pushBackMessage(unsigned short message_code)
{
	if (m_error_messages.size >= m_error_messages.capacity) {
		m_resizeVector();
	}

	if (m_exit_immediately) {
		return;
	}
	
	
	// Copy the message
	m_error_messages.buffer[m_error_messages.size] = message_code;
	m_error_messages.size++;
}


/////////////////////////////////////////////////////////|
///
///	Resizes the C style vector
///	Handles necessary edge cases
/// 
/// If necessary signals that execution should be stopped
/// 
/// Return:
///		void
///
/////////////////////////////////////////////////////////|
void qmake::ErrorHandler::m_resizeVector(void)
{
	// Create new buffer
	unsigned short new_capacity = m_error_messages.capacity * 2;
	unsigned short* new_buffer = (unsigned short*)malloc(sizeof(unsigned short) * new_capacity);

	if (new_buffer == NULL) {
		UNEXPECTED_ERROR();
		m_exit_immediately = true;
		return;
	}

	// memcopy
	m_memCpy(new_buffer, m_error_messages.buffer, m_error_messages.size);

	// Swap pointers
	free(m_error_messages.buffer);
	m_error_messages.buffer = new_buffer;
	m_error_messages.capacity = new_capacity;
}


////////////////////////////////////|
///
/// Simple implementation of memcpy
/// Checks necessary edge cases
/// 
/// Returns:
///		void
///
////////////////////////////////////|
void qmake::ErrorHandler::m_memCpy(unsigned short* dst, const unsigned short* src, unsigned short size)
{
	if (dst == NULL || src == NULL) {
		UNEXPECTED_ERROR();
		m_exit_immediately = true;
		return;
	}

	unsigned short counter = 0;
	while (counter < size) {
		dst[counter] = src[counter];
		++counter;
	}
}

//////////////////////////////////////////////|
///
/// Calculates the message index in the table
/// Acts as a low budge hashing algorithm
///
/// Returns:
///		unsigned short (aka the value
/// 
//////////////////////////////////////////////|
unsigned short qmake::ErrorHandler::m_calculateOffset(unsigned short value)
{
	unsigned short offset = 0;
	while (value >>= 1) {
		offset++;
	}
	return offset;
}