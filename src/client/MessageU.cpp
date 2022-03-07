#include "MessageU.h"

MessageU::MessageU()
{
	std::string server_address = FileUtils::fileToString(SERVER_CONFIG_PATH);
	int split_index = server_address.find(":");
	if (split_index == std::string::npos) {
		throw std::invalid_argument(SERVER_CONFIG_PATH + " not in the right format");
	}
	serverIp = server_address.substr(0, split_index);
	std::string port_str = server_address.substr(split_index + 1, server_address.length() - 1);
	serverPort = std::stoi(port_str);
	try
	{
		std::string myinfo = FileUtils::fileToString(USER_INFO_PATH);
		std::string b64private_key_str, client_id_str;

		std::stringstream ss(myinfo);
		std::getline(ss, username, '\n');
		std::getline(ss, client_id_str, '\n');
		ss << ss.rdbuf();
		b64private_key_str = ss.str();
		client_id = StringUtils::StrToUuid(client_id_str);
		std::string private_key_str = Base64Wrapper::decode(b64private_key_str);
		privateKey = new RSAPrivateWrapper(private_key_str);
		std::string publicKey_str = privateKey->getPublicKey();
		//std::string b64publicKey_str = Base64Wrapper::encode(publicKey_str);
		publicKey = new RSAPublicWrapper(publicKey_str);
	}
	catch (const std::exception&)
	{
		std::cout << "can't find " << USER_INFO_PATH << std::endl;
		privateKey = NULL;
		publicKey = NULL;
	}
}

Response MessageU::handleInput(InputEnum::userInput choice)
{
	Response(resp);
	try
	{
		switch (choice)
		{
		case InputEnum::userInput::registertion:
		{
			if (FileUtils::fileExist(USER_INFO_PATH))
			{
				throw std::exception("you are already registered (user info file already exist)");
			}
			std::string username;
			std::cout << "enter client username: ";
			std::cin.ignore(); // flushing the input buffer
			std::getline(std::cin, username);
			if (username.length() > MAX_USERNAME_LENGTH - 1)
			{
				throw std::exception("username is to big");
			}
			resp = registerUser(username);
			break;
		}
		case InputEnum::userInput::clientsList:
			resp = getCLientList();
			break;
		case InputEnum::userInput::getPubKey:
		{
			ClientInfo* user_info = getClientInput();
			resp = getPubKey(user_info);
			break;
		}
		case InputEnum::userInput::getMessages:
		{
			resp = getMessages();
			break;
		}
		case InputEnum::userInput::sendMessage:
		{
			ClientInfo* dst_client = getClientInput();
			AESWrapper* sym_key = dst_client->getSymKey();
			if (sym_key == NULL)
			{
				throw std::exception("can�t decrypt message");
			}
			std::string text_message;
			std::cout << "enter text: ";
			std::getline(std::cin, text_message);
			std::string text_message_encrypted = sym_key->encrypt(text_message.c_str(), text_message.length());
			resp = sendMessage(dst_client, messageType::sendText, text_message_encrypted);
			break;
		}
		case InputEnum::userInput::requestSymKey:
		{
			ClientInfo* dst_client = getClientInput();
			resp = sendMessage(dst_client, messageType::requestSymKey, "");
			break;
		}
		case InputEnum::userInput::sendSymKey:
		{
			ClientInfo* dst_client = getClientInput();
			unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
			AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH);
			dst_client->setSymKey(key);
			if (dst_client->getPubKey() == NULL)
			{
				std::string err_msg = "unknown public key for '" + dst_client->getUserName() + "' please request public key";
				throw std::exception(err_msg.c_str());
			}
			std::string sym_key = dst_client->getPubKey()->encrypt((const char*)key, AESWrapper::DEFAULT_KEYLENGTH);
			resp = sendMessage(dst_client, messageType::sendSymKey, sym_key);
			break;
		}
		case InputEnum::userInput::sendFile:
		{
			ClientInfo* dst_client = getClientInput();
			AESWrapper* sym_key = dst_client->getSymKey();
			if (sym_key == NULL)
			{
				throw std::exception("can't decrypt message");
			}
			std::string file_path;
			std::cout << "enter file path: ";
			//std::cin.ignore(); // flushing the input buffer
			std::getline(std::cin, file_path);
			std::string file_content = FileUtils::fileToString(file_path);
			std::string file_content_encrypted = sym_key->encrypt(file_content.c_str(), file_content.length());
			resp = sendMessage(dst_client, messageType::sendFile, file_content_encrypted);
			break;
		}
		case InputEnum::userInput::exitApp:
			// TODO free some memory
			break;
		default:
			std::cout << "Unkown option: " << int(choice) << std::endl;
			break;
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "some error occured wile handling " << int(choice) << " request:" << std::endl;
		std::cout << ex.what() << std::endl << std::endl;
	}
	return resp;
}

