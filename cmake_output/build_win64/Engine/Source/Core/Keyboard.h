#pragma once

#include <vector>
#include <unordered_map>


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
		static std::unordered_map<KeyboardButtonId, KeyboardButtonState> button_states;
		static KeyboardButtonState get(KeyboardButtonId button_id) {
			if (button_states.find(button_id) == button_states.end()) return KeyboardButtonState::UNDEFINED;
			return button_states.at(button_id);
		}
	};
	std::unordered_map<KeyboardButtonId, KeyboardButtonState> KeyboardButtons::button_states;

	class KeyboardBindings {
	private:
		static std::vector<KeyboardButtonId> forward;
		static std::vector<KeyboardButtonId> backward;
		static std::vector<KeyboardButtonId> left;
		static std::vector<KeyboardButtonId> right;
		static bool initialized;

		static void load_ini() {
			forward.push_back(KeyboardButtonId::A);
			// TODO proper ini load
			initialized = true;
		}

		static bool get_button_pressed(const std::vector<KeyboardButtonId>& bindings) {
			if (!initialized) load_ini();
			for (const auto& bind : bindings) {
				if (KeyboardButtons::get(bind) == KeyboardButtonState::PRESSED) return true;
			}
			return false;
		}

	public:
		static bool get_forward() { return get_button_pressed(forward); }
		static bool get_backward() { return get_button_pressed(backward); }
		static bool get_left() { return get_button_pressed(left); }
		static bool get_right() { return get_button_pressed(right); }
	};
	std::vector<KeyboardButtonId> KeyboardBindings::forward;
	std::vector<KeyboardButtonId> KeyboardBindings::backward;
	std::vector<KeyboardButtonId> KeyboardBindings::left;
	std::vector<KeyboardButtonId> KeyboardBindings::right;
	bool KeyboardBindings::initialized = false;
}