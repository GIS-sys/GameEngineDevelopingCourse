#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>

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

		virtual void move(size_t frame, float dt) = 0;

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
	};

	class ControllableGameObject : public GameObject {
		virtual void move(size_t frame, float dt) {}
	};

	class MovingGameObject : public GameObject {
		virtual void move(size_t frame, float dt) {
			Math::Vector3f pos = GetPosition();

			// Showcase
			pos.x += 0.5f * dt;
			pos.y -= 0.5f * dt;
			SetPosition(pos, frame);
		}
	};

	class PhysicalGameObject : public GameObject {
		virtual void move(size_t frame, float dt) {}
	};
}