void MessageU::printMenu() {
	std::cout << WELCOME_MESSAGE << std::endl << std::endl;
	for (const auto option : InputEnum::All)
	{
		std::cout << int(option) << ") " << optionToText(option) << std::endl;
	}
	std::cout << "?" << std::endl;
}

std::string MessageU::optionToText(InputEnum::userInput option)
{
	switch (option)
	{
	case InputEnum::userInput::registertion: return "Register";
	case InputEnum::userInput::clientsList: return "Request for clients list";
	case InputEnum::userInput::getPubKey: return "Request for public key";
	case InputEnum::userInput::getMessages: return "Request for waiting messages";
	case InputEnum::userInput::sendMessage: return "Send a text message";
	case InputEnum::userInput::requestSymKey: return "Send a request for symmetric key";
	case InputEnum::userInput::sendSymKey: return "Send your symmetric key";
	case InputEnum::userInput::sendFile: return "Send a file";
	case InputEnum::userInput::exitApp: return "Exit client";
	default: return "invalid option";
	}
}

Response MessageU::registerUser(std::string userName)
{
	Request req = Request(UUID(), VERSION, requestCode::registertion, MAX_USERNAME_LENGTH + RSAPublicWrapper::KEYSIZE);
	strncpy_s(req.getPayload(), userName.length() + 1, userName.c_str(), userName.length() + 1);
	privateKey = new RSAPrivateWrapper();
	std::string private_key_str = privateKey->getPrivateKey();
	std::string pubkey_str = privateKey->getPublicKey();
	publicKey = new RSAPublicWrapper(pubkey_str);
	std::memcpy(&req.getPayload()[MAX_USERNAME_LENGTH], pubkey_str.c_str(), RSAPublicWrapper::KEYSIZE);
	Response resp = req.sendRequset(serverIp, serverPort);
	if (resp.getCode() == responseCode::registertion)
	{
		username = userName;
		std::memcpy(&client_id, resp.getPayload(), sizeof(UUID));
		std::string client_id_str = StringUtils::UuidToStr(client_id);
		std::ofstream myfile;
		myfile.open(USER_INFO_PATH);
		myfile << userName << std::endl << client_id_str << std::endl << Base64Wrapper::encode(private_key_str);
		myfile.close();
	}
	
	return resp;
}
Response MessageU::getCLientList()
{
	Request req = Request(client_id, VERSION, requestCode::users_list, 0);
	Response resp = req.sendRequset(serverIp, serverPort);

	UUID user_uuid;
	std::cout << "ID" << std::string(30, ' ') << " | username" << std::endl;
	std::cout << std::string(43, '-') << std::endl;
	for (uint32_t i = 0; i < resp.getPayloadSize(); i += MAX_USERNAME_LENGTH + sizeof(UUID))
	{
		std::memcpy(&user_uuid, &resp.getPayload()[i], sizeof(UUID));
		std::string user_uuid_str = StringUtils::UuidToStr(user_uuid);
		std::cout << user_uuid_str << " | ";
		std::string user_username(&resp.getPayload()[i + sizeof(UUID)]);
		std::cout << user_username << std::endl;
		clientsIdToUsername[user_uuid_str] = user_username;
		if (clients[user_username] == NULL)
		{
			clients[user_username] = new ClientInfo();
			clients[user_username]->setUserName(user_username);
			clients[user_username]->setId(user_uuid);
		}
	}
	return resp;
}
Response MessageU::getPubKey(ClientInfo* user_info)
{
	UUID dst_client_id = user_info->getId();
	Request req = Request(client_id, VERSION, requestCode::getPubKey, sizeof(dst_client_id));
	std::memcpy(req.getPayload(), &dst_client_id, sizeof(dst_client_id));
	Response resp = req.sendRequset(serverIp, serverPort);
	if (resp.getCode() == responseCode::getPubKey)
	{
		RSAPublicWrapper* dst_pubkey = new RSAPublicWrapper(&resp.getPayload()[sizeof(UUID)], RSAPublicWrapper::KEYSIZE);
		std::cout << std::endl << "the public key is:" << std::endl;
		std::cout << Base64Wrapper::encode(dst_pubkey->getPublicKey());
		user_info->setPubKey(dst_pubkey);
	}
	return resp;
}
Response MessageU::getMessages()
{
	Request req = Request(client_id, VERSION, requestCode::getMessages, 0);
	Response resp = req.sendRequset(serverIp, serverPort);

	std::vector<Message> msgs = Message::ReadMessages(resp.getPayload(), resp.getPayloadSize());
	for (Message msg : msgs)
	{
		ClientInfo* user_info = clients[clientsIdToUsername[StringUtils::UuidToStr(msg.getToClientId())]];
		if (user_info == NULL)
		{
			std::string err_msg = "unknown user '" + StringUtils::UuidToStr(msg.getToClientId()) + "' please get clients list";
			std::cout << err_msg << std::endl;
		}
		else
		{
			std::cout << "From: " << user_info->getUserName() << std::endl;
			std::cout << "Content:" << std::endl;
			switch (msg.getMessageType())
			{
			case messageType::sendFile:
			{
				std::string file_content = user_info->getSymKey()->decrypt(msg.getContent(), msg.getMessageSize());
				char temp_path[L_tmpnam_s];
				tmpnam_s(temp_path, L_tmpnam_s);
				FileUtils::stringToFile(file_content, std::string(temp_path));
				std::cout << "file saved in path: " << temp_path << std::endl;
				break;
			}
			case messageType::requestSymKey:
				std::cout << "Request for symmetric key" << std::endl;
				break;
			case messageType::sendSymKey:
			{
				std::cout << "symmetric key received" << std::endl;
				std::string symkey = privateKey->decrypt(msg.getContent(), msg.getMessageSize());
				user_info->setSymKey((unsigned char*)symkey.c_str());
				break;
			}
			case messageType::sendText:
				std::cout << user_info->getSymKey()->decrypt(msg.getContent(), msg.getMessageSize()) << std::endl;
				break;
			default:
				break;
			}
			std::cout << "-----<EOM>-----" << std::endl << std::endl;
		}
	}
	return resp;
}
Response MessageU::sendMessage(ClientInfo* user_info, messageType message_type, std::string content)
{
	Message msg = Message(user_info->getId(), message_type, content.size(), (char*)content.c_str());
	Request req = Request(client_id, VERSION, requestCode::sendMessage, msg.getSizeBytes(), msg.getBytes());
	Response resp = req.sendRequset(serverIp, serverPort);
	return resp;
}

ClientInfo* MessageU::getClientInput()
{
	std::string client_username;
	std::cout << "enter client username: ";
	std::cin.ignore(); // flushing the input buffer
	std::getline(std::cin, client_username);
	ClientInfo* dst_client = clients[client_username];
	if (dst_client == NULL)
	{
		std::string err_msg = "unknown client '" + client_username + "' please get clients list";
		throw std::exception(err_msg.c_str());
	}
	return dst_client;
}
