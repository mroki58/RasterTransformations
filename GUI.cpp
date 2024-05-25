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

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	bSizer6->SetMinSize( wxSize( 130,-1 ) );
	RotateButton1 = new wxButton( this, wxID_ANY, wxT("Rotate\n( in image plane)"), wxDefaultPosition, wxSize( 130,50 ), 0 );
	bSizer6->Add( RotateButton1, 0, wxALL, 5 );

	RotateButton2 = new wxButton( this, wxID_ANY, wxT("Rotate\n (in other planes)"), wxDefaultPosition, wxSize( 130,50 ), 0 );
	bSizer6->Add( RotateButton2, 0, wxALL, 5 );

	ShearButton = new wxButton( this, wxID_ANY, wxT("Shear"), wxDefaultPosition, wxSize( 130,30 ), 0 );
	bSizer6->Add( ShearButton, 0, wxALL, 5 );

	DistortionButton = new wxButton( this, wxID_ANY, wxT("Other image\n distortions"), wxPoint( -1,-1 ), wxSize( 130,50 ), 0 );
	bSizer6->Add( DistortionButton, 0, wxALL, 5 );


	bSizer1->Add( bSizer6, 0, 0, 5 );

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
}

MyFrame1::~MyFrame1()
{
}
