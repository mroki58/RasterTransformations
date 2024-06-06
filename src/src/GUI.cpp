///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.1.0-0-g733bf3d)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUI.h"

///////////////////////////////////////////////////////////////////////////

MyFrame1::MyFrame1( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxSize( -1,-1 ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer2;
	wxBoxSizer* bSizerGrid;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	bSizerGrid = new wxBoxSizer(wxHORIZONTAL);

	bSizer2->SetMinSize( wxSize( 130,-1 ) );
	LoadButton = new wxButton( this, wxID_ANY, wxT("Load Image"), wxDefaultPosition, wxSize( 130,30 ), 0 );
	bSizer2->Add( LoadButton, 0, wxALL, 5 );

	RotateButton1 = new wxButton( this, wxID_ANY, wxT("Rotate\n(in image plane)"), wxDefaultPosition, wxSize( 130,50 ), 0 );
	bSizer2->Add( RotateButton1, 0, wxALL, 5 );

	RotateButton2 = new wxButton( this, wxID_ANY, wxT("Rotate\n (in other planes)"), wxDefaultPosition, wxSize( 130,50 ), 0 );
	bSizer2->Add( RotateButton2, 0, wxALL, 5 );

	ShearButton = new wxButton( this, wxID_ANY, wxT("Shear"), wxDefaultPosition, wxSize( 130,30 ), 0 );
	bSizer2->Add( ShearButton, 0, wxALL, 5 );

	DistortionButton = new wxButton( this, wxID_ANY, wxT("Other image\n distortions"), wxPoint( -1,-1 ), wxSize( 130,50 ), 0 );
	bSizer2->Add( DistortionButton, 0, wxALL, 5 );

	SaveButton = new wxButton( this, wxID_ANY, wxT("Save image"), wxDefaultPosition, wxSize( 130,30 ), 0 );
	bSizer2->Add( SaveButton, 0, wxALL, 5 );

	GridBox = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
	bSizerGrid->Add(GridBox, 0, wxALL, 5);
	bSizerGrid->Add(new wxStaticText(this, wxID_ANY, "Toggle grid"), 0, wxALL, 5);

	bSizer2->Add(bSizerGrid, 0, wxALL, 5);

	bSizer1->Add( bSizer2, 0, 0, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	ImgScrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxSize( 500,500 ), wxHSCROLL|wxVSCROLL );
	ImgScrolledWindow->SetScrollRate( 5, 5 );
	ImgScrolledWindow->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );

	bSizer3->Add( ImgScrolledWindow, 1, wxEXPAND | wxALL, 5 );


	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	LoadButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::LoadButtonOnButtonClick ), NULL, this );
	RotateButton1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::RotateButton1OnButtonClick ), NULL, this );
	RotateButton2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::RotateButton2OnButtonClick ), NULL, this );
	ShearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::ShearButtonOnButtonClick ), NULL, this );
	DistortionButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::DistortionButtonOnButtonClick ), NULL, this );
	SaveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::SaveButtonOnButtonClick ), NULL, this );
	GridBox->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(MyFrame1::GridBoxToggle), NULL, this);
	ImgScrolledWindow->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MyFrame1::ImgScrolledWindowOnUpdateUI ), NULL, this );
}

MyFrame1::~MyFrame1()
{
	// Disconnect Events
	LoadButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::LoadButtonOnButtonClick ), NULL, this );
	RotateButton1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::RotateButton1OnButtonClick ), NULL, this );
	RotateButton2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::RotateButton2OnButtonClick ), NULL, this );
	ShearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::ShearButtonOnButtonClick ), NULL, this );
	DistortionButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::DistortionButtonOnButtonClick ), NULL, this );
	SaveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::SaveButtonOnButtonClick ), NULL, this );
	GridBox->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame1::GridBoxToggle), NULL, this);
	ImgScrolledWindow->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MyFrame1::ImgScrolledWindowOnUpdateUI ), NULL, this );

}
