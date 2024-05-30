// std library
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <vector>

// include my code
#include "GameEngine.h"
#include "utils/config/ConfigManager.h"
#include "utils/config/GameConfig.h"
#include "utils/config/RenderingConfig.h"
#include "visuals/Renderer.h"
#include "input/Input.h"
#include "input/Direction.h"
#include "utils/StringUtils.h"
#include "utils/LuaUtils.h"
#include "utils/LuaStateSaver.h"

// dependencies
#include "glm/glm.hpp"
#include "rapidjson/document.h"

// SDL2
#include "AudioHelper.h"
#include "Helper.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

void Engine::runLifecycleFunctions(glm::vec2 mousePos, int clickType) {
    // do OnStart for all actors with NEW OnStart components
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnStart) {
        for (auto& [key, component] : actor->componentsWithOnStart) {
            component->callLuaFunction("OnStart", actor->name);
            if (component->onStartCalled) {
                actor->componentsToRemoveFromOnStart[component->key] = component;
            }
        }

        for (auto& [key, component] : actor->componentsToRemoveFromOnStart) {
            actor->componentsWithOnStart.erase(key);
        }
        actor->componentsToRemoveFromOnStart.clear();
    }

    // do OnUpdate for all actors
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnUpdate) {
        for (auto& [key, component] : actor->componentsWithOnUpdate) {
            component->callLuaFunction("OnUpdate", actor->name);
        }
    }

    // do OnLateUpdate for all actors
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnLateUpdate) {
        for (auto& [key, component] : actor->componentsWithOnLateUpdate) {
            component->callLuaFunction("OnLateUpdate", actor->name);
        }
    }

    // TODO: This is awful, fix PLEASE
    glm::vec2 origMouse = mousePos;

    size_t maxId = 0;
    int type = 0;
    std::shared_ptr<Actor> clickedActor;

    std::vector<std::shared_ptr<Component>> leftClicked;
    std::vector<std::shared_ptr<Component>> middleClicked;
    std::vector<std::shared_ptr<Component>> rightClicked;

    // handle clicks
    if (clickType != -1) {
        for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnClick) {
            for (auto& [key, component] : actor->componentsWithOnClick) {
                glm::vec2 mouse = mousePos;
                // TODO: Need to make these C++ components for this to make any sense
                luabridge::LuaRef spriteRenderer = actor->getComponent("SpriteRenderer");
                luabridge::LuaRef transform = actor->getComponent("Transform");

                // make sure we have a sprite renderer
                size_t id = spriteRenderer["id"]; // need this to get size
                std::string sprite = spriteRenderer["sprite"];
                bool ui = spriteRenderer["ui"];

                // make sure we have a transform
                if (!transform.isNil() && transform["x"].isNumber() && transform["y"].isNumber()) {
                    // if the image was requested this frame
                    if ((ui && resourceManager.uiImageDrawRequests.size() >= id) ||
                        (!ui && resourceManager.imageDrawRequests.size() >= id)) {
                        float currX = transform["x"].cast<float>();
                        float currY = transform["y"].cast<float>();
                        float pivotX = transform["pivotX"];
                        float pivotY = transform["pivotY"];
                        SDL_Point pivotPoint = {
                            static_cast<int>(pivotX * Artist::getImageWidth(sprite)),
                            static_cast<int>(pivotY * Artist::getImageHeight(sprite))
                        };

                        if (!ui) {
                            mouse.x -= Camera::getWidth() / 2.0f;
                            mouse.y -= Camera::getHeight() / 2.0f;
                            currX *= 100;
                            currY *= 100;
                        }

                        mouse.x += pivotPoint.x;
                        mouse.y += pivotPoint.y;

                        // if we're within the image's bounds, check if this is our clicked actor/component
                        if (mouse.x >= currX && mouse.x <= currX + Artist::getImageWidth(sprite)) {
                            if (mouse.y >= currY && mouse.y <= currY + Artist::getImageHeight(sprite)) {
                                // if the current type is a regular sprite and we need a ui
                                if ((type == 0 && spriteRenderer.isNil())) {
                                    type = 1;
                                    maxId = -1;
                                }

                                // id needs to be highest possible
                                if (id >= maxId) {
                                    maxId = id;
                                    clickedActor = actor;

                                    if (clickType == 1) {
                                        leftClicked.emplace_back(component);
                                    }
                                    else if (clickType == 2) {
                                        middleClicked.emplace_back(component);
                                    }
                                    else if (clickType == 3) {
                                        rightClicked.emplace_back(component);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    mousePos = origMouse;

    std::vector<std::shared_ptr<Component>> enter;
    std::vector<std::shared_ptr<Component>> hover;
    std::vector<std::shared_ptr<Component>> exit;

    // handle mouses
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnMouse) {
        for (auto& [key, component] : actor->componentsWithOnMouse) {
            // TODO: Need to make these C++ components for this to make any sense
            luabridge::LuaRef spriteRenderer = actor->getComponent("SpriteRenderer");
            luabridge::LuaRef transform = actor->getComponent("Transform");

            // make sure we have a sprite renderer
            size_t id = spriteRenderer["id"]; // need this to get size
            std::string sprite = spriteRenderer["sprite"];
            bool ui = spriteRenderer["UI"];

            // make sure we have a transform
            if (!transform.isNil() && transform["x"].isNumber() && transform["y"].isNumber()) {
                // if the image was requested this frame
                if ((ui && resourceManager.uiImageDrawRequests.size() <= id) ||
                    (!ui && resourceManager.imageDrawRequests.size() <= id)) {
                    float currX = transform["x"].cast<float>();
                    float currY = transform["y"].cast<float>();

                    if (!ui) {
                        mousePos.x -= Camera::getWidth() / 2.0f;
                        mousePos.y -= Camera::getHeight() / 2.0f;
                        currX *= 100;
                        currY *= 100;
                    }

                    float pivotX = transform["pivotX"];
                    float pivotY = transform["pivotY"];

                    SDL_Point pivotPoint = {
                        static_cast<int>(pivotX * Artist::getImageWidth(sprite)),
                        static_cast<int>(pivotY * Artist::getImageHeight(sprite))
                    };

                    mousePos.x += pivotPoint.x;
                    mousePos.y += pivotPoint.y;

                    // if we're within the image's bounds, check if this is our clicked actor/component
                    if (mousePos.x >= currX && mousePos.x <= currX + Artist::getImageWidth(sprite)) {
                        if (mousePos.y >= currY && mousePos.y <= currY + Artist::getImageHeight(sprite)) {
                            // maintain hovers
                            if (component->mouseHovered) {
                                component->mouseEntered = false;
                                component->mouseHovered = true;
                                hover.emplace_back(component);
                            }
                            // otherwise add an enter AND a hover
                            else {
                                component->mouseEntered = true;
                                component->mouseHovered = true;
                                enter.emplace_back(component);
                                hover.emplace_back(component);
                            }
                            continue; // make sure we don't execute exited code if this happens
                        }
                    }

                    // handle mouse exits
                    if (component->mouseEntered || component->mouseHovered) {
                        component->mouseEntered = false;
                        component->mouseHovered = false;

                        component->mouseExited = true;
                        exit.emplace_back(component);
                    }
                }
            }
        }
    }

    // if we have a clicked actor
    if (clickedActor != nullptr) {
        for (std::shared_ptr<Component>& comp : leftClicked) {
            comp->callLuaFunction("OnLeftClick", clickedActor->name);
        }
        for (std::shared_ptr<Component>& comp : middleClicked) {
            comp->callLuaFunction("OnMiddleClick", clickedActor->name);
        }
        for (std::shared_ptr<Component>& comp : rightClicked) {
            comp->callLuaFunction("OnRightClick", clickedActor->name);
        }
    }

    for (std::shared_ptr<Component>& comp : enter) {
        comp->callLuaFunction("OnMouseEnter", clickedActor->name);
    }
    for (std::shared_ptr<Component>& comp : hover) {
        comp->callLuaFunction("OnMouseHover", clickedActor->name);
    }
    for (std::shared_ptr<Component>& comp : exit) {
        comp->callLuaFunction("OnMouseExit", clickedActor->name);
    }
}

void Engine::runtimeAlterations() {
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsToAdd) {
        LuaUtils::instantiateActor(actor);

        for (auto& [key, component] : actor->componentsByKey) {
            component->instanceTable["actor"] = actor.get();
        }
    }
    GameInfo::scene.actorsToAdd.clear();

    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsToRemove) {
        LuaUtils::destroyActor(actor);
    }
    GameInfo::scene.actorsToRemove.clear();

    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithNewComponents) {
        for (std::shared_ptr<Component> component : actor->componentsToAdd) {
            std::optional<rapidjson::Value*> opt = std::nullopt;
            actor->addComponent(component, opt);
        }
        actor->componentsToAdd.clear();
    }
    GameInfo::scene.actorsWithNewComponents.clear();

    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithComponentsToRemove) {
        for (std::shared_ptr<Component> component : actor->componentsToRemove) {
            actor->removeComponent(component);
        }
        actor->componentsToRemove.clear();
    }
    GameInfo::scene.actorsWithComponentsToRemove.clear();
}

void Engine::start() {
    isGameRunning = true;

    RenderingConfig& renderConfig = configManager.renderingConfig;

    for (std::shared_ptr<Actor>& actor : GameInfo::scene.actors) {
        // store the actor as a convenience reference in the component
        for (auto& [key, component] : actor->componentsByKey) {
            component->instanceTable["actor"] = actor.get();
        }
    }

    // do OnStart for all actors, then CLEAR actorsWithOnStart to allow for new
    // OnStarts to be added in the middle of the game
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnStart) {
        for (auto& [key, component] : actor->componentsWithOnStart) {
            component->callLuaFunction("OnStart", actor->name);
        }
    }
    GameInfo::scene.actorsWithOnStart.clear();

    // main game loop
    // see function declaration/docs for order of events
    while (isGameRunning) {
        // if there is a scene switch, we need to do that right at the start of the frame
        if (GameInfo::scene.id != GameInfo::newScene.id) {
            GameInfo::scene = GameInfo::newScene;
            LuaUtils::currentScene = &GameInfo::scene;

            for (std::shared_ptr<Actor>& actor : GameInfo::scene.actors) {
                // store the actor as a convenience reference in the component
                for (auto& [key, component] : actor->componentsByKey) {
                    component->instanceTable["actor"] = actor.get();
                }
            }
        }
        
        glm::vec2 mousePos = Input::getMousePosition();
        int clickType = -1;
        // Process events
        SDL_Event sdlEvent;
        while (Helper::SDL_PollEvent498(&sdlEvent)) {
            Input::processEvent(sdlEvent);

            if (Input::getMouseButtonDown(1)) {
                clickType = 1;
            }
            if (Input::getMouseButtonDown(2)) {
                clickType = 2;
            }
            if (Input::getMouseButtonDown(3)) {
                clickType = 3;
            }
            // handle a quit event
            if (sdlEvent.type == SDL_QUIT) {
                requestStop();
            }
        }

        runLifecycleFunctions(mousePos, clickType);

        runtimeAlterations();

        // make the input not "newly down" or "newly up" anymore
        Input::lateUpdate();

        // render the game first
        renderer.render();

        // set the scale back to normal
        SDL_RenderSetScale(renderer.sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);

        // present the render AND DELAY, apparently it does that for us
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);
    }

    // execute OnExit for all actors
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnExit) {
        for (auto& [key, component] : actor->componentsWithOnExit) {
            component->callLuaFunction("OnExit", actor->name);
        }
    }
}

