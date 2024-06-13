///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.1.0-0-g733bf3d)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/wxprec.h>
#include <wx/dcbuffer.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame1
///////////////////////////////////////////////////////////////////////////////
class MyFrame1 : public wxFrame
{
	private:

	protected:
		wxButton* LoadButton;
		wxButton* RotateButton1;
		wxButton* RotateButton2;
		wxButton* ShearButton;
		wxButton* DistortionButton;
		wxButton* SaveButton;
		wxButton* MirrorX;
		wxButton* MirrorY;
		wxScrolledWindow* ImgScrolledWindow;
		//wxCheckBox* GridBox;

		// Virtual event handlers, override them in your derived class
		virtual void LoadButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void RotateButton1OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void RotateButton2OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void ShearButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void DistortionButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void SaveButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		//virtual void GridBoxToggle(wxCommandEvent& event) { event.Skip(); }
		virtual void MirrorButton3OnButtonClick(wxCommandEvent& event) { event.Skip(); }
		virtual void MirrorButton4OnButtonClick(wxCommandEvent& event) { event.Skip(); }
		virtual void ImgScrolledWindowOnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }


	public:

		MyFrame1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxBORDER_NONE|wxTAB_TRAVERSAL );

		~MyFrame1();

};

