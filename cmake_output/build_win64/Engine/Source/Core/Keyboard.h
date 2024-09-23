#pragma once

#include <vector>
#include <unordered_map>
#include <Core/ini.h>


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
		S,
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
		KeyboardButtons* keyboard_buttons;
		bool initialized = false;

		void load_ini() {
			forward.push_back(KeyboardButtonId::A);
			// TODO proper ini load
			ini_parse("qwe", [](void* user, const char* section, const char* name, const char* value) { return 0;  }, nullptr);
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
	};
}