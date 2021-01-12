#include "cPCH.h"
#include "cTestDialog.h"

cTestDialog::cTestDialog(wxWindow* parent) : wxDialog(parent, -1, "Graph Distance Editor", wxDefaultPosition, wxSize(260, 250))
{
	wxPanel* panel = new wxPanel(this, -1);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBox* st = new wxStaticBox(panel, -1, wxT("Distance Metrics"),
		wxPoint(5, 5), wxSize(240, 150));
	wxRadioButton* rb0 = new wxRadioButton(panel, -1,
		wxT("Euclidean metric"), wxPoint(15, 30), wxDefaultSize, wxRB_GROUP);

	wxRadioButton* rb1 = new wxRadioButton(panel, -1,
		wxT("Manhattan metric"), wxPoint(15, 55));
	wxRadioButton* rb2 = new wxRadioButton(panel, -1,
		wxT("Taxicab metric"), wxPoint(15, 80));
	wxRadioButton* rb3 = new wxRadioButton(panel, -1,
		wxT("L-p metric"), wxPoint(15, 105));
	wxTextCtrl* tc = new wxTextCtrl(panel, -1, wxT(""),
		wxPoint(95, 100));
	wxRadioButton* rb4 = new wxRadioButton(panel, -1,
		wxT("Customized metric"), wxPoint(15, 130));

	wxButton* okButton = new wxButton(this, -1, wxT("Ok"),
		wxDefaultPosition, wxSize(70, 30));
	wxButton* closeButton = new wxButton(this, -1, wxT("Close"),
		wxDefaultPosition, wxSize(70, 30));

	Bind(wxEVT_BUTTON, &cTestDialog::OnClose, this, closeButton->GetId());

	hbox->Add(okButton, 1);
	hbox->Add(closeButton, 1, wxLEFT, 5);

	vbox->Add(panel, 1);
	vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	SetSizer(vbox);

	Centre();
	ShowModal();
	Destroy();
}

void cTestDialog::OnClose(wxCommandEvent& event)
{
	Close(true);
}
