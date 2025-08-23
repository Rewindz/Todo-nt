#pragma once

#include <wx/wx.h>

class InvisibleScrollWindow : public wxScrolledWindow
{
public:
    InvisibleScrollWindow(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxVSCROLL)
        : wxScrolledWindow(parent, id, pos, size, style)
    {
        // Enable mouse wheel scrolling
        Bind(wxEVT_MOUSEWHEEL, &InvisibleScrollWindow::OnMouseWheel, this);
        Bind(wxEVT_SIZE, &InvisibleScrollWindow::OnSize, this);
    }

protected:
    void OnMouseWheel(wxMouseEvent& event)
    {
        int rotation = event.GetWheelRotation();
        int delta = event.GetWheelDelta();
        int linesPerAction = event.GetLinesPerAction();

        int scrollUnits = -(rotation / delta) * linesPerAction;

        int currentX, currentY;
        GetViewStart(&currentX, &currentY);

        Scroll(currentX, currentY + scrollUnits);
        event.Skip(false); // Don't propagate to prevent double scrolling
    }

    wxSize DoGetBestClientSize() const override
    {
        wxSize size = wxScrolledWindow::DoGetBestClientSize();
        // Add back the scrollbar width that would normally be subtracted
        if (HasScrollbar(wxVERTICAL))
            size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
        if (HasScrollbar(wxHORIZONTAL))
            size.y += wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y);
        return size;
    }

    void OnSize(wxSizeEvent& event)
    {
        event.Skip();

        CallAfter([this]() {
                wxWindowList& children = GetChildren();
                for (wxWindow* child : children) {
                    if (auto* casted = dynamic_cast<wxScrollBar*>(child)) {
                        child->Show(false);
                    }
                }
#ifdef __WXMSW__
                HWND hwnd = (HWND)GetHandle();
                ShowScrollBar(hwnd, SB_VERT, FALSE);
                ShowScrollBar(hwnd, SB_HORZ, FALSE);
#endif
            });

    }

};