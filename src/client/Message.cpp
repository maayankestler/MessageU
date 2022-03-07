#include "Message.h"

std::vector<Message> Message::ReadMessages(char* bytes, uint32_t size)
{
	std::vector<Message> msgs;
	UUID user_uuid;
	uint32_t message_id;
	uint8_t message_type_id;
	messageType message_type;
	uint32_t message_size;
	char* content = NULL;
	uint32_t i = 0;
	while (i < size)
	{
		std::memcpy(&user_uuid, &bytes[i], sizeof(user_uuid));
		i += sizeof(user_uuid);
		std::memcpy(&message_id, &bytes[i], sizeof(message_id));
		i += sizeof(message_id);
		std::memcpy(&message_type_id, &bytes[i], sizeof(message_type_id));
		i += sizeof(message_type_id);
		message_type = messageType(message_type_id);
		std::memcpy(&message_size, &bytes[i], sizeof(message_size));
		i += sizeof(message_size);
		if (message_size > 0)
		{
			content = new char[message_size]();
			std::memcpy(content, &bytes[i], message_size);
			i += message_size;
		}
		else
		{
			content = NULL;
		}
		Message msg = Message(user_uuid, message_id, message_type_id, message_size, content);
		msgs.push_back(msg);
	}
	return msgs;
}

Message::Message(UUID to_client_id, uint32_t message_id, uint8_t message_type_id, uint32_t message_size, char* content = NULL)
{
	setToClientId(to_client_id);
	setMessageId(message_id);
	setMessageType(message_type_id);
	setMessageSize(message_size);
	content = content;
}

Message::~Message()
{
	delete content;
}
