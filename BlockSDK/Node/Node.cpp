//
// Created by Ruhaan on 29/07/25.
//

#include "Node.h"
#include "Graphics/Graphics.h"
#include "Widget/Widget.h"

#include "../../Canvas/Scene/CanvasScene.h"
#include "../Scene/NodeScene.h"

Node::Node(Scene *scene_, const std::string &title) : scene(scene_) {

    content = new WidgetNode();

    grNode = new GraphicsNode(this);
    grNode->setTitle(title);
    scene->addNode(this);
    scene->grScene->addItem(grNode);



}

