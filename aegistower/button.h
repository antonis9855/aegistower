#pragma once
#include "widget.h"
#include <string>
#include "globalstate.h"
// Button widget me typous kai text
class Button : public Widget
{
public:
    enum Button_type
    {
        Tower_button,
        HEALTH,
        PAUSE,
        INVENTORY,
        PLAY,
        QUIT,
        SETTINGS,
        DIFFICULTY_EASY,
        DIFFICULTY_MEDIUM,
        DIFFICULTY_HARD,
        BACK,
        START_WAVE
    };
    Button(Button_type m_type, float pos_x, float pos_y, std::string m_button_texture,
           std::string m_text = "", int m_gold = 0,
           float m_dimension_x = 120.0f, float m_dimension_y = 120.0f,
           Text_alignment h = CENTER, Text_alignment v = BOTTOM,
           float m_font_size = 23.0f, float r = 194.0f, float g = 151.0f, float b = 112.0f,
           bool m_interactiv = true)
        : Widget()
    {
        this->setColor(r, g, b);
        this->setDimensions(m_dimension_x, m_dimension_y);
        this->setPosition(pos_x, pos_y);
        this->set_font_text(m_text);
        this->set_text_alignment_h(h, v);
        this->set_font_size(m_font_size);
        this->setInteractive(m_interactiv);
        m_font_brush.fill_color[0] = 1.0f;
        m_font_brush.fill_color[1] = 1.0f;
        m_font_brush.fill_color[2] = 1.0f;
        m_gold_worth = m_gold;
        m_type_of_button = m_type;
        m_texture_name = m_button_texture;
        m_widget_brush.fill_color[0] = m_color_r / 255.0f;
        m_widget_brush.fill_color[1] = m_color_g / 255.0f;
        m_widget_brush.fill_color[2] = m_color_b / 255.0f;
        if (!m_button_texture.empty()) {
            m_widget_brush.texture = Globalstate::getInstance()->getImagesDir() + m_button_texture;
        }
    }

    Button_type getButtonType() const { return m_type_of_button; }
    int getGoldWorth() const { return m_gold_worth; }

private:
    Button_type m_type_of_button;
    std::string m_texture_name;
    int m_gold_worth;
};
