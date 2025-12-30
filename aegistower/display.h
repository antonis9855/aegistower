#pragma once
#include "widget.h"
#include "globalstate.h"
#include <string>
class ResourceDisplay : public Widget
{
public:
    ResourceDisplay(std::string texture = "", bool m_interactive = false, 
                    float pos_x = 320.0f, float pos_y = 890.0f,
                    float width = 1280.0f, float height = 170.0f, 
                    float r = 194.0f, float g = 151.0f, float b = 112.0f)
        : Widget()
    {
        this->setColor(r, g, b);
        this->setDimensions(width, height);
        this->setPosition(pos_x, pos_y);
        this->setInteractive(m_interactive);
        m_widget_brush.fill_color[0] = m_color_r / 255.0f;
        m_widget_brush.fill_color[1] = m_color_g / 255.0f;
        m_widget_brush.fill_color[2] = m_color_b / 255.0f;
        if (!texture.empty()) {
            m_widget_brush.texture = Globalstate::getInstance()->getImagesDir() + texture;
        }
    }
};
