#pragma once
#include <Windows.h>
#include <vector>

enum class messageType {
	requestSymKey = 1,
	sendSymKey = 2,
	sendText = 3,
	sendFile = 4,
};

class Message
{
private:
	UUID _to_client_id;
	uint32_t _message_id;
	uint8_t _message_type_id;
	uint32_t _message_size;
	char* _content = NULL;
public:
	static std::vector<Message> ReadMessages(char* bytes, uint32_t size);
	Message(UUID user_uuid, uint32_t message_id, uint8_t message_type_id, uint32_t message_size, char* content = NULL);
	Message(UUID user_uuid, messageType message_type_id, uint32_t message_size, char* content = NULL);
	char* getBytes();
	uint32_t getSizeBytes();

	void setContent(char* cont)
	{
		_content = cont;
	}

	char* getContent()
	{
		return _content;
	}

	UUID getToClientId()
	{
		return _to_client_id;
	}

	void setToClientId(UUID id)
	{
		_to_client_id = id;
	}

	uint32_t getMessageId()
	{
		return _message_id;
	}

	void setMessageId(uint32_t id)
	{
		_message_id = id;
	}

	messageType getMessageType()
	{
		return messageType(_message_type_id);
	}

	void setMessageType(uint8_t type)
	{
		_message_type_id = type;
	}

	void setMessageType(messageType type)
	{
		setMessageType(uint8_t(type));
	}

	void setMessageSize(uint32_t size)
	{
		_message_size = size;
	}

	uint32_t getMessageSize()
	{
		return _message_size;
	}
};
