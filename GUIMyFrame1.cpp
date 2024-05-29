#include "GUIMyFrame1.h"
#include "vecmat.h"
#include <fstream>

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
	ImgScrolledWindow->SetScrollbars(25, 25, 52, 40);
	Img_Cpy = new wxImage();
}

void GUIMyFrame1::LoadButtonOnButtonClick( wxCommandEvent& event )
{
	wxInitAllImageHandlers();

	wxFileDialog* file_dialog = new wxFileDialog(this, "Load file", "", "", wxT("to jpg (*.jpg)|*.jpg"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (file_dialog->ShowModal() == wxID_CANCEL)
	{
	return;
	}

	Img_Org.LoadFile(file_dialog->GetPath());
	Img_Cpy = new wxImage(Img_Org.Copy());

	wxClientDC dc(ImgScrolledWindow);
	dc.Clear();
	Draw();

}

void GUIMyFrame1::RotateButton1OnButtonClick( wxCommandEvent& event )
{
	wxDialog* dialog = new wxDialog(this, wxID_ANY, "Options", wxDefaultPosition, wxSize(300, 340), wxDEFAULT_DIALOG_STYLE);

    wxPanel* panel = new wxPanel(dialog, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC);

	wxTextCtrl* textCtrl1 = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxTextCtrl* textCtrl2 = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxTextCtrl* textCtrl3 = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, validator);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle:"), 0, wxALL, 5);
    sizer->Add(textCtrl1, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter x-coordinate of center of rotation:"), 0, wxALL, 5);
    sizer->Add(textCtrl2, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter y-coordinate of center of rotation:"), 0, wxALL, 5);
	sizer->Add(textCtrl3, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, ""), 0, wxALL, 5);

	sizer->Add(new wxButton(panel, wxID_OK), 0, wxALIGN_CENTER, 5);

	panel->SetSizer(sizer);
	panel->Fit();

	dialog->ShowModal();
    dialog->Destroy();

	RotateImagePlane(atof(textCtrl1->GetValue()), atoi(textCtrl2->GetValue()), atoi(textCtrl3->GetValue()));
}

void GUIMyFrame1::RotateImagePlane(double angle, int x, int y)
{
	wxClientDC dc(ImgScrolledWindow);
	dc.Clear();
	*Img_Cpy = Img_Cpy->Rotate(angle * M_PI / 180, wxPoint(x,y), true, 0);
	Draw();
}

void GUIMyFrame1::RotateButton2OnButtonClick( wxCommandEvent& event )
{
	wxClientDC dc(ImgScrolledWindow);
	dc.Clear();
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();

	int size = width * height;
	unsigned char* old = Img_Cpy->GetData(); // stare dane z obrazka
	
	Vector4* vectors = new Vector4[size];
	
	double angleY = 0 * M_PI/180; // na razie zak³adam sta³e wartoœci k¹ta
	double angleZ = 30 * M_PI/180;

	// wczytanie wartoœci kolorów dla wektorów
	for (int i = 0; i < size; ++i)
	{
		vectors[i].SetColor(old[3 * i], old[3 * i + 1], old[3 * i + 2]);
		vectors[i].data[0] = i % width - width/2;
		vectors[i].data[1] = height/2 - (i / width);
		vectors[i].data[2] = 0;
	}
	

	Matrix4 Yaxis;
	Matrix4 Zaxis;

	Yaxis.data[0][0] = cos(angleY);
	Yaxis.data[0][2] = sin(angleY);
	Yaxis.data[1][1] = 1;
	Yaxis.data[2][0] = -sin(angleY);
	Yaxis.data[2][2] = cos(angleY);
	Yaxis.data[3][3] = 1;

	Zaxis.data[0][0] = 1;
	Zaxis.data[1][1] = cos(angleZ);
	Zaxis.data[1][2] = -sin(angleZ);
	Zaxis.data[2][1] = sin(angleZ);
	Zaxis.data[2][2] = cos(angleZ);
	Zaxis.data[3][3] = 1;


	for (int i = 0; i < size; ++i)
	{
		vectors[i] = Zaxis * Yaxis * vectors[i];
		vectors[i].data[0] += width / 2;
		vectors[i].data[1] += height / 2;
	}

	int new_width = (int)(width);
	int new_height = (int)(height);

	delete Img_Cpy;
	Img_Cpy = new wxImage(new_width, new_height);


	for (int i = 0; i < size; ++i)
	{
		Img_Cpy->SetRGB(vectors[i].GetX(), height - vectors[i].GetY(), vectors[i].GetRed(), vectors[i].GetGreen(), vectors[i].GetBlue());
	}

	delete [] vectors;
	Draw();
}

void GUIMyFrame1::ShearButtonOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement ShearButtonOnButtonClick
}

void GUIMyFrame1::DistortionButtonOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement DistortionButtonOnButtonClick
}

void GUIMyFrame1::SaveButtonOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement SaveButtonOnButtonClick
}

void GUIMyFrame1::ImgScrolledWindowOnUpdateUI( wxUpdateUIEvent& event )
{
	Draw();
}


void GUIMyFrame1::Draw()
{
	wxBitmap bitmap(*Img_Cpy);          // Tworzymy tymczasowa bitmape na podstawie Img_Cpy
	wxClientDC dc(ImgScrolledWindow);   // Pobieramy kontekst okna
	ImgScrolledWindow->DoPrepareDC(dc); // Musimy wywolac w przypadku wxScrolledWindow, zeby suwaki prawidlowo dzialaly
	dc.DrawBitmap(bitmap, 0, 0, true); // Rysujemy bitmape na kontekscie urzadzenia
}