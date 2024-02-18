#include "RenderingConfig.h"

void RenderingConfig::parse(rapidjson::Document& renderingDocument) {
	// handle camera
	if (renderingDocument.HasMember("x_resolution"))
		renderSize.x = renderingDocument["x_resolution"].GetInt();
	if (renderingDocument.HasMember("y_resolution"))
		renderSize.y = renderingDocument["y_resolution"].GetInt();

	// get the zoom factor first, so it can be applied to our camera offset
	if (renderingDocument.HasMember("zoom_factor"))
		zoomFactor = renderingDocument["zoom_factor"].GetFloat();
	if (renderingDocument.HasMember("cam_ease_factor"))
		easeFactor = renderingDocument["cam_ease_factor"].GetFloat();
	if (renderingDocument.HasMember("cam_offset_x"))
		cameraOffset.x = renderingDocument["cam_offset_x"].GetFloat() * zoomFactor;
	if (renderingDocument.HasMember("cam_offset_y"))
		cameraOffset.y = renderingDocument["cam_offset_y"].GetFloat() * zoomFactor;

	// handle bg color
	if (renderingDocument.HasMember("clear_color_r"))
		clearColor.r = renderingDocument["clear_color_r"].GetInt();
	if (renderingDocument.HasMember("clear_color_g"))
		clearColor.g = renderingDocument["clear_color_g"].GetInt();
	if (renderingDocument.HasMember("clear_color_b"))
		clearColor.b = renderingDocument["clear_color_b"].GetInt();
}