void Engine::requestStop() {
	isGameRunning = false;
}

int main(int argc, char* argv[]) {
    // suppress compiler warnings
    (void)argc;
    (void)argv;

	// Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        const std::string& sdlError = SDL_GetError();
        Error::error("SDL could not initialize. SDL error: " + sdlError);
    }
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);

    AudioHelper::Mix_AllocateChannels498(50);

	// open the default audio device for playback
	AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 1, 2048);

    // open a new state for the lua and open some default libraries
    LuaStateSaver::luaState = luaL_newstate();
    luaL_openlibs(LuaStateSaver::luaState);

    Input::init();

    LuaUtils::setupLua();

    ResourceManager resourceManager;
	ConfigManager configManager(resourceManager);
	Renderer renderer(configManager, resourceManager);

	Engine engine(renderer, configManager, resourceManager);
    LuaUtils::currentScene = &GameInfo::scene;
    LuaUtils::sceneConfig = &configManager.sceneConfig;
    LuaUtils::resourceManager = &resourceManager;

    AudioPlayer::resourceManager = &resourceManager;

    Camera::renderConfig = &configManager.renderingConfig;

	engine.start();

    // destroy SDL resources to make sure there aren't memory leaks
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    Mix_Quit();

    // clear the lua state
    LuaStateSaver::luaState = nullptr;

	return 0;
}
