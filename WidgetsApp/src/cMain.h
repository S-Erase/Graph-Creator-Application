#pragma once
#include "cPCH.h"
#include <wx/collpane.h>
#include "cDisplay.h"

class cMain : public wxFrame
{
public:
    cMain();
    ~cMain();

    inline wxPanel* GetPanel() { return m_panel; }
    inline cDisplay* GetDisplay() { return m_display; }
private:
    wxPanel* m_panel;
    cDisplay* m_display;
    cDisplayTimer* m_displaytimer;
private:
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnToggleSel(wxCommandEvent& event);
    void OnDeselect(wxCommandEvent& event);
    void OnDeleteSelection(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnEditLengths(wxCommandEvent& event);
    void OnEditPallete(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void EvtRefresh(wxCollapsiblePaneEvent& event) {
        Refresh();
    }

    void OnMouse(wxMouseEvent& event)
    {
        event.Skip();
    }

    void ResetView(wxCommandEvent& event) {
        m_display->ResetView();
        event.Skip();
    }
};

