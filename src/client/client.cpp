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
		choice = InputEnum::userInput(user_input); // TODO handle invalid input
		if (choice == InputEnum::exitApp) // TODO check if can be done at handleInput
			break;
		else
		{
			app.handleInput(choice);
			// resp = app.handleInput(choice);
			if (resp.code == int(responseCode::generalError))
			{
				std::cout << "server responded with an error" << std::endl;
			}
		}
	}
}
