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

void GUIMyFrame1::ShearButtonOnButtonClick(wxCommandEvent& event)
{
	wxDialog* dialog = new wxDialog(this, wxID_ANY, "Options", wxDefaultPosition, wxSize(300, 340), wxDEFAULT_DIALOG_STYLE);

	wxPanel* panel = new wxPanel(dialog, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC);

	wxTextCtrl* textCtrl1 = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxTextCtrl* textCtrl2 = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, validator);

	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter x-shear angle:"), 0, wxALL, 5);
	sizer->Add(textCtrl1, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter y-shear angle:"), 0, wxALL, 5);
	sizer->Add(textCtrl2, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, ""), 0, wxALL, 5);

	sizer->Add(new wxButton(panel, wxID_OK), 0, wxALIGN_CENTER, 5);

	panel->SetSizer(sizer);
	panel->Fit();

	dialog->ShowModal();
	dialog->Destroy();
	if (Img_Cpy->IsOk())
	{
		ShearX(atof(textCtrl1->GetValue()));
		ShearY(atof(textCtrl2->GetValue()));
	}

}

void GUIMyFrame1::ShearX(double angle)
{
	if (std::fmod(angle + 90, 180) == 0) return; //je¿eli k¹t prosty to pochylenie nie ma sensu bo obraz z³o¿y³by siê do lini prostej

	wxClientDC dc(ImgScrolledWindow);
	//dc.SetBackground(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
	dc.Clear();
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();


	double tangens = tan(angle * M_PI / 180.0);
	int dif_width = (double)height * tangens; //wartoœæ o ile zmieni siê szerokoœæ obrazka
	wxImage* Img_new = new wxImage(width + abs(dif_width), height);

	unsigned char* data = Img_Cpy->GetData();

	double xstart = abs((double)dif_width) / 2.0; // punkt referencyjny wzglêdem którego rysujê (przesuniêcie centralnego wiersza)

	for (int y = 0; y < height; y++)
	{
		int cur_dif_width = (double)((double)height / 2.0 - y) * tangens; //aktualne przesuniêcie wiersza wzglêdem xstart

		for (int x = 0; x < width; x++) // przepisanie wiersza
		{
			Img_new->SetRGB(xstart + x + cur_dif_width, y, data[(y * width + x) * 3 + 0], data[(y * width + x) * 3 + 1], data[(y * width + x) * 3 + 2]);
		}
	}
	delete Img_Cpy;
	Img_Cpy = CutXborder(Img_new); //trzeba usun¹æ puste pole przed i za obrazkiem które mog³o powstaæ przy kilkukrotnym wywo³ywaniu
	Draw();
}

void GUIMyFrame1::ShearY(double angle)
{
	if (std::fmod(angle + 90, 180) == 0) return; //je¿eli k¹t prosty to pochylenie nie ma sensu bo obraz z³o¿y³by siê do lini prostej

	wxClientDC dc(ImgScrolledWindow);
	//dc.SetBackground(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
	dc.Clear();
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();


	double tangens = tan(angle * M_PI / 180.0);
	int dif_height = (double)width * tangens;//wartoœæ o ile zmieni siê wysokoœæ obrazka
	wxImage* Img_new = new wxImage(width, height + abs(dif_height));

	unsigned char* data = Img_Cpy->GetData();

	double ystart = abs((double)dif_height) / 2.0; // punkt referencyjny wzglêdem którego rysujê (przesuniêcie centralnej kolumny)

	for (int x = 0; x < width; x++)
	{
		int cur_dif_height = ((double)width / 2.0 - x) * tangens; //aktualne przesuniêcie kolumny wzglêdem ystart
		for (int y = 0; y < height; y++)// przepisanie kolumny
		{
			Img_new->SetRGB(x, ystart + y + cur_dif_height, data[(y * width + x) * 3 + 0], data[(y * width + x) * 3 + 1], data[(y * width + x) * 3 + 2]);
		}
	}
	delete Img_Cpy;
	Img_Cpy = CutYborder(Img_new);//trzeba usun¹æ puste pole nad i pod obrazkiem które mog³o powstaæ przy kilkukrotnym wywo³ywaniu
	Draw();
}

wxImage* GUIMyFrame1::CutXborder(wxImage* Img)
{
	int width = Img->GetSize().GetWidth();
	int height = Img->GetSize().GetHeight();

	unsigned char* data = Img->GetData();
	int cutpos = 0; //pierwsza niepusta kolumna

	for (int x = 0; x < width; x++) //znalezienie cutpos
	{
		bool cut = true;
		for (int y = 0; y < height; y++)
		{
			if (data[(y * width + x) * 3 + 0] != 0)
			{
				cut = false;
				break;
			}
			if (data[(y * width + x) * 3 + 1] != 0)
			{
				cut = false;
				break;
			}
			if (data[(y * width + x) * 3 + 2] != 0)
			{
				cut = false;
				break;
			}
		}
		if (cut) cutpos++;
		else break;
	}
	if (cutpos == 0) //jeœli spe³niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(width - 2 * cutpos, height);

	for (int y = 0; y < height; y++)//przepisanie punktów pomij¹j¹c "ramkê".
	{
		for (int x = 0; x < width - 2 * cutpos; x++)
			Img_cut->SetRGB(x, y, data[(y * width + x + cutpos) * 3 + 0], data[(y * width + x + cutpos) * 3 + 1], data[(y * width + x + cutpos) * 3 + 2]);
	}
	delete Img;
	return Img_cut;
}

wxImage* GUIMyFrame1::CutYborder(wxImage* Img)
{
	int width = Img->GetSize().GetWidth();
	int height = Img->GetSize().GetHeight();

	unsigned char* data = Img->GetData();
	int cutpos = 0; //pierwszy niepusty wiersz

	for (int y = 0; y < height; y++) //znalezienie cutpos
	{
		bool cut = true;
		for (int x = 0; x < width; x++)
		{
			if (data[(y * width + x) * 3 + 0] != 0)
			{
				cut = false;
				break;
			}
			if (data[(y * width + x) * 3 + 1] != 0)
			{
				cut = false;
				break;
			}
			if (data[(y * width + x) * 3 + 2] != 0)
			{
				cut = false;
				break;
			}
		}
		if (cut) cutpos++;
		else break;
	}
	if (cutpos == 0) //jeœli spe³niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(width, height - 2 * cutpos);
	for (int y = 0; y < height - 2 * cutpos; y++) //przepisanie punktów pomij¹j¹c "ramkê".
	{
		for (int x = 0; x < width; x++)
			Img_cut->SetRGB(x, y, data[((y + cutpos) * width + x) * 3 + 0], data[((y + cutpos) * width + x) * 3 + 1], data[((y + cutpos) * width + x) * 3 + 2]);
	}
	return Img_cut;
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