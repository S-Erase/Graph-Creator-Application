#pragma once
#include "cPCH.h"
#include "cMain.h"
#include <wx/statline.h>
#include <functional>

class cControlPanel : public wxPanel
{
public:
    cControlPanel(cMain* frame, wxWindowID id, const wxPoint& pos, const wxSize& size)
        : wxPanel(frame->GetPanel(), id, pos, size)
    {
        m_main = frame;

        m_toolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_VERTICAL);
        wxBitmap bmpView("src/assets/icons/Toolbar_View.png", wxBITMAP_TYPE_PNG);
        m_toolbar->AddTool(100, "View Controls", bmpView);
        wxBitmap bmpCreate("src/assets/icons/Toolbar_Create.png", wxBITMAP_TYPE_PNG);
        m_toolbar->AddTool(101, "Creation Controls", bmpCreate);
        wxBitmap bmpSelect("src/assets/icons/Toolbar_Select.png", wxBITMAP_TYPE_PNG);
        m_toolbar->AddTool(102, "Selection Controls", bmpSelect);
        wxBitmap bmpEdit("src/assets/icons/Toolbar_Edit.png", wxBITMAP_TYPE_PNG);
        m_toolbar->AddTool(103, "Edit Controls", bmpEdit);
        m_toolbar->Realize();

        Bind(wxEVT_TOOL, &cControlPanel::OnTool, this, 100);
        Bind(wxEVT_TOOL, &cControlPanel::OnTool, this, 101);
        Bind(wxEVT_TOOL, &cControlPanel::OnTool, this, 102);
        Bind(wxEVT_TOOL, &cControlPanel::OnTool, this, 103);

        m_createctrl = new wxControl(this, -1);
        m_selectctrl = new wxControl(this, -1);
        m_viewctrl = new wxControl(this, -1);
        m_editctrl = new wxControl(this, -1);

        wxBoxSizer* m_viewbox = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* m_createbox = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* m_selectbox = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* m_editbox = new wxBoxSizer(wxVERTICAL);

        //View Controls
        wxButton* b_resetview = new wxButton(m_viewctrl, -1, "Reset View");
        wxCheckBox* b_showgrid = new wxCheckBox(m_viewctrl, -1, "Show Grid");
        b_showgrid->SetValue(true);
        m_viewbox->Add(b_resetview);
        m_viewbox->Add(b_showgrid);

        //Creation Controls
        wxButton* b_clear = new wxButton(m_createctrl, -1, "Clear Graph");
        wxButton* b_delsel = new wxButton(m_createctrl, -1, "Delete Selection");
        m_createbox->Add(b_clear);
        m_createbox->Add(b_delsel);

        //Selection Controls
        wxButton* b_selall = new wxButton(m_selectctrl, -1, "Select All");
        wxButton* b_dselall = new wxButton(m_selectctrl, -1, "Deselect All");
        wxButton* b_togsel = new wxButton(m_selectctrl, -1, "Toggle Selection");
        m_selectbox->Add(b_selall);
        m_selectbox->Add(b_dselall);
        m_selectbox->Add(b_togsel);
        wxRadioButton* rb_selnew = new wxRadioButton(m_selectctrl, 200, "New Selection", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        wxRadioButton* rb_seladd = new wxRadioButton(m_selectctrl, 201, "Add Selection");
        wxRadioButton* rb_selsub = new wxRadioButton(m_selectctrl, 202, "Subtract Selection");
        wxRadioButton* rb_seltog = new wxRadioButton(m_selectctrl, 203, "Toggle Selection");
        wxRadioButton* rb_selint = new wxRadioButton(m_selectctrl, 204, "Intersect Selection");
        m_selectbox->Add(rb_selnew);
        m_selectbox->Add(rb_seladd);
        m_selectbox->Add(rb_selsub);
        m_selectbox->Add(rb_seltog);
        m_selectbox->Add(rb_selint);

        Bind(wxEVT_BUTTON, &cControlPanel::ResetView, this, b_resetview->GetId());
        Bind(wxEVT_CHECKBOX, &cControlPanel::ToggleGrid, this, b_showgrid->GetId());
        Bind(wxEVT_BUTTON, &cControlPanel::ClearGraph, this, b_clear->GetId());
        Bind(wxEVT_BUTTON, &cControlPanel::DeleteSelection, this, b_delsel->GetId());
        Bind(wxEVT_BUTTON, &cControlPanel::ToggleSelection, this, b_togsel->GetId());
        Bind(wxEVT_BUTTON, &cControlPanel::SelectAll, this, b_selall->GetId());
        Bind(wxEVT_BUTTON, &cControlPanel::DeselectAll, this, b_dselall->GetId());

        Bind(wxEVT_RADIOBUTTON, &cControlPanel::SelectionSettings, this, rb_selnew->GetId());
        Bind(wxEVT_RADIOBUTTON, &cControlPanel::SelectionSettings, this, rb_seladd->GetId());
        Bind(wxEVT_RADIOBUTTON, &cControlPanel::SelectionSettings, this, rb_selsub->GetId());
        Bind(wxEVT_RADIOBUTTON, &cControlPanel::SelectionSettings, this, rb_seltog->GetId());
        Bind(wxEVT_RADIOBUTTON, &cControlPanel::SelectionSettings, this, rb_selint->GetId());

        m_viewctrl->SetSizer(m_viewbox);
        m_createctrl->SetSizer(m_createbox);
        m_selectctrl->SetSizer(m_selectbox);
        m_editctrl->SetSizer(m_editbox);

        m_ctrl = m_viewctrl;

        hbox = new wxBoxSizer(wxHORIZONTAL);

        hbox->Add(m_toolbar, 0, wxEXPAND);
        hbox->Add(m_ctrl, 1, wxEXPAND);
        SetSizer(hbox);
    }
private:
    cMain* m_main;
    wxToolBar* m_toolbar;
    wxControl* m_ctrl;

