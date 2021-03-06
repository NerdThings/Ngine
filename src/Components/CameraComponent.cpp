/**********************************************************************************************
*
*   Ngine - The 2D game engine.
*
*   Copyright (C) 2019 NerdThings
*
*   LICENSE: Apache License 2.0
*   View: https://github.com/NerdThings/Ngine/blob/master/LICENSE
*
**********************************************************************************************/

#include "CameraComponent.h"

#include "../Graphics/Camera.h"
#include "../BaseEntity.h"

namespace NerdThings::Ngine::Components {
    // Private Methods

    void CameraComponent::UpdateCamera(EntityTransformChangedEventArgs e) {
        // Update the target
        _Camera.Position = e.EntityPosition;

        // We ignore scale and rotation as this is independently controlled

        // TODO: Should entity origin be used for the camera??
    }

    // Destructor

    CameraComponent::~CameraComponent() {
        _TransformChangeEvent.Detach();
    }

    // Public Constructors

    CameraComponent::CameraComponent(BaseEntity *parent_, const float zoom_, const Vector2 origin_,
                                     const float rotation_)
        : Component(parent_) {
        auto par = GetParent<BaseEntity>();

        // Setup camera
        _Camera = Graphics::Camera(par->GetPosition(), rotation_, zoom_, origin_);;

        // Attach to on position changed
        _TransformChangeEvent = par->OnTransformChanged +=
                new ClassMethodEventHandler<CameraComponent, EntityTransformChangedEventArgs>(this, &CameraComponent::UpdateCamera);
    }

    // Public Methods

    void CameraComponent::Activate() {
        GetParent<BaseEntity>()->GetParentScene()->SetActiveCamera(&_Camera);
    }

    void CameraComponent::SetOrigin(const Vector2 origin_) {
        _Camera.Origin = origin_;
    }

    void CameraComponent::SetRotation(const float rotation_) {
        _Camera.Rotation = rotation_;
    }

    void CameraComponent::SetZoom(const float zoom_) {
        _Camera.Zoom = zoom_;
    }
}
