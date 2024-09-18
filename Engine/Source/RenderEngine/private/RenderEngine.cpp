#include <RenderEngine.h>
#include <RHIHelper.h>
#include <Mesh.h>

namespace GameEngine::Render
{
	RenderEngine::RenderEngine()
	{
		m_rhi = HAL::RHIHelper::CreateRHI("D3D12");
		m_rhi->Init();

		RenderObject::Ptr box = std::make_shared<RenderObject>();
		m_RenderObjects.push_back(box);
		box->m_mesh = m_rhi->CreatePyramidMesh(0.0, 0.0, 0.0);
		box->m_material = m_rhi->GetMaterial(box->m_mesh->GetName());
		
		m_rhi->ExecuteCommandLists();
		m_rhi->Flush();
	}

	void RenderEngine::Update()
	{
		// move around
		long mss = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
		float dx = (mss % 1000) / 100.0;
		float dy = dx, dz = dx;
		/*m_RenderObjects[0]->m_mesh = m_rhi->CreatePyramidMesh(dx, dy, dz);
		m_RenderObjects[0]->m_material = m_rhi->GetMaterial(m_RenderObjects[0]->m_mesh->GetName());*/
		/*m_RenderObjects.pop_back();
		RenderObject::Ptr box = std::make_shared<RenderObject>();
		m_RenderObjects.push_back(box);
		box->m_mesh = m_rhi->CreatePyramidMesh(0.0, 0.0, 0.0);
		box->m_material = m_rhi->GetMaterial(box->m_mesh->GetName());*/
		m_RenderObjects[0]->m_mesh->move(dx, dy, dz);

		// draw
		m_rhi->Update(m_RenderObjects[0]->m_mesh, m_RenderObjects[0]->m_material);
		m_rhi->Flush();
	}

	void RenderEngine::OnResize(uint16_t width, uint16_t height)
	{
		m_swapChainWidth = width;
		m_swapChainHeight = height;
		m_rhi->OnResize();
	}
}