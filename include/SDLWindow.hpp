#pragma once

#include "Window.hpp"

struct SDL_Window;

namespace gintonic
{

class SDLWindow : public Window
{
  public:
    SDLWindow(const char* title);
    SDLWindow(const char* title, const int width, const int height);
    ~SDLWindow() override;

    void resize(const int newWidth, const int newHeight) override final;

    void show() noexcept override final;

    void hide() noexcept override final;

    int getWidth() const noexcept override final;

    int getHeight() const noexcept override final;

    float getAspectRatio() const noexcept override final
    {
        return mAspectRatio;
    }

    vec2f getDimensions() const noexcept override final
    {
        return vec2f(static_cast<float>(getWidth()),
                     static_cast<float>(getHeight()));
    }

    int getID() const noexcept;

  private:
    float mAspectRatio;
    SDL_Window* mHandle;
    friend class SDLRenderContext;
    void updateAspectRatio() noexcept;
    void preInit();
    void postInit(const char* title, const int width, const int height,
                  const uint32_t flags);
};

} // gintonic
