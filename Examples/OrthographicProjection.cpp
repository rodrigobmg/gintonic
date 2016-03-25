#include "Application.hpp"

#include <iomanip>

#define APPNAME "Orthographic Projection"

class OrthographicApplication : public Application
{
public:

	OrthographicApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		auto lCamera = std::make_shared<Camera>();
		lCamera->name = "OrthoCamera";
		lCamera->setNearPlane(-5.0f);
		lCamera->setFarPlane(5.0f);
		lCamera->setWidth(4.0f * Renderer::aspectRatio());
		lCamera->setHeight(4.0f);
		lCamera->setProjectionType(Camera::kOrthographic);
		auto lCameraEntity = Renderer::getCameraEntity();
		lCameraEntity->camera = lCamera;
		lCameraEntity->setTranslationZ(0.0f);
		Renderer::setFreeformCursor(true);
		Renderer::show();

		auto lTexture = std::make_shared<Texture2D>("Resources/DaVinci.jpg");
		auto lMaterial = std::make_shared<Material>();
		lMaterial->name = "DaVinci";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.0f);
		lMaterial->diffuseTexture = lTexture;
		auto lCube = std::make_shared<Entity>("Cube");
		lCube->material = lMaterial;
		lCube->mesh = Renderer::getUnitCube();
		lCube->setParent(mRootEntity);
	}

	virtual ~OrthographicApplication() noexcept = default;

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		auto lCameraEntity = Renderer::getCameraEntity();
		Renderer::cerr() << "Camera projection: " << lCameraEntity->camera->projectionMatrix() << '\n';
		Renderer::cerr() << "Camera width:      " << lCameraEntity->camera->width() << '\n';
		Renderer::cerr() << "Camera height:     " << lCameraEntity->camera->height() << '\n';
		Renderer::cerr() << "Camera near plane: " << lCameraEntity->camera->nearPlane() << '\n';
		Renderer::cerr() << "Camera far plane:  " << lCameraEntity->camera->farPlane() << '\n';

	}

};

DEFINE_MAIN(OrthographicApplication);
