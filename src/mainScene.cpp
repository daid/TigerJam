#include "mainScene.h"
#include "ingameMenu.h"
#include "main.h"

#include <sp2/graphics/gui/loader.h>
#include <sp2/random.h>
#include <sp2/engine.h>
#include <sp2/scene/camera.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>


Scene::Scene()
: sp::Scene("MAIN")
{
    sp::Scene::get("INGAME_MENU")->enable();
}

Scene::~Scene()
{
    sp::Scene::get("INGAME_MENU")->disable();
}

void Scene::onUpdate(float delta)
{
}