    wxStaticText* viewtext;

    wxControl* m_viewctrl;
    wxControl* m_createctrl;
    wxControl* m_selectctrl;
    wxControl* m_editctrl;

    wxBoxSizer* hbox;

    void OnTool(wxCommandEvent& event) {
        hbox->Detach(1);
        m_ctrl->Hide();
        switch (event.GetId()) {
        case 100:
            m_ctrl = m_viewctrl;
            hbox->Add(m_viewctrl, 1, wxEXPAND);
            m_main->GetDisplay()->SetTool(Tool::View);
            break;
        case 101:
            m_ctrl = m_createctrl;
            hbox->Add(m_createctrl, 1, wxEXPAND);
            m_main->GetDisplay()->SetTool(Tool::Create);
            break;
        case 102:
            m_ctrl = m_selectctrl;
            hbox->Add(m_selectctrl, 1, wxEXPAND);
            m_main->GetDisplay()->SetTool(Tool::Select);
            break;
        case 103:
            m_ctrl = m_editctrl;
            hbox->Add(m_editctrl, 1, wxEXPAND);
            m_main->GetDisplay()->SetTool(Tool::Edit);
            break;
        }
        m_ctrl->Show();
        hbox->Layout();
        Refresh();
        Update();
        event.Skip();
    }

    void ResetView(wxCommandEvent& event) {
        m_main->GetDisplay()->ResetView();
        event.Skip();
    }
    void ToggleGrid(wxCommandEvent& event) {
        m_main->GetDisplay()->ToggleGrid();
        event.Skip();
    }
    void ClearGraph(wxCommandEvent& event) {
        m_main->GetDisplay()->GetGraph().Clear("Clear Graph");
        event.Skip();
    }
    void SelectAll(wxCommandEvent& event) {
        m_main->GetDisplay()->SelectAll();
        event.Skip();
    }
    void DeselectAll(wxCommandEvent& event) {
        m_main->GetDisplay()->DeselectAll();
        event.Skip();
    }
    void ToggleSelection(wxCommandEvent& event) {
        m_main->GetDisplay()->ToggleSelection();
        event.Skip();
    }
    void DeleteSelection(wxCommandEvent& event) {
        m_main->GetDisplay()->DeleteSelection();
        event.Skip();
    }

    void SelectionSettings(wxCommandEvent& event) {
        switch (event.GetId()) {
        case 200:
            m_main->GetDisplay()->SetSelectionMode(SelectMode::New);
            break;
        case 201:
            m_main->GetDisplay()->SetSelectionMode(SelectMode::Add);
            break;
        case 202:
            m_main->GetDisplay()->SetSelectionMode(SelectMode::Subtract);
            break;
        case 203:
            m_main->GetDisplay()->SetSelectionMode(SelectMode::Toggle);
            break;
        case 204:
            m_main->GetDisplay()->SetSelectionMode(SelectMode::Intersect);
            break;
        }
        event.Skip();
    }
};

