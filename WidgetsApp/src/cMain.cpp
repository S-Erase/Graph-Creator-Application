#include "cPCH.h"
#include <wx/wfstream.h>
#include "cMain.h"
#include "cTestDialog.h"
#include "cPalleteDialog.h"
#include "cControlPanel.h"

#include <vector>

enum {ID_Lengths, ID_ResetView, ID_Pallete, ID_SelectAll, ID_ToggleSel, ID_Desel, ID_DeleteSel};

cMain::cMain()
    : wxFrame(NULL, wxID_ANY, "Travelling Salesman Problem Solver",wxDefaultPosition,wxSize(960,640))
{
    
    SetIcon(wxIcon("src/assets/icons/Icon.ico", wxBITMAP_TYPE_ICO));

    wxMenu* m_menuFile = new wxMenu;
    m_menuFile->Append(wxID_NEW);
    m_menuFile->Append(wxID_OPEN);
    m_menuFile->Append(wxID_SAVE);
    m_menuFile->AppendSeparator();
    m_menuFile->Append(wxID_EXIT);

    wxMenu* m_menuEdit = new wxMenu;
    //m_menuEdit->Append(ID_Lengths, "Edit Weights");
    m_menuEdit->Append(ID_DeleteSel, "Delete Selection..\tDel");

    wxMenu* m_menuView = new wxMenu;
    m_menuView->Append(ID_ResetView, "&Reset View...\tCtrl+R");
    m_menuView->Append(ID_Pallete, "Change &Pallete...\tCtrl+P", "Change the colour pallete on the canvas.");

    wxMenu* m_menuSelect = new wxMenu;
    m_menuSelect->Append(ID_SelectAll, "Select &All...\tCtrl+A");
    m_menuSelect->Append(ID_Desel, "&Deselect...\tCtrl+D");

    wxMenu* m_menuHelp = new wxMenu;
    m_menuHelp->Append(wxID_ABOUT);

    wxMenuBar* m_menuBar = new wxMenuBar;
    m_menuBar->Append(m_menuFile, "&File");
    m_menuBar->Append(m_menuEdit, "&Edit");
    m_menuBar->Append(m_menuSelect, "&Selection");
    m_menuBar->Append(m_menuView, "&View");
    m_menuBar->Append(m_menuHelp, "&Help");
    SetMenuBar(m_menuBar);
    CreateStatusBar();
    //SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &cMain::OnNew, this, wxID_NEW);
    Bind(wxEVT_MENU, &cMain::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &cMain::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &cMain::OnEditLengths, this, ID_Lengths);
    Bind(wxEVT_MENU, &cMain::OnDeleteSelection, this, ID_DeleteSel);
    Bind(wxEVT_MENU, &cMain::OnEditPallete, this, ID_Pallete);
    Bind(wxEVT_MENU, &cMain::ResetView, this, ID_ResetView);
    Bind(wxEVT_MENU, &cMain::OnSelectAll, this, ID_SelectAll);
    Bind(wxEVT_MENU, &cMain::OnToggleSel, this, ID_ToggleSel);
    Bind(wxEVT_MENU, &cMain::OnDeselect, this, ID_Desel);
    Bind(wxEVT_MENU, &cMain::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &cMain::OnExit, this, wxID_EXIT);

    wxImage::AddHandler(new wxPNGHandler);

    wxToolBar* m_toolbar = CreateToolBar();
    wxBitmap bmpNew("src/assets/icons/Toolbar_New.png", wxBITMAP_TYPE_PNG);
    m_toolbar->AddTool(wxID_NEW, "New Graph", bmpNew);
    wxBitmap bmpOpen("src/assets/icons/Toolbar_Open.png", wxBITMAP_TYPE_PNG);
    m_toolbar->AddTool(wxID_OPEN, "Open Graph", bmpOpen);
    wxBitmap bmpSave("src/assets/icons/Toolbar_Save.png", wxBITMAP_TYPE_PNG);
    m_toolbar->AddTool(wxID_SAVE, "Save Graph", bmpSave);
    m_toolbar->AddSeparator();
    wxBitmap bmpToggleSel("src/assets/icons/Toolbar_ToggleSelect.png", wxBITMAP_TYPE_PNG);
    m_toolbar->AddTool(ID_ToggleSel, "Toggle Select", bmpToggleSel);
    wxBitmap bmpDesel("src/assets/icons/Toolbar_Deselect.png", wxBITMAP_TYPE_PNG);
    m_toolbar->AddTool(ID_Desel, "Deselect", bmpDesel);
    m_toolbar->AddSeparator();
    m_toolbar->Realize();

    m_panel = new wxPanel(this, -1);

    cControlPanel* ctrlpanel = new cControlPanel(this, -1, wxDefaultPosition, wxSize(200, -1));

    m_display = new cDisplay(m_panel);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(ctrlpanel, 0, wxEXPAND);
    hbox->Add(m_display, 1, wxEXPAND);
    m_panel->SetSizer(hbox);

    m_displaytimer = new cDisplayTimer(m_display);
    m_displaytimer->Start(10);
}
cMain::~cMain()
{
    delete m_displaytimer;
}

void cMain::OnNew(wxCommandEvent& event)
{
    m_display->GetGraph().Clear("New Graph");
    event.Skip();
}
void cMain::OnOpen(wxCommandEvent& event)
{
    wxFileDialog* openFileDialog = new wxFileDialog(this,wxT("Open .gra files"),"","","Graph files (*.gra)|*.gra", wxFD_OPEN| wxFD_FILE_MUST_EXIST);

    if (openFileDialog->ShowModal() == wxID_OK) {
        wxString fileName = openFileDialog->GetPath();
        wxFileInputStream file(fileName);
        m_display->GetGraph().ReadFromFile(file);
    }
    event.Skip();
}
void cMain::OnSave(wxCommandEvent& event)
{
    wxFileDialog* saveFileDialog = new wxFileDialog(this, wxT("Save .gra files"), "", "", "Graph files (*.gra)|*.gra", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog->ShowModal() == wxID_OK) {
        wxString fileName = saveFileDialog->GetPath();
        wxFileOutputStream file(fileName);
        m_display->GetGraph().WriteToFile(file);
    }
    event.Skip();
}
void cMain::OnToggleSel(wxCommandEvent& event)
{
    m_display->ToggleSelection();
    event.Skip();
}
void cMain::OnDeselect(wxCommandEvent& event)
{
    m_display->DeselectAll();
    event.Skip();
}
void cMain::OnDeleteSelection(wxCommandEvent& event)
{
    m_display->DeleteSelection();
    event.Skip();
}
void cMain::OnSelectAll(wxCommandEvent& event)
{
    m_display->SelectAll();
    event.Skip();
}
void cMain::OnEditLengths(wxCommandEvent& event)
{
    cTestDialog* dialog = new cTestDialog(this);
    dialog->Show();
    event.Skip();
}
void cMain::OnEditPallete(wxCommandEvent& event)
{
    cPalleteDialog* dialog = new cPalleteDialog(m_display);
    dialog->Show();
    event.Skip();
}
void cMain::OnExit(wxCommandEvent& event)
{
    m_displaytimer->Stop();
    Close(true);
    event.Skip();
}
void cMain::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This application allows the flexible creation of different graphs of vertices on which to apply the Travelling Salesman Problem."
        , "About this Application", wxOK | wxICON_INFORMATION);

    event.Skip();
}
