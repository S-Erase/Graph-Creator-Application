#include "cPCH.h"
#include "cApp.h"
#include "cMain.h"

wxIMPLEMENT_APP(cApp);

bool cApp::OnInit()
{
    cMain* frame = new cMain();
    frame->Show(true);
    return true;
}