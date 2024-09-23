#pragma once

#include <vector>
#include <unordered_map>
#include <ini.h>


namespace GameEngine
{
	enum KeyboardButtonState {
		PRESSED = 0,
		NOT_PRESSED,
		UNDEFINED
	};

	enum KeyboardButtonId {
		A = 0,
		D,
		E,
		S,
		Q,
		W
	};

	struct KeyboardButtons {
		std::unordered_map<KeyboardButtonId, KeyboardButtonState> button_states;
		KeyboardButtonState get(KeyboardButtonId button_id) {
			if (button_states.find(button_id) == button_states.end()) return KeyboardButtonState::UNDEFINED;
			return button_states.at(button_id);
		}
	};

	class KeyboardBindings {
	private:
		std::vector<KeyboardButtonId> forward;
		std::vector<KeyboardButtonId> backward;
		std::vector<KeyboardButtonId> left;
		std::vector<KeyboardButtonId> right;
		std::vector<KeyboardButtonId> down;
		std::vector<KeyboardButtonId> up;
		KeyboardButtons* keyboard_buttons;
		bool initialized = false;

		void load_ini() {
			std::function<int(void*, const char*, const char*, const char*)> callback = [this](void* user, const char* section, const char* name, const char* value) {
				if (strcmp(section, "kb") == 0) {
					// decide which action
					std::vector<KeyboardButtonId>* target;
					if (strcmp(name, "forward") == 0) {
						target = &forward;
					} else if (strcmp(name, "backward") == 0) {
						target = &backward;
					} else if(strcmp(name, "left") == 0) {
						target = &left;
					} else if (strcmp(name, "right") == 0) {
						target = &right;
					} else if (strcmp(name, "up") == 0) {
						target = &up;
					} else if (strcmp(name, "down") == 0) {
						target = &down;
					} else {
						return 0;
					}
					// decide which letters
					for (int i = 0; value[i] != 0 && i < 100; ++i) {
						if (value[i] == 'A') {
							target->push_back(KeyboardButtonId::A);
						} else if (value[i] == 'D') {
							target->push_back(KeyboardButtonId::D);
						} else if (value[i] == 'S') {
							target->push_back(KeyboardButtonId::S);
						} else if (value[i] == 'W') {
							target->push_back(KeyboardButtonId::W);
						} else if (value[i] == 'Q') {
							target->push_back(KeyboardButtonId::Q);
						} else if (value[i] == 'E') {
							target->push_back(KeyboardButtonId::E);
						} else {
							continue;
						}
					}
				}
				return 0;
			};
			GameEngine::Core::ini_parse("keybindings.ini", callback, nullptr); // This file must be in cmake_output\build_win64\Engine\Source\Game\ folder
			initialized = true;
		}

		bool get_button_pressed(const std::vector<KeyboardButtonId>& bindings) {
			if (!initialized) load_ini();
			for (const auto& bind : bindings) {
				if (keyboard_buttons->get(bind) == KeyboardButtonState::PRESSED) return true;
			}
			return false;
		}

	public:
		KeyboardBindings(KeyboardButtons* keyboard_buttons) : keyboard_buttons(keyboard_buttons) {}
		bool get_forward() { return get_button_pressed(forward); }
		bool get_backward() { return get_button_pressed(backward); }
		bool get_left() { return get_button_pressed(left); }
		bool get_right()  { return get_button_pressed(right); }
		bool get_down() { return get_button_pressed(down); }
		bool get_up() { return get_button_pressed(up); }
	};
}