#pragma once
#include "sgg/graphics.h"
#include <string>
class Widget
{
public:
    enum Text_alignment
    {
        LEFT,
        CENTER,
        RIGHT,
        BOTTOM,
        UP
    };  
    Widget();
    virtual ~Widget() = default;
    void setPosition(float x, float y);
    void setDimensions(float width, float height);
    void setText(const std::string& text);
    void setColor(float r, float g, float b);
    void setHoverColor(float r, float g, float b);
    void setClickColor(float r, float g, float b);
    bool check_mouse_pos(float mouse_x, float mouse_y) const;
    bool check_if_hovered();
    bool check_if_clicked();
    void setInteractive(bool interactive);
    graphics::Brush get_widget_brush() const;
    void restore_brush_properties();
    void set_brush_properties();
    void set_font_text(std::string text);
    void set_font_size(float size);
    void calculate_text_position();
    void set_text_alignment_h(Text_alignment hor, Text_alignment ver);
    virtual void update(float mouse_x, float mouse_y, bool mouse_clicked);
    virtual void draw();
protected:
    void updateBrushColor() {
        if (m_is_hovered) {
            set_brush_properties();
        } else if (!m_is_clicked) {
            restore_brush_properties();
        }
    }
    float m_widget_width;
    float m_widget_height;
    float m_widget_x;
    float m_widget_y;
    float m_color_r, m_color_g, m_color_b;
    float m_hover_color_r, m_hover_color_g, m_hover_color_b;
    float m_click_color_r, m_click_color_g, m_click_color_b;
    bool m_is_hovered;
    bool m_is_clicked;
    graphics::Brush m_widget_brush;
    std::string m_font_path;
    std::string m_font_text;
    float m_font_size;
    float m_font_pos_x;
    float m_font_pos_y;
    float m_text_r, m_text_g, m_text_b;
    graphics::Brush m_font_brush;
    Text_alignment m_text_alignment_h;
    Text_alignment m_text_alignment_v;
    bool m_interactive;
};
