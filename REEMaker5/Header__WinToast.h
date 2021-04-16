/*
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
* ░▒▓    _    _      _                  __          ___    _______              _ 		▓▒░
* ░▒▓   | |  | |    | |                 \ \        / (_)  |__   __|            | | 		▓▒░
* ░▒▓   | |__| | ___| |_ __   ___ _ __   \ \  /\  / / _ _ __ | | ___   __ _ ___| |_ 	▓▒░
* ░▒▓   |  __  |/ _ \ | '_ \ / _ \ '__|   \ \/  \/ / | | '_ \| |/ _ \ / _` / __| __| 	▓▒░
* ░▒▓   | |  | |  __/ | |_) |  __/ |       \  /\  /  | | | | | | (_) | (_| \__ \ |_  	▓▒░
* ░▒▓   |_|  |_|\___|_| .__/ \___|_|        \/  \/   |_|_| |_|_|\___/ \__,_|___/\__| 	▓▒░
* ░▒▓				  | |																▓▒░
* ░▒▓        		  |_|																▓▒░
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
*  REEMaker 5 __ Grégory WENTZEL (c) 2021
*/

#pragma once
#include <string>
#include <wintoast/wintoastlib.h>
using namespace WinToastLib;
class CustomHandler : public IWinToastHandler {
public:
	void toastActivated() const {
		std::wcout << L"The user clicked in this toast" << std::endl;
		exit(0);
	}

	void toastActivated(int actionIndex) const {
		std::wcout << L"The user clicked on action #" << actionIndex << std::endl;
		exit(16 + actionIndex);
	}

	void toastDismissed(WinToastDismissalReason state) const {
		switch (state) {
		case UserCanceled:
			std::wcout << L"The user dismissed this toast" << std::endl;
			//exit(1);
			break;
		case TimedOut:
			std::wcout << L"The toast has timed out" << std::endl;
			//exit(2);
			break;
		case ApplicationHidden:
			std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
			//exit(3);
			break;
		default:
			std::wcout << L"Toast not activated" << std::endl;
			//exit(4);
			break;
		}
	}

	void toastFailed() const {
		std::wcout << L"Error showing current toast" << std::endl;
		//exit(5);
	}
};
enum Results {
	ToastClicked,                                   // user clicked on the toast
	ToastDismissed,                                 // user dismissed the toast
	ToastTimeOut,                                   // toast timed out
	ToastHided,                                             // application hid the toast
	ToastNotActivated,                              // toast was not activated
	ToastFailed,                                    // toast failed
	SystemNotSupported,                             // system does not support toasts
	UnhandledOption,                                // unhandled option
	MultipleTextNotSupported,               // multiple texts were provided
	InitializationFailure,                  // toast notification manager initialization failure
	ToastNotLaunched                                // toast could not be launched
};