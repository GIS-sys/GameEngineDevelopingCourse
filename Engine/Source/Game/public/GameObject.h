#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>
#include <Constants.h>

namespace GameEngine
{
	class GameObject
	{
	public:
		GameObject() = default;
		virtual ~GameObject() = default;

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(position, frame);
			}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

		virtual void move(size_t frame, float dt, const std::vector<GameObject*>& all_objects) = 0;

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
	};

	class MovingGameObject : public GameObject {
	protected:
		Math::Vector3f speed;

	public:
		MovingGameObject(Math::Vector3f speed) : speed(speed) {}

		virtual void move(size_t frame, float dt, const std::vector<GameObject*>&) {
			Math::Vector3f pos = GetPosition();
			pos = pos + speed * dt;
			SetPosition(pos, frame);
		}
	};

	class OscillatingGameObject : public GameObject {
	protected:
		Math::Vector3f oscillation_vector;
		float current_position_tick = 0.0;
		float time_period = 0.0;

		float get_oscillation_fraction() const {
			return std::sin(current_position_tick * 2 * Math::Constants::PI / time_period);
		}

	public:
		OscillatingGameObject(Math::Vector3f oscillation_vector, float time_period) : oscillation_vector(oscillation_vector), time_period(time_period) {}

		virtual void move(size_t frame, float dt, const std::vector<GameObject*>&) {
			float oscillatin_fraction_before = get_oscillation_fraction();
			current_position_tick = current_position_tick + dt;
			float oscillatin_fraction_after = get_oscillation_fraction();
			Math::Vector3f delta = oscillation_vector * (oscillatin_fraction_after - oscillatin_fraction_before);

			Math::Vector3f pos = GetPosition();
			pos = pos + delta;
			SetPosition(pos, frame);
		}
	};

	class ControllableGameObject : public GameObject {
	public:
		virtual void move(size_t frame, float dt, const std::vector<GameObject*>&) {}
	};

	class PhysicalGameObject : public GameObject {
	public:
		virtual void move(size_t frame, float dt, const std::vector<GameObject*>& all_objects) {}
	};
}