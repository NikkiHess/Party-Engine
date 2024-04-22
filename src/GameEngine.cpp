// std libraries
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <memory>

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

void Engine::runLifecycleFunctions(std::optional<glm::vec2> clickPos) {
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
    if (clickPos.has_value()) {
        int maxId = -1;
        int type = 0;
        std::shared_ptr<Actor> clickedActor;
        std::vector<std::shared_ptr<Component>> clickedComponents;

        for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnClick) {
            for (auto& [key, component] : actor->componentsWithOnClick) {
                // TODO: Need to make these C++ components for this to make any sense
                luabridge::LuaRef spriteRenderer = actor->getComponent("SpriteRenderer");
                luabridge::LuaRef uiSpriteRenderer = actor->getComponent("UISpriteRenderer");
                luabridge::LuaRef transform = actor->getComponent("Transform");

                // make sure we have a sprite renderer
                int id = !spriteRenderer.isNil() ? spriteRenderer["id"] : uiSpriteRenderer["id"]; // need this to get size

                // make sure we have a transform
                if (!transform.isNil() && transform["x"].isNumber() && transform["y"].isNumber()) {
                    auto it = resourceManager.imageRequestSizes.find(id);

                    // if the image was requested this frame
                    if (it != resourceManager.imageRequestSizes.end()) {
                        glm::vec2 click = clickPos.value();

                        float currX = transform["x"].cast<float>();
                        float currY = transform["y"].cast<float>();

                        if (!spriteRenderer.isNil()) {
                            click.x -= Camera::getWidth() / 2;
                            click.y -= Camera::getHeight() / 2;
                            currX *= 100;
                            currY *= 100;
                        }

                        // if we're within the image's bounds, check if this is our clicked actor/component
                        if (click.x >= currX && click.x <= currX + it->second.x) {
                            if (click.y >= currY && click.y <= currY + it->second.y) {

                                // if the current type is a regular sprite and we need a ui
                                if ((type == 0 && spriteRenderer.isNil())) {
                                    type = 1;
                                    maxId = -1;
                                }

                                // id needs to be highest possible
                                if (id > maxId) {
                                    maxId = id;
                                    clickedActor = actor;
                                    clickedComponents.emplace_back(component);
                                }
                            }
                        }
                    }
                }
            }
        }

        // if we have a clicked actor
        if (clickedActor != nullptr) {
            for (std::shared_ptr<Component>& comp : clickedComponents) {
                comp->callLuaFunction("OnClick", clickedActor->name);
            }
        }
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
    size_t currentIntroIndex = 0;
    isGameRunning = true;

    GameConfig& gameConfig = configManager.gameConfig;
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

        std::optional<glm::vec2> clickPos = std::nullopt;
        // Process events
        SDL_Event sdlEvent;
        while (Helper::SDL_PollEvent498(&sdlEvent)) {
            Input::processEvent(sdlEvent);

            if (Input::getMouseButtonDown(1)) {
                clickPos = std::make_optional<glm::vec2>(Input::getMousePosition());
            }
            // handle a quit event
            if (sdlEvent.type == SDL_QUIT) {
                requestStop();
            }
        }

        runLifecycleFunctions(clickPos);

        runtimeAlterations();

        // make the input not "newly down" or "newly up" anymore
        Input::lateUpdate();

        // handle and render gameplay
        if (!gameOver) {
            int frame = Helper::GetFrameNumber();

            // render the game first
            renderer.render();

            // set the scale back to normal
            SDL_RenderSetScale(renderer.sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);
        }

        // present the render AND DELAY, apparently it does that for us
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);

        // at the very end of every frame, clear request caches
        resourceManager.imageRequestSizes.clear();
        ImageDrawRequest::numRequests = 0;

        resourceManager.textRequestSizes.clear();
        TextDrawRequest::numRequests = 0;
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

	return 0;
}
