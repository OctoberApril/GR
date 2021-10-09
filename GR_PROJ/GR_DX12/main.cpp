#include "Application.h"
#include <iostream>

#include "AssetImporter.h"
#include "SharedPtr.h"

#include "Scene.h"
#include "ThirdCamera.h"

class Sample : public Application
{
public:
	Sample(const char* app_name) :Application(app_name) {};

protected:
	void Start() override;
	void Setup() override;
	void Stop() override;

	template<typename T> using SharedPtr = ::SharedPtr<T>;	

	SharedPtr<Scene> m_SampleScene;
	SharedPtr<ThirdCamera> m_Camera;
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
	//auto castle = AssetImporter::ImportModel("./Model/Sponza/sponza.obj");
	auto castle = AssetImporter::ImportModel("./Model/BackPack/BackPack.fbx");
	
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