#pragma once
#include "cPCH.h"
class cTestDialog : public wxDialog
{
public:
	cTestDialog(wxWindow* parent);
private:
	void OnClose(wxCommandEvent& event);
};

