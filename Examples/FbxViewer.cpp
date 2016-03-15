#include "gintonic.hpp"

#define APPNAME "FbxViewer"

struct GameState
{
	std::vector<std::shared_ptr<gintonic::Entity>> mEntities;
	std::vector<std::shared_ptr<gintonic::Material>> mMaterials;
	std::vector<std::shared_ptr<gintonic::Mesh>> mMeshes;
	std::vector<std::shared_ptr<gintonic::Light>> mLights;
	std::vector<std::shared_ptr<gintonic::Camera>> mCameras;
	std::shared_ptr<gintonic::Entity> mRootEntity;

	float mMoveSpeed = 1.0f;

	void clear() noexcept
	{
		mEntities.clear();
		mMaterials.clear();
		mMeshes.clear();
		mLights.clear();
		mCameras.clear();
		mRootEntity = nullptr;
	}
};

GameState gState;

bool initialize(const int argc, char** argv)
{
	using namespace gintonic;

	if (argc <= 1)
	{
		std::cerr << "Supply a filename...\n";
		return false;
	}

	// Create Entity and camera.
	auto lCamera = std::make_shared<Camera>();
	lCamera->name = "DefaultCamera";
	lCamera->setNearPlane(0.01f);
	lCamera->setFarPlane(100.0f);
	lCamera->setProjectionType(Camera::kPerspectiveProjection);
	auto lCameraEntity = std::make_shared<Entity>();
	lCameraEntity->name = "DefaultCamera";
	lCameraEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lCameraEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
	lCameraEntity->camera = lCamera;

	try
	{
		initializeEverything(APPNAME, lCameraEntity);

		const boost::filesystem::path lFilename(argv[1]);
		if (!boost::filesystem::is_regular_file(lFilename))
		{
			std::cerr << lFilename << " is not a regular file!\n";
			return false;
		}
		const auto lFilenameAsString = lFilename.string();

		gintonic::FbxImporter lImporter;
		auto lResult = lImporter.loadFromFile(lFilenameAsString.c_str());
		gState.mEntities = std::move(lResult.entities);
		gState.mMaterials = std::move(lResult.materials);
		gState.mMeshes = std::move(lResult.meshes);
		gState.mLights = std::move(lResult.lights);
		gState.mCameras = std::move(lResult.cameras);
		gState.mRootEntity = std::move(lResult.rootEntity);

		std::cout << "\nNumber of entities: " << gState.mEntities.size() << '\n';
		std::cout << "Root Entity: " << gState.mRootEntity->name << '\n';
		std::cout << "Number of materials: " << gState.mMaterials.size() << '\n';
		std::cout << "Number of meshes: " << gState.mMeshes.size() << '\n';
		std::cout << "Number of lights: " << gState.mLights.size() << '\n';
		std::cout << "Number of cameras: " << gState.mCameras.size() << "\n\n";

		gState.mEntities.push_back(lCameraEntity);
		gState.mCameras.push_back(lCamera);
	}
	catch (const exception& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}

	if (gState.mLights.empty())
	{
		// Put a directional light in the scene
		// so that we see something interesting.
		// The directional light shines downwards.
		auto lLightEntity = std::make_shared<gintonic::Entity>();
		auto lLight = std::shared_ptr<Light>(new DirectionalLight());
		lLight->intensity = 1.0f;
		lLightEntity->name = "DefaultDirectionalLight";
		lLightEntity->setLocalTransform
		(
			SQT
			(
				vec3f(1.0f, 1.0f, 1.0f), 
				quatf::axis_angle
				(
					vec3f(1.0f, 0.0f, 0.0f), 
					-M_PI / 2.0f + 1.0f
				), 
				vec3f(0.0f, 0.0f, 0.0f)
			)
		);
		lLight->name = "DefaultDirectionalLight";
		lLightEntity->light = lLight;
		gState.mEntities.push_back(lLightEntity);
		gState.mLights.push_back(lLight);
	}

	// Determine a "nice" camera offset so that we are not "inside" geometry,
	// and at the same time determine a "nice" movement speed so that we don't
	// feel like snails or airplanes.
	float lCameraOffsetZDistance(0.0f);

	for (auto lMesh : gState.mMeshes)
	{
		const auto lBBox = lMesh->getLocalBoundingBox();
		auto lBBoxArea = 
			(lBBox.max_corner.x - lBBox.min_corner.x) * 
			(lBBox.max_corner.y - lBBox.min_corner.y) * 
			(lBBox.max_corner.z - lBBox.min_corner.z);

		// Take the cube root. Crude approximation of a single side length.
		lBBoxArea = std::pow(lBBoxArea, 1.0f / 3.0f);

		gState.mMoveSpeed += lBBoxArea / static_cast<float>(gState.mMeshes.size());

		if (lBBox.max_corner.z > lCameraOffsetZDistance)
		{
			lCameraOffsetZDistance = lBBox.max_corner.z;
		}

	}

	lCameraEntity->setTranslationZ(lCameraOffsetZDistance + 4.0f);

	return true;
}

int main(int argc, char** argv)
{
	using namespace gintonic;

	if (!initialize(argc, argv))
	{
		return EXIT_FAILURE;
	}

	std::cout << Renderer::name() << '\n';
	std::cout << Renderer::version() << '\n';

	Renderer::setFreeformCursor(true);
	Renderer::show();

	double lDeltaTime, lElapsedTime;

	while (Renderer::shouldClose() == false)
	{
		Renderer::getElapsedAndDeltaTime(lElapsedTime, lDeltaTime);
		auto lCameraEntity = Renderer::getCameraEntity();

		if (Renderer::key(SDL_SCANCODE_Q))
		{
			Renderer::close();
		}
		if (Renderer::key(SDL_SCANCODE_W))
		{
			lCameraEntity->moveForward(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_A))
		{
			lCameraEntity->moveLeft(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_S))
		{
			lCameraEntity->moveBackward(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_D))
		{
			lCameraEntity->moveRight(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_SPACE))
		{
			lCameraEntity->moveUp(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_C))
		{
			lCameraEntity->moveDown(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::keyTogglePress(SDL_SCANCODE_T))
		{
			Renderer::setWireframeMode(!Renderer::getWireframeMode());
		}

		const auto lMouseDelta = -deg2rad(Renderer::mouseDelta()) / 10.0f;
		lCameraEntity->camera->addMouse(lMouseDelta);
		lCameraEntity->setRotation(quatf::mouse(lCameraEntity->camera->angles()));

		gState.mRootEntity->setRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), lElapsedTime / 10.0f));

		Renderer::submitEntities(gState.mEntities.begin(), gState.mEntities.end());

		Renderer::update();
	}

	return EXIT_SUCCESS;
}