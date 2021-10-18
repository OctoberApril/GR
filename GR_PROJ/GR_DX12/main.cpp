#include "Application.h"
#include <iostream>

#include "AssetImporter.h"
#include "float2.h"
#include "SharedPtr.h"
#include "Input.h"

#include "Scene.h"
#include "ThirdCamera.h"
#include "GameObject.h"
#include "Graphics.h"
#include "assimp/material.h"

class Sample : public Application
{
public:
	Sample(const char* app_name) :Application(app_name) {};

protected:
	void Start() override;
	void Setup() override;
	void Stop() override;
	void Update() override;

	template<typename T> using SharedPtr = ::SharedPtr<T>;

	SharedPtr<Scene> m_SampleScene;
	ThirdCamera* m_Camera;
	SharedPtr<GameObject> m_CameraObj;
	SharedPtr<GameObject> m_Castle;
};

void Sample::Setup()
{
	std::cout << "Sample Setup" << std::endl;
}

void Sample::Start()
{
	std::cout << "Sample Start" << std::endl;

	m_SampleScene = new Scene("Sample Scene");
	//Load Model
	m_Castle = AssetImporter::ImportModel("./Model/Sponza/sponza.obj");
	//m_Castle = AssetImporter::ImportModel("./Model/BackPack/BackPack.fbx");
	auto t = m_Castle->GetComponent<Transform>();
	t->SetScale(2,2, 2);

	m_CameraObj = new GameObject();
	m_Camera = m_CameraObj->AddComponent<ThirdCamera>();
	m_Camera->SetCameraMode(CameraMode_Ortho);
	m_Camera->SetOrthoCamera(-3000, 3000, 3000, -3000, -3000, 3000);
	//m_Camera->SetCameraMode(CameraMode_Perspective);
	//m_Camera->SetPerspectiveCamera(90, 1, 0.1f, 2000);
	auto transform = m_CameraObj->GetComponent<Transform>();
	transform->SetPosition(0, 0, 200);
}

void Sample::Update()
{
	int w = DX12Graphics::Instance->GetGraphicsWidth();
	int h = DX12Graphics::Instance->GetGraphicsHeight();
	auto transform = m_CameraObj->transform();
	auto m_Rotation = transform->GetRotation();
	glm::vec3 m_Position = transform->GetPosition();

	{
		static float2 lastMousePosition = Input::Instance->GetMousePosition();

		if (Input::Instance->GetMouseButton(0))
		{
			float2 currentMousePosition = Input::Instance->GetMousePosition();
			float2 deltaMove = currentMousePosition - lastMousePosition;
			lastMousePosition = currentMousePosition;

			float xTheta = deltaMove.x / (w * 1.0f) * 3.1415927f * 0.5f;
			float yTheta = 0; //deltaMove.y / (h * 1.0f) * 3.1415927f * 0.5f;

			m_Rotation = glm::quat(glm::vec3(yTheta, xTheta, 0.0f)) * m_Rotation;
			m_CameraObj->transform()->SetRotation(m_Rotation);

			std::cout << m_Rotation.x << "		" << m_Rotation.y << "		" << m_Rotation.z << "		" << m_Rotation.w << std::endl;
		}
		else
		{
			lastMousePosition = Input::Instance->GetMousePosition();
		}
	}

	if (Input::Instance->GetKey(KeyCode::W))
	{
		m_Position = transform->GetLocalToWorldMatrix() * glm::vec4(-0.0f, 0, 50.0f, 1);
		transform->SetPosition(m_Position);
		std::cout << "[" << m_Position.x << "," << m_Position.y << "," << m_Position.z << "]" << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::S))
	{
		m_Position = transform->GetLocalToWorldMatrix() * glm::vec4(-0.0f, 0, -50.0f, 1);
		transform->SetPosition(m_Position);
		std::cout << "[" << m_Position.x << "," << m_Position.y << "," << m_Position.z << "]" << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::Q))
	{
		m_Position = transform->GetLocalToWorldMatrix() * glm::vec4(-0.0f, 50, 0.0f, 1);
		transform->SetPosition(m_Position);
		std::cout << "[" << m_Position.x << "," << m_Position.y << "," << m_Position.z << "]" << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::R))
	{
		m_Position = transform->GetLocalToWorldMatrix() * glm::vec4(-0.0f, -50.0f, 0.0f, 1);
		transform->SetPosition(m_Position);
		std::cout << "[" << m_Position.x << "," << m_Position.y << "," << m_Position.z << "]" << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::A))
	{
		m_Position = transform->GetLocalToWorldMatrix() * glm::vec4(-30.5f, 0, 0.0f, 1);
		transform->SetPosition(m_Position);
		std::cout << "[" << m_Position.x << "," << m_Position.y << "," << m_Position.z << "]" << std::endl;
	}

	if (Input::Instance->GetKey(KeyCode::D))
	{
		m_Position = transform->GetLocalToWorldMatrix() * glm::vec4(30.5f, 0, 0.0f, 1);
		transform->SetPosition(m_Position);
		std::cout << "[" << m_Position.x << "," << m_Position.y << "," << m_Position.z << "]" << std::endl;
	}
}

void Sample::Stop()
{
	std::cout << "Sample Stop" << std::endl;
}



void main()
{
	Sample app("Sample Demo");
	app.Run();
}