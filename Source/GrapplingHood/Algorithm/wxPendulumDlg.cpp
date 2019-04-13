//// ---------------------
///// @author Martin Ettl
///// @date   2013-02-03
//// ---------------------
//
//#include "wxPendulumDlg.hpp"
//
//IMPLEMENT_APP(wxPendulumDlgApp)
//
//bool wxPendulumDlgApp::OnInit()
//{
//    wxPendulumDlg* dialog = new wxPendulumDlg(NULL);
//    SetTopWindow(dialog);
//    dialog->Show(true);
//    return true;
//}
//
//int wxPendulumDlgApp::OnExit()
//{
//    return 0;
//}
//
//BEGIN_EVENT_TABLE(wxPendulumDlg, wxDialog)
//    EVT_CLOSE(wxPendulumDlg::OnClose)
//    EVT_SIZE(wxPendulumDlg::wxPendulumDlgSize)
//    EVT_PAINT(wxPendulumDlg::wxPendulumDlgPaint)
//    EVT_TIMER(ID_WXTIMER1, wxPendulumDlg::OnTimer)
//END_EVENT_TABLE()
//
//wxPendulumDlg::wxPendulumDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
//    : wxDialog(parent, id, title, position, size, style)
//{
//    CreateGUIControls();
//}
//
//wxPendulumDlg::~wxPendulumDlg()
//{
//}
//
//void wxPendulumDlg::CreateGUIControls()
//{
//    SetIcon(wxNullIcon);
//    SetSize(8, 8, 509, 412);
//    Center();
//
//    m_uiLength = 200;
//    m_Angle    = M_PI / 2.;
//    m_AngleVelocity = 0;
//
//    m_timer = new wxTimer();
//    m_timer->SetOwner(this, ID_WXTIMER1);
//    m_timer->Start(20);
//}
//
//void wxPendulumDlg::OnClose(wxCloseEvent& WXUNUSED(event))
//{
//    Destroy();
//}
//
//void wxPendulumDlg::wxPendulumDlgPaint(wxPaintEvent& WXUNUSED(event))
//{
//    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
//    wxBufferedPaintDC pdc(this);
//    wxGCDC dc( pdc ) ;
//
//    // Get window dimensions
//    wxSize sz = GetClientSize();
//    // determine the center of the canvas
//    const wxRealPoint center(wxPoint(sz.x / 2.0, sz.y / 2.0));
//
//    // create background color
//    wxColour powderblue = wxColour(176, 224, 230);
//
//    // draw powderblue background
//    dc.SetPen(powderblue);
//    dc.SetBrush(powderblue);
//    dc.DrawRectangle(0, 0, sz.x, sz.y);
//
//    // draw lines
//    wxPen Pen(*wxBLACK_PEN);
//    Pen.SetWidth(1);
//    dc.SetPen(Pen);
//    dc.SetBrush(*wxBLACK_BRUSH);
//
//    const wxDouble dt = 0.15;
//    const wxDouble angleAccel = (-9.81 / m_uiLength) * sin(m_Angle);
//    m_AngleVelocity += angleAccel * dt;
//    m_Angle += m_AngleVelocity * dt;
//
//    const wxDouble anchorX = sz.x / 2.0, anchorY = sz.y / 4.0;
//    const wxDouble ballX = anchorX + (sin(m_Angle) * m_uiLength);
//    const wxDouble ballY = anchorY + (cos(m_Angle) * m_uiLength);
//    dc.DrawLine(anchorX, anchorY, ballX, ballY);
//
//    dc.SetBrush(*wxGREY_BRUSH);
//    dc.DrawEllipse(anchorX - 3.0, anchorY - 4.0, 7.0, 7.0);
//
//    dc.SetBrush(wxColour(255, 255, 0)); // yellow
//    dc.DrawEllipse(ballX - 7, ballY - 7, 20, 20);
//}
//
//void wxPendulumDlg::wxPendulumDlgSize(wxSizeEvent& WXUNUSED(event))
//{
//    Refresh();
//}
//
//void wxPendulumDlg::OnTimer(wxTimerEvent& WXUNUSED(event))
//{
//    // force refresh
//    Refresh();
//}
