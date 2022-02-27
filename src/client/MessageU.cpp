#include "MessageU.h"

MessageU::MessageU()
{
	std::string server_address = fileToString(SERVER_CONFIG_PATH);
	std::cout << server_address << std::endl;
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
		//RSAPrivateWrapper privateKey(myinfo); // TODO split myinfo
		//RSAPublicWrapper publicKey(privateKey.getPublicKey());
		privateKey = new RSAPrivateWrapper(myinfo); // TODO split myinfo
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
	//std::cout << choice << " was chosen" << std::endl;
	Response(resp);
	switch (choice)
	{
	case InputEnum::registertion:
	{
		std::string username;
		std::cout << "enter username: ";
		std::cin >> username;
		resp = registerUser(username);
		break;
	}
	case InputEnum::clientsList:
		resp = getCLientList();
		break;
	case InputEnum::getPubKey:
		resp = getPubKey();
		break;
	case InputEnum::getMessages:
		resp = getMessages();
		break;
	case InputEnum::sendMessage:
		resp = sendMessage();
		break;
	case InputEnum::requestSymKey:
		resp = requestSymKey();
		break;
	case InputEnum::sendSymKey:
		resp = requestSymKey();
		break;
	case InputEnum::exitApp:
		break;
	default:
		std::cout << "Unkown oprion: " << choice << std::endl;
		break;
	}
	return resp;
}

void MessageU::printMenu() {
	std::cout << WELCOME_MESSAGE << std::endl << std::endl;
	for (const auto option : InputEnum::All)
	{
		std::cout << option << ") " << optionToText(option) << std::endl;
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
	if (fileExist(USER_INFO_PATH))
	{
		throw std::exception("user info file already exist");
	}
	Request(req);
	req.version = VERSION;
	req.code = uint16_t(requestCode::registertion);
	req.client_id = UUID(); // TODO chhose defualt value
	if (userName.length() > MAX_USERNAME_LENGTH - 1)
	{
		throw std::exception("username ist to big");
	}
	req.payload_size = MAX_USERNAME_LENGTH + PUBLIC_KEY_SIZE;
	req.payload = new char[req.payload_size]();
	strncpy_s(req.payload, req.payload_size, userName.c_str(), userName.length() + 1);
	privateKey = new RSAPrivateWrapper();
	std::string private_key = privateKey->getPrivateKey();
	std::string pubkey = privateKey->getPublicKey();
	publicKey = new RSAPublicWrapper(pubkey);
	std::memcpy(&req.payload[MAX_USERNAME_LENGTH], pubkey.c_str(), PUBLIC_KEY_SIZE);
	//strncpy_s(&req.payload[MAX_USERNAME_LENGTH], PUBLIC_KEY_SIZE, pubkey.c_str(), pubkey.length());
	return req.sendRequset(serverIp, serverPort);
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