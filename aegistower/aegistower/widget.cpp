#include "widget.h"
#include "sgg/graphics.h"
#include "globalstate.h"
Widget::Widget()
    : m_widget_width(0.0f), m_widget_height(0.0f),
    m_widget_x(0.0f), m_widget_y(0.0f),
    m_font_text(""),
    m_color_r(0.0f), m_color_g(0.0f), m_color_b(0.0f),
    m_is_hovered(false), m_is_clicked(false), 
    m_font_path(Globalstate::getInstance()->getFontsDir()),
    m_font_size(0.0f),
    m_font_pos_x(0.0f),
    m_font_pos_y(0.0f),
    m_text_r(0.0f),
    m_text_g(0.0f),
    m_text_b(0.0f),
    m_text_alignment_h(CENTER),
    m_text_alignment_v(CENTER),
    m_interactive(true)
{
    m_widget_brush.fill_opacity = 1.0f;
    m_widget_brush.outline_opacity = 0.0f;
    m_font_brush.fill_opacity = 1.0f;
    m_font_brush.fill_color[0] = m_text_r;
    m_font_brush.fill_color[1] = m_text_g;
    m_font_brush.fill_color[2] = m_text_b;
}
void Widget::setPosition(float x, float y) {
    m_widget_x = x;
    m_widget_y = y;
}
void Widget::setDimensions(float width, float height) {
    m_widget_width = width;
    m_widget_height = height;
}
void Widget::setText(const std::string& text) {
    m_font_text = text;
}
void Widget::setColor(float r, float g, float b) {
    m_color_r = r;
    m_color_g = g;
    m_color_b = b;
    updateBrushColor();
}
bool Widget::check_mouse_pos(float mouse_x, float mouse_y) const {
    return mouse_x >= m_widget_x &&
        mouse_x <= m_widget_x + m_widget_width &&
        mouse_y >= m_widget_y &&
        mouse_y <= m_widget_y + m_widget_height;
}
bool Widget::check_if_hovered() {
    float m_x = Globalstate::getInstance()->getMouse_pos_x();
    float m_y = Globalstate::getInstance()->getMouse_pos_y();
    m_is_hovered = check_mouse_pos(m_x, m_y);
    return m_is_hovered;
}
bool Widget::check_if_clicked() {
    return m_is_clicked;
}
void Widget::setInteractive(bool interactive) {
    m_interactive = interactive;
}
graphics::Brush Widget::get_widget_brush() const {
    return m_widget_brush;
}
void Widget::restore_brush_properties() {
    m_widget_brush.fill_opacity = 1.0f;
    m_widget_brush.outline_color[0] = 0.0f;
    m_widget_brush.outline_color[1] = 0.0f;
    m_widget_brush.outline_color[2] = 0.0f;
    m_widget_brush.outline_opacity = 1.0f;
    m_widget_brush.outline_width = 2.0f;
}
void Widget::set_brush_properties() {
    m_widget_brush.fill_opacity = 0.5f;
    m_widget_brush.outline_color[0] = 1.0f;
    m_widget_brush.outline_color[1] = 1.0f;
    m_widget_brush.outline_color[2] = 1.0f;
    m_widget_brush.outline_opacity = 1.0f;
    m_widget_brush.outline_width = 8.0f;
}
void Widget::set_font_text(std::string text) {
    m_font_text = text;
}
void Widget::set_font_size(float size) {
    m_font_size = size;
}
void Widget::calculate_text_position() {
    switch (m_text_alignment_h) {
    case LEFT:
        m_font_pos_x = m_widget_x + 5.0f;
        break;
    case CENTER:
        m_font_pos_x = m_widget_x + (m_widget_width / 2.0f) - (m_font_size * m_font_text.length()) / 4.0f;
        break;
    case RIGHT:
        m_font_pos_x = m_widget_x + m_widget_width + 20.0f;
        break;
    default:
        break;
    }
    switch (m_text_alignment_v) {
    case BOTTOM:
        m_font_pos_y = m_widget_y + m_widget_height + 25.0f;
        break;
    case CENTER:
        m_font_pos_y = m_widget_y + (m_widget_height / 2.0f) + m_font_size / 2.0f;
        break;
    case UP:
        m_font_pos_y = m_widget_y + m_font_size + 5.0f;
        break;
    default:
        break;
    }
}
void Widget::set_text_alignment_h(Text_alignment hor, Text_alignment ver) {
    m_text_alignment_h = hor;
    m_text_alignment_v = ver;
}
void Widget::update(float mouse_x, float mouse_y, bool mouse_clicked) {
    if (!m_interactive) return;
    m_is_hovered = check_mouse_pos(mouse_x, mouse_y);
    if (m_is_hovered && mouse_clicked) {
        m_is_clicked = true;
    } else {
        m_is_clicked = false;
    }
    updateBrushColor();
}
void Widget::draw() {
    graphics::drawRect(m_widget_x + m_widget_width / 2.0f,
        m_widget_y + m_widget_height / 2.0f,
        m_widget_width, m_widget_height,
        m_widget_brush);
    graphics::setFont(m_font_path);
    calculate_text_position();
    graphics::drawText(m_font_pos_x, m_font_pos_y, m_font_size, m_font_text, m_font_brush);
}
