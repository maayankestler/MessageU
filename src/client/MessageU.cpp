#include "MessageU.h"

MessageU::MessageU()
{
	std::string server_address = fileToString(SERVER_CONFIG_PATH);
	int split_index = server_address.find(":");
	if (split_index == std::string::npos) {
		throw std::invalid_argument(SERVER_CONFIG_PATH + " not in the right format");
	}
	serverIp = server_address.substr(0, split_index);
	std::string port_str = server_address.substr(split_index + 1, server_address.length() - 1);
	serverPort = std::stoi(port_str);
	//RSAPrivateWrapper privateKey(""); // TODO better default value (null)
	//RSAPublicWrapper publicKey(privateKey.getPublicKey());
	
	try
	{
		std::string myinfo = fileToString(USER_INFO_PATH);
		std::string private_key_str, client_id_str;

		std::stringstream ss(myinfo);
		std::getline(ss, username, '\n');
		std::getline(ss, client_id_str, '\n');
		ss << ss.rdbuf();
		private_key_str = ss.str();

		// add "-" to match the UuidFromString convention
		for (int i=8;i<24;i+=5)
			client_id_str.insert(i, "-");
		UuidFromStringA((RPC_CSTR)client_id_str.c_str(), &client_id);
		std::string b64private_key_str = Base64Wrapper::decode(private_key_str);
		privateKey = new RSAPrivateWrapper(b64private_key_str);
		publicKey = new RSAPublicWrapper(privateKey->getPublicKey());
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
			if (fileExist(USER_INFO_PATH))
			{
				throw std::exception("you are already registered (user info file already exist)");
			}
			std::string username;
			std::cout << "enter username: ";
			std::cin >> username;
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
			resp = getPubKey();
			break;
		case InputEnum::userInput::getMessages:
			resp = getMessages();
			break;
		case InputEnum::userInput::sendMessage:
			resp = sendMessage();
			break;
		case InputEnum::userInput::requestSymKey:
			resp = requestSymKey();
			break;
		case InputEnum::userInput::sendSymKey:
			resp = requestSymKey();
			break;
		case InputEnum::userInput::exitApp:
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
	std::string text = "";
	switch (option)
	{
	case InputEnum::userInput::registertion:
		text = "Register";
		break;
	case InputEnum::userInput::clientsList:
		text = "Request for clients list";
		break;
	case InputEnum::userInput::getPubKey:
		text = "Request for public key";
		break;
	case InputEnum::userInput::getMessages:
		text = "Request for waiting messages";
		break;
	case InputEnum::userInput::sendMessage:
		text = "Send a text message";
		break;
	case InputEnum::userInput::requestSymKey:
		text = "Send a request for symmetric key";
		break;
	case InputEnum::userInput::sendSymKey:
		text = "Send your symmetric key";
		break;
	case InputEnum::userInput::exitApp:
		text = "Exit client";
		break;
	default:
		text = "invalid option";
		break;
	}
	return text;
}

Response MessageU::registerUser(std::string userName)
{
	Request(req);
	req.version = VERSION;
	req.code = uint16_t(requestCode::registertion);
	req.client_id = UUID(); // TODO chhose defualt value
	req.payload_size = MAX_USERNAME_LENGTH + PUBLIC_KEY_SIZE;
	req.payload = new char[req.payload_size]();
	strncpy_s(req.payload, userName.length() + 1, userName.c_str(), userName.length() + 1);
	privateKey = new RSAPrivateWrapper();
	std::string private_key_str = privateKey->getPrivateKey();
	std::string pubkey_str = privateKey->getPublicKey();
	publicKey = new RSAPublicWrapper(pubkey_str);
	std::memcpy(&req.payload[MAX_USERNAME_LENGTH], pubkey_str.c_str(), PUBLIC_KEY_SIZE);
	Response resp = req.sendRequset(serverIp, serverPort);
	if (resp.code == int(responseCode::registertion))
	{
		username = userName;
		std::memcpy(&client_id, resp.payload, sizeof(UUID));
		std::string client_id_str = UuidStr(client_id);
		std::ofstream myfile;
		myfile.open(USER_INFO_PATH);
		myfile << userName << std::endl << client_id_str << std::endl << Base64Wrapper::encode(private_key_str);
		myfile.close();
	}
	
	return resp;
}
Response MessageU::getCLientList()
{
	return Response();
}
Response MessageU::getPubKey()
{
	return Response();
}
Response MessageU::getMessages()
{
	return Response();
}
Response MessageU::sendMessage()
{
	return Response();
}
Response MessageU::requestSymKey()
{
	return Response();
}
Response MessageU::sendSymKey()
{
	return Response();
}

// TODO think where to put this func and add comments
std::string MessageU::hexStr(unsigned char* data, int len)
{
	std::stringstream ss;
	ss << std::hex;
	for (int i = 0; i < len; ++i)
		ss << std::setw(2) << std::setfill('0') << (int)data[i];
	return ss.str();
}

// TODO think where to put this func
std::string MessageU::UuidStr(UUID uuid)
{
	std::stringstream ss;
	ss << std::hex << uuid.Data1 << uuid.Data2 << uuid.Data3;
	ss << hexStr(uuid.Data4, sizeof(uuid.Data4));
	return ss.str();
}
