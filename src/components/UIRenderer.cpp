// my code
#include "../visuals/Artist.h"
#include "UIRenderer.h"

void UIRenderer::onStart() {
	//try {
	//	transform = actor->getComponent("Transform");
	//	if (transform.isNil()) {
	//		Error::error("UIRenderer requires a Transform component on actor " + actor->name);
	//	}
	//}
	//catch (luabridge::LuaException e) {
	//	std::cout << e.what() << '\n';
	//}
}

void UIRenderer::onUpdate() {
	Artist::requestDrawUIEx("PlayButton", 5, 5, 0, 1, 1, 0.5, 0.5, 255, 255, 255, 255, sortingOrder);
}