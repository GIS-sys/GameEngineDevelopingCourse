#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		// How many objects do we want to create
		for (int i = 0; i < 100; ++i)
		{
			const int MAX_OBJECT_TYPES = 4;
			int random_choice = std::rand() % MAX_OBJECT_TYPES;
			if (random_choice == 0) {
				m_Objects.push_back(new PhysicalGameObject());
			} else if (random_choice == 1) {
				m_Objects.push_back(new ControllableGameObject());
			} else if (random_choice == 2) {
				m_Objects.push_back(new OscillatingGameObject(Math::Vector3f(5.0, -0.2, 0.5), 6.0));
			} else {
				m_Objects.push_back(new MovingGameObject(Math::Vector3f(0.0, 1.0, 2.0)));
			}
			m_Objects.back()->SetPosition(Math::Vector3f(0.0, i * 2.5, 0.0), m_renderThread->GetMainFrame());
			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
		}

		Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoBack", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoRight", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
		//Core::g_InputHandler->RegisterCallback("GoRight", [&]() { m_Objects[0]->SetPosition(m_Objects[0]->GetPosition() + Math::Vector3f(0.01, 0.0, 0.0), m_renderThread->GetMainFrame()); });
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->move(m_renderThread->GetMainFrame(), dt, m_Objects);
		}
	}
}