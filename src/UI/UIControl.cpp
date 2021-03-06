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

#include "UIControl.h"

#include "../Graphics/Renderer.h"
#include "UIPanel.h"

namespace NerdThings::Ngine::UI {
    // Private Fields

    std::unordered_map<std::type_index, TUIStyle> UIControl::_GlobalStyles;

    // Destructor

    UIControl::~UIControl() {
        for (auto child : _ChildrenOrdered) {
            delete child;
        }
        _Children.clear();
        _ChildrenOrdered.clear();
    }

    // Public Method(s)

    void UIControl::AddChild(std::string name, UIControl *control_) {
        if (_ChildrenConfig == 0)
            throw std::runtime_error("This control cannot have children.");
        if (_ChildrenConfig == 1 && _ChildrenOrdered.size() > 0)
            throw std::runtime_error("Cannot add more than one child to this control.");

        if (_Children.find(name) != _Children.end())
            throw std::runtime_error("Cannot add a child by the same name.");

        _Children.insert({name, control_});
        _ChildrenOrdered.push_back(control_);
        control_->_Parent = this;
    }

    void UIControl::Draw() {
        DrawStyles();

        DrawChildren();
    }

    void UIControl::DrawChildren() {
        // Draw children

        for (auto child : _ChildrenOrdered) {
            child->Draw();
        }
    }

    void UIControl::DrawStyles() {
        // Draw default styles

        auto style = GetStyle();

        auto controlRect = GetRenderRectangle();
        auto controlBorderRect = style.GetBorderRect(controlRect);
        auto controlBackgroundRect = style.GetBackgroundRect(controlRect);

        // Draw geometry
        if (style.DrawDefaults) {
            // Background
            if (style.BackgroundTexture != nullptr && style.BackgroundTexture->IsValid())
                Graphics::Renderer::DrawTexture(style.GetBackgroundTexture(),
                                               controlBackgroundRect,
                                               {
                                                       0,
                                                       0,
                                                       static_cast<float>(style.BackgroundTexture->Width),
                                                       static_cast<float>(style.BackgroundTexture->Height)
                                               },
                                               style.BackColor);
            else
                Graphics::Renderer::DrawRectangle(controlBackgroundRect, style.BackColor);

            // Border
            if (style.BorderThickness > 0) {
                Graphics::Renderer::DrawRectangleLines(controlBorderRect, style.BorderColor,
                                                      static_cast<int>(style.BorderThickness));
            }
        }
    }

    TUIStyle UIControl::GetGlobalStyle(std::type_index type_) {
        if (_GlobalStyles.find(type_) != _GlobalStyles.end())
            return _GlobalStyles[type_];
        return TUIStyle();
    }

    void UIControl::SetGlobalStyle(std::type_index type_, TUIStyle style_) {
        if (_GlobalStyles.find(type_) != _GlobalStyles.end())
            _GlobalStyles[type_] = style_;
        else
            _GlobalStyles.insert({type_, style_});
    }

    Vector2 UIControl::GetLogicPosition() {
        auto parPanel = GetParent<UIPanel>();
        if (parPanel == nullptr) {
            auto par = GetParent<UIControl>();
            auto parStyle = par->GetStyle();
            return parStyle.GetContentPosition(par->GetLogicPosition());
        } else {
            // Get panel setup
            auto pos = parPanel->GetLogicPosition();
            pos.X += parPanel->GetOffsetBeside(this);
            pos.Y += parPanel->GetOffsetAbove(this);
            return pos;
        }
    }

    Rectangle UIControl::GetLogicRectangle() {
        return Rectangle(GetLogicPosition(), GetWidth(), GetHeight());
    }

    Vector2 UIControl::GetRenderPosition() {
        auto parPanel = GetParent<UIPanel>();
        if (parPanel == nullptr) {
            auto par = GetParent<UIControl>();
            auto parStyle = par->GetStyle();
            return parStyle.GetContentPosition(par->GetRenderPosition());
        } else {
            // Get panel setup
            auto pos = Vector2::Zero;
            pos.X += parPanel->GetOffsetBeside(this);
            pos.Y += parPanel->GetOffsetAbove(this);
            return pos;
        }
    }

    Rectangle UIControl::GetRenderRectangle() {
        return Rectangle(GetRenderPosition(), GetWidth(), GetHeight());
    }

    TUIStyle UIControl::GetStyle() {
        return _Style;
    }

    void UIControl::RemoveChild(std::string name) {
        if (_Children.find(name) == _Children.end())
            throw std::runtime_error("Cannot remove a child that does not exist.");
        auto child = _Children[name];
        child->_Parent = nullptr;
        _ChildrenOrdered.erase(std::remove(_ChildrenOrdered.begin(), _ChildrenOrdered.end(), child), _ChildrenOrdered.end());
        _Children.erase(name);
    }

    void UIControl::SetStyle(TUIStyle style_) {
        _Style = style_;
    }

    void UIControl::Update() {
        for (auto child : _ChildrenOrdered) {
            child->Update();
        }
    }
}