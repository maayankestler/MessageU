#include <iostream>
#include "Protocol.h"
#include "MessageU.h"

int main()
{
	short user_input;
	InputEnum::userInput choice;
	MessageU(app);
	Response(resp);
	while (true)
	{
		app.printMenu();
		std::cin >> user_input;
		std::cout << std::endl;
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore();
			std::cout << "got invalid input, please try again" << std::endl << std::endl;
		}
		else
		{
			choice = InputEnum::userInput(user_input);
			resp = app.handleInput(choice);
			if (resp.code == int(responseCode::generalError))
			{
				std::cout << "server responded with an error" << std::endl;
			}
			if (choice == InputEnum::userInput::exitApp)
				break;
		}
		std::cout << std::endl;
	}
}
