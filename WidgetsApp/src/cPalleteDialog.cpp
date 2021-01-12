#include "cPCH.h"
#include "cPalleteDialog.h"

cPalleteDialog::cPalleteDialog(cDisplay* parent) : wxDialog((wxWindow*)parent, -1, "Colour Pallete Editor", wxDefaultPosition, wxSize(300, 300))
	, m_parent(parent)
{
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* stboxs = new wxStaticBoxSizer(wxVERTICAL, this, "Pallete Editor");
	wxGridSizer* grids = new wxGridSizer(2);

	wxStaticText* bg_text = new wxStaticText(this,-1,"Background Colour");
	bg_picker = new wxColourPickerCtrl(this, -1, parent->GetBGColor());

	wxStaticText* grid_text = new wxStaticText(this, -1, "Grid Colour");
	grid_picker = new wxColourPickerCtrl(this, -1, parent->GetGridColor());

	wxStaticText* vertex_text = new wxStaticText(this, -1, "Vertex Colour 1");
	vertex_picker = new wxColourPickerCtrl(this, -1, parent->GetVertexColor());

	wxStaticText* svertex_text = new wxStaticText(this, -1, "Vertex Colour 2");
	svertex_picker = new wxColourPickerCtrl(this, -1, parent->GetSelectedVertexColor());

	grids->Add(bg_text);
	grids->Add(bg_picker);
	grids->Add(grid_text);
	grids->Add(grid_picker);
	grids->Add(vertex_text);
	grids->Add(vertex_picker);
	grids->Add(svertex_text);
	grids->Add(svertex_picker);

	stboxs->Add(grids);

	wxButton* okButton = new wxButton(this, -1, wxT("Ok"),
		wxDefaultPosition, wxSize(70, 30));
	wxButton* applyButton = new wxButton(this, -1, wxT("Apply"),
		wxDefaultPosition, wxSize(70, 30));
	wxButton* closeButton = new wxButton(this, -1, wxT("Close"),
		wxDefaultPosition, wxSize(70, 30));

	Bind(wxEVT_BUTTON, &cPalleteDialog::OnOk, this, okButton->GetId());
	Bind(wxEVT_BUTTON, &cPalleteDialog::OnApply, this, applyButton->GetId());
	Bind(wxEVT_BUTTON, &cPalleteDialog::OnClose, this, closeButton->GetId());

	hbox->Add(okButton, 1);
	hbox->Add(applyButton, 1, wxLEFT, 5);
	hbox->Add(closeButton, 1, wxLEFT, 5);

	vbox->Add(stboxs, 1, wxEXPAND);
	vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	SetSizer(vbox);

	Centre();
	ShowModal();
	Destroy();
}

void cPalleteDialog::OnOk(wxCommandEvent& event)
{
	m_parent->SetBGColor(bg_picker->GetColour());
	m_parent->SetGridColor(grid_picker->GetColour());
	m_parent->SetVertexColor(vertex_picker->GetColour());
	m_parent->SetSelectedVertexColor(svertex_picker->GetColour());
	Close(true);
}

void cPalleteDialog::OnApply(wxCommandEvent& event)
{
	m_parent->SetBGColor(bg_picker->GetColour());
	m_parent->SetGridColor(grid_picker->GetColour());
	m_parent->SetVertexColor(vertex_picker->GetColour());
	m_parent->SetSelectedVertexColor(svertex_picker->GetColour());
	event.Skip();
}

void cPalleteDialog::OnClose(wxCommandEvent& event)
{
	Close(true);
}