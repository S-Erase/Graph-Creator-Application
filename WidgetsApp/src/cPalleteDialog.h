#pragma once
#include "cPCH.h"
#include <wx/clrpicker.h>
#include "cDisplay.h"

class cPalleteDialog : public wxDialog
{
public:
	cPalleteDialog(cDisplay* parent);
private:
	cDisplay* m_parent;

	wxColourPickerCtrl* bg_picker;
	wxColourPickerCtrl* grid_picker;
	wxColourPickerCtrl* vertex_picker;
	wxColourPickerCtrl* svertex_picker;

	void OnOk(wxCommandEvent& event);
	void OnApply(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
};

