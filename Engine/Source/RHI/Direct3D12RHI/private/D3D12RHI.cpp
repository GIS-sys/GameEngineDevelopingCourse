#include <array.h>
#include <D3D12Mesh.h>
#include <D3D12RHI.h>
#include <D3D12RHIPrivate.h>
#include <D3D12Material.h>
#include <Math/Vector.h>

namespace GameEngine
{
	namespace Render::HAL
	{
		using namespace Core;

		D3D12RHI::D3D12RHI() :
			m_d3d12Private(std::make_shared<D3D12RHIPrivate>())
		{
			m_d3d12Private->EnableDebugLayer();
		}

		void D3D12RHI::Init()
		{
			m_d3d12Private->Init();
		}

		void D3D12RHI::Update(Mesh::Ptr mesh, Material::Ptr material)
		{
			m_d3d12Private->Update(mesh, material);
		}

		Mesh::Ptr D3D12RHI::CreateBoxMesh()
		{
			array<Vertex, 8> vertices =
			{
				Vertex({ Math::Vector3f(-1.0f, -1.0f, -1.0f), Math::Vector4f((float*)&DirectX::Colors::White) }),
				Vertex({ Math::Vector3f(-1.0f, +1.0f, -1.0f), Math::Vector4f((float*)&DirectX::Colors::Black) }),
				Vertex({ Math::Vector3f(+1.0f, +1.0f, -1.0f), Math::Vector4f((float*)&DirectX::Colors::Red) }),
				Vertex({ Math::Vector3f(+1.0f, -1.0f, -1.0f), Math::Vector4f((float*)&DirectX::Colors::Green) }),
				Vertex({ Math::Vector3f(0.0f, 0.0f, +1.0f), Math::Vector4f((float*)&DirectX::Colors::Blue) })
			};

			array<uint16_t, 36> indices =
			{
				// front face
				0, 1, 4,

				// back face
				1, 2, 4,

				// left face
				2, 3, 4,

				// right face
				3, 0, 4,

				// bottom face
				0, 1, 2,
				0, 2, 3
			};

			return m_d3d12Private->CreateMesh(vertices.begin(), vertices.size(), sizeof(Vertex), indices.begin(), indices.size(), sizeof(uint16_t));
		}

		Material::Ptr D3D12RHI::GetMaterial(const std::string& name)
		{
			return m_d3d12Private->GetMaterial(name);
		}

		void D3D12RHI::ExecuteCommandLists()
		{
			m_d3d12Private->ExecuteCommandLists();
		}

		void D3D12RHI::Flush()
		{
			m_d3d12Private->FlushCommandQueue();
		}

		void D3D12RHI::OnResize()
		{
			m_d3d12Private->OnResize();
		}
	}
}