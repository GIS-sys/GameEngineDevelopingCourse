#include <RenderEngine.h>
#include <RHIHelper.h>
#include <Mesh.h>
#include <RHI/RHI/Movement.h>

namespace GameEngine::Render
{
	RenderEngine::RenderEngine()
	{
		m_rhi = HAL::RHIHelper::CreateRHI("D3D12");
		m_rhi->Init();

		// BLINKING if you want to get rid of blinking - comment one of objects creation

		RenderObject::Ptr box = std::make_shared<RenderObject>();
		m_RenderObjects.push_back(box);
		box->m_mesh = m_rhi->CreateBoxMesh();
		box->m_material = m_rhi->GetMaterial(box->m_mesh->GetName());

		RenderObject::Ptr pyramid = std::make_shared<RenderObject>();
		m_RenderObjects.push_back(pyramid);
		pyramid->m_mesh = m_rhi->CreatePyramidMesh(1.0, 2.0, 3.0);
		pyramid->m_material = m_rhi->GetMaterial(pyramid->m_mesh->GetName());
		
		m_rhi->ExecuteCommandLists();
		m_rhi->Flush();
	}

	void RenderEngine::Update()
	{
		// move around
		long mss = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
		float delta = (mss % 5000) / 5000.0;
		Movement movement(
			std::sin(delta * 6.28) * 16,
			std::cos(delta * 6.28) * 8,
			std::sin(2 * delta * 6.28) * 8,
			delta * (1 - delta) * delta * (1 - delta) * 4
		);

		// draw
		for (const auto& object : m_RenderObjects) {
			m_rhi->Update(object->m_mesh, object->m_material, movement);
			m_rhi->Flush(); // BLINKING I know it's not supposed to be here but for some reason multiple objects are not supported (could not figure it out yet)
		}
		m_rhi->Flush();
	}

	void RenderEngine::OnResize(uint16_t width, uint16_t height)
	{
		m_swapChainWidth = width;
		m_swapChainHeight = height;
		m_rhi->OnResize();
	}
}