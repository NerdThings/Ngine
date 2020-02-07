/**********************************************************************************************
*
*   Ngine - A 2D game engine.
*
*   Copyright 2020 NerdThings (Reece Mackie)
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
**********************************************************************************************/

#include "components/CameraComponent.hpp"

#include "Entity.hpp"

namespace ngine::components {
    void CameraComponent::_updateCamera(EntityTransformChangedEventArgs e) {
        // Update the target
        m_camera.Position = e.EntityTransformation.Position;
    }

    CameraComponent::~CameraComponent() {
        m_transformChangeEvent.detach();
    }

    CameraComponent::CameraComponent(Entity *parent, const float zoom, const Vector2 origin,
                                     const float rotation)
        : Component(parent) {
        auto par = getEntity<Entity>();

        // Setup camera
        m_camera = graphics::Camera(par->getPosition(), rotation, zoom, origin);;

        // Attach to on position changed
        m_transformChangeEvent = par->OnTransformChanged +=
                new ClassMethodEventHandler<CameraComponent, EntityTransformChangedEventArgs>(this,
                                                                                              &CameraComponent::_updateCamera);
    }

    void CameraComponent::activate() {
        getScene()->setActiveCamera(&m_camera);
    }

    Vector2 CameraComponent::getOrigin() {
        return m_camera.Origin;
    }

    void CameraComponent::setOrigin(Vector2 origin) {
        m_camera.Origin = origin;
    }

    float CameraComponent::getRotation() {
        return m_camera.Rotation;
    }

    void CameraComponent::setRotation(const float rotation) {
        m_camera.Rotation = rotation;
    }

    float CameraComponent::getZoom() {
        return m_camera.Zoom;
    }

    void CameraComponent::setZoom(const float zoom) {
        m_camera.Zoom = zoom;
    }
}
