#include <LibGUI/GScrollableWidget.h>
#include <LibGUI/GScrollBar.h>

GScrollableWidget::GScrollableWidget(GWidget* parent)
    : GWidget(parent)
{
    m_vertical_scrollbar = new GScrollBar(Orientation::Vertical, this);
    m_vertical_scrollbar->set_step(4);
    m_vertical_scrollbar->on_change = [this] (int) {
        update();
    };

    m_horizontal_scrollbar = new GScrollBar(Orientation::Horizontal, this);
    m_horizontal_scrollbar->set_step(4);
    m_horizontal_scrollbar->set_big_step(30);
    m_horizontal_scrollbar->on_change = [this] (int) {
        update();
    };

    m_corner_widget = new GWidget(this);
    m_corner_widget->set_fill_with_background_color(true);
}

GScrollableWidget::~GScrollableWidget()
{
}

void GScrollableWidget::resize_event(GResizeEvent& event)
{
    update_scrollbar_ranges();

    m_vertical_scrollbar->set_relative_rect(event.size().width() - m_vertical_scrollbar->preferred_size().width(), 0, m_vertical_scrollbar->preferred_size().width(), event.size().height() - m_horizontal_scrollbar->preferred_size().height());
    m_horizontal_scrollbar->set_relative_rect(0, event.size().height() - m_horizontal_scrollbar->preferred_size().height(), event.size().width() - m_vertical_scrollbar->preferred_size().width(), m_horizontal_scrollbar->preferred_size().height());

    m_corner_widget->set_visible(m_vertical_scrollbar->is_visible() && m_horizontal_scrollbar->is_visible());
    if (m_corner_widget->is_visible()) {
        Rect corner_rect { m_horizontal_scrollbar->rect().right() + 1, m_vertical_scrollbar->rect().bottom() + 1, height_occupied_by_horizontal_scrollbar(), width_occupied_by_vertical_scrollbar() };
        m_corner_widget->set_relative_rect(corner_rect);
    }
}

void GScrollableWidget::update_scrollbar_ranges()
{
    int available_height = height() - m_size_occupied_by_fixed_elements.height() - height_occupied_by_horizontal_scrollbar();
    int excess_height = max(0, (m_content_size.height() + m_padding.height() * 2) - available_height);
    m_vertical_scrollbar->set_range(0, excess_height);

    int available_width = width() - m_size_occupied_by_fixed_elements.width() - width_occupied_by_vertical_scrollbar();
    int excess_width = max(0, (m_content_size.width() + m_padding.height() * 2) - available_width);
    m_horizontal_scrollbar->set_range(0, excess_width);

    m_vertical_scrollbar->set_big_step(visible_content_rect().height() - m_vertical_scrollbar->step());
}

void GScrollableWidget::set_content_size(const Size& size)
{
    if (m_content_size == size)
        return;
    m_content_size = size;
    update_scrollbar_ranges();
}

void GScrollableWidget::set_size_occupied_by_fixed_elements(const Size& size)
{
    if (m_size_occupied_by_fixed_elements == size)
        return;
    m_size_occupied_by_fixed_elements = size;
    update_scrollbar_ranges();
}

void GScrollableWidget::set_padding(const Size& size)
{
    if (m_padding == size)
        return;
    m_padding = size;
    update_scrollbar_ranges();
}

int GScrollableWidget::height_occupied_by_horizontal_scrollbar() const
{
    return m_horizontal_scrollbar->is_visible() ? m_horizontal_scrollbar->height() : 0;
}

int GScrollableWidget::width_occupied_by_vertical_scrollbar() const
{
    return m_vertical_scrollbar->is_visible() ? m_vertical_scrollbar->width() : 0;
}

Rect GScrollableWidget::visible_content_rect() const
{
    return {
        m_horizontal_scrollbar->value(),
        m_vertical_scrollbar->value(),
        width() - width_occupied_by_vertical_scrollbar() - padding().width() * 2 - m_size_occupied_by_fixed_elements.width(),
        height() - height_occupied_by_horizontal_scrollbar() - padding().height() * 2 - m_size_occupied_by_fixed_elements.height()
    };
}

void GScrollableWidget::scroll_into_view(const Rect& rect, Orientation orientation)
{
    if (orientation == Orientation::Vertical)
        return scroll_into_view(rect, false, true);
    return scroll_into_view(rect, true, false);
}

void GScrollableWidget::scroll_into_view(const Rect& rect, bool scroll_horizontally, bool scroll_vertically)
{
    auto visible_content_rect = this->visible_content_rect();
    if (visible_content_rect.contains(rect))
        return;

    if (scroll_vertically) {
        if (rect.top() < visible_content_rect.top())
            m_vertical_scrollbar->set_value(rect.top());
        else if (rect.bottom() > visible_content_rect.bottom())
            m_vertical_scrollbar->set_value(rect.bottom() - visible_content_rect.height());
    }
    if (scroll_horizontally) {
        if (rect.left() < visible_content_rect.left())
            m_horizontal_scrollbar->set_value(rect.left());
        else if (rect.right() > visible_content_rect.right())
            m_horizontal_scrollbar->set_value(rect.right() - visible_content_rect.width());
    }
}

void GScrollableWidget::set_scrollbars_enabled(bool scrollbars_enabled)
{
    if (m_scrollbars_enabled == scrollbars_enabled)
        return;
    m_scrollbars_enabled = scrollbars_enabled;
    m_vertical_scrollbar->set_visible(m_scrollbars_enabled);
    m_horizontal_scrollbar->set_visible(m_scrollbars_enabled);
    m_corner_widget->set_visible(m_scrollbars_enabled);
}
