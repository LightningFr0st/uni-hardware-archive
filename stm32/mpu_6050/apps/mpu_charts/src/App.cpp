#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    SetAppearance(Appearance::System);

    MainFrame *mainFrame = new MainFrame("MPU Charts");
    mainFrame->SetClientSize(1500, 1000);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}