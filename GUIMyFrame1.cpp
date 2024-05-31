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

	wxString wildcard =
		"Wszystkie pliki zdj�� (*.bmp;*.gif;*.jpeg;*.jpg;*.png;*.tiff;*.ppm)|"
		"*.bmp;*.gif;*.jpeg;*.jpg;*.png;*.tiff;*.ppm|"
		"Pliki BMP (*.bmp)|*.bmp|"
		"Pliki GIF (*.gif)|*.gif|"
		"Pliki JPEG (*.jpeg;*.jpg)|*.jpeg;*.jpg|"
		"Pliki PNG (*.png)|*.png|"
		"Pliki TIFF (*.tiff)|*.tiff|"
		"Pliki PPM (*.ppm)|*.ppm|"
		"Wszystkie pliki (*.*)|*.*";

	wxFileDialog* file_dialog = new wxFileDialog(this, "Load file", "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (file_dialog->ShowModal() == wxID_CANCEL)
	{
		return;
	}

	Img_Org.LoadFile(file_dialog->GetPath());
	Img_Cpy = new wxImage(Img_Org.Copy());

	wxClientDC dc(ImgScrolledWindow);
	dc.Clear();
	Draw();
	width = Img_Cpy->GetSize().GetWidth();
	height = Img_Cpy->GetSize().GetHeight();
	size = width * height;

	unsigned char* old = Img_Cpy->GetData(); // stare dane z obrazka

	delete[] vectors;
	vectors = new Vector4[size];

	for (int i = 0; i < size; ++i)
	{
		vectors[i].SetColor(old[3 * i], old[3 * i + 1], old[3 * i + 2]);
		vectors[i].data[0] = i % width - width / 2;
		vectors[i].data[1] = (i / width) - height / 2;
		vectors[i].data[2] = 0;
	}


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

	delete dialog;
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
	Img_Org = Img_Cpy->Copy();
	wxDialog* dialog = new wxDialog(this, wxID_ANY, "Options", wxDefaultPosition, wxSize(300, 240), wxDEFAULT_DIALOG_STYLE);

	wxPanel* panel = new wxPanel(dialog, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC);


	slider1 = new wxSlider(panel, wxID_ANY, 0, -180, 180, wxDefaultPosition, wxDefaultSize);
	slider2 = new wxSlider(panel, wxID_ANY, 0, -180, 180, wxDefaultPosition, wxDefaultSize);

	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle(YOZ):"), 0, wxALL, 5);
	sizer->Add(slider1, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle(XOZ):"), 0, wxALL, 5);
	sizer->Add(slider2, 0, wxALL | wxEXPAND, 5);


	sizer->Add(new wxButton(panel, wxID_OK), 0, wxALIGN_CENTER, 5);

	panel->SetSizer(sizer);
	panel->Fit();

	dialog->Bind(wxEVT_SCROLL_THUMBTRACK, &GUIMyFrame1::_Rotation, this);

	dialog->ShowModal();
	dialog->Destroy();

	delete dialog;
	
}

void GUIMyFrame1::_Rotation(wxCommandEvent& e)
{
	RotateOtherAxis(slider1->GetValue(), slider2->GetValue());
}

wxImage GUIMyFrame1::BilinearInterpolate(const wxImage& src, int newWidth, int newHeight) 
{
	int oldWidth = src.GetWidth();
	int oldHeight = src.GetHeight();
	wxImage dst(newWidth, newHeight);

	unsigned char* srcData = src.GetData();
	unsigned char* dstData = dst.GetData();

	double xRatio = static_cast<double>(oldWidth - 1) / newWidth;
	double yRatio = static_cast<double>(oldHeight - 1) / newHeight;

	for (int y = 0; y < newHeight; ++y) {
		for (int x = 0; x < newWidth; ++x) {
			double gx = x * xRatio;
			double gy = y * yRatio;

			int x1 = static_cast<int>(gx);
			int y1 = static_cast<int>(gy);
			int x2 =  std::max(0, std::min(oldWidth - 1, x1+1));
			int y2 =  std::max(0, std::min(oldHeight - 1, y1+1));

			double dx = gx - x1;
			double dy = gy - y1;

			for (int c = 0; c < 3; ++c) {
				int offset1 = (y1 * oldWidth + x1) * 3 + c;
				int offset2 = (y1 * oldWidth + x2) * 3 + c;
				int offset3 = (y2 * oldWidth + x1) * 3 + c;
				int offset4 = (y2 * oldWidth + x2) * 3 + c;

				double color = (1 - dx) * (1 - dy) * srcData[offset1] +
					dx * (1 - dy) * srcData[offset2] +
					(1 - dx) * dy * srcData[offset3] +
					dx * dy * srcData[offset4];
				dstData[(y * newWidth + x) * 3 + c] = static_cast<unsigned char>(color);
			}
		}
	}

	return dst;
}



void GUIMyFrame1::RotateOtherAxis(double angle1, double angle2)
{
	width = Img_Org.GetSize().GetWidth();
	height = Img_Org.GetSize().GetHeight();
	size = width * height;

	unsigned char* old = Img_Org.GetData(); // stare dane z obrazka

	vectors = new Vector4[size];

	for (int i = 0; i < size; ++i)
	{
		vectors[i].SetColor(old[3 * i], old[3 * i + 1], old[3 * i + 2]);
		vectors[i].data[0] = i % width - width / 2;
		vectors[i].data[1] = (i / width) - height / 2;
		vectors[i].data[2] = 0;
	}

	wxClientDC dc(ImgScrolledWindow);
	dc.Clear();

	double angleX = angle1 * M_PI / 180; 
	double angleY = angle2 * M_PI / 180; 

	Matrix4 Xaxis;

	Xaxis.data[0][0] = 1;
	Xaxis.data[1][1] = cos(angleX);
	Xaxis.data[1][2] = -sin(angleX);
	Xaxis.data[2][1] = sin(angleX);
	Xaxis.data[2][2] = cos(angleX);
	Xaxis.data[3][3] = 1;

	Matrix4 Yaxis;

	Yaxis.data[0][0] = cos(angleY);
	Yaxis.data[0][2] = sin(angleY);
	Yaxis.data[1][1] = 1;
	Yaxis.data[2][0] = -sin(angleY);
	Yaxis.data[2][2] = cos(angleY);
	Yaxis.data[3][3] = 1;


	for (int i = 0; i < size; ++i)
	{
		vectors[i] = Yaxis * Xaxis * vectors[i];
		vectors[i].data[0] += width / 2;
		vectors[i].data[1] += height / 2;
	}

	delete Img_Cpy;
	Img_Cpy = new wxImage(width, height);


	for (int i = 0; i < size - 1; ++i)
	{
		Img_Cpy->SetRGB(vectors[i].GetX(), vectors[i].GetY(), vectors[i].GetRed(), vectors[i].GetGreen(), vectors[i].GetBlue());
	}

	*Img_Cpy = BilinearInterpolate(*Img_Cpy, width, height).Copy();

	Draw();

	for (int i = 0; i < size; ++i)
	{
		vectors[i].data[0] -= width / 2;
		vectors[i].data[1] -= height / 2;
	}

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
	if (std::fmod(angle + 90, 180) == 0) return; //je�eli k�t prosty to pochylenie nie ma sensu bo obraz z�o�y�by si� do lini prostej

	wxClientDC dc(ImgScrolledWindow);
	//dc.SetBackground(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
	dc.Clear();
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();


	double tangens = tan(angle * M_PI / 180.0);
	int dif_width = (double)height * tangens; //warto�� o ile zmieni si� szeroko�� obrazka
	wxImage* Img_new = new wxImage(width + abs(dif_width), height);

	unsigned char* data = Img_Cpy->GetData();

	double xstart = abs((double)dif_width) / 2.0; // punkt referencyjny wzgl�dem kt�rego rysuj� (przesuni�cie centralnego wiersza)

	for (int y = 0; y < height; y++)
	{
		int cur_dif_width = (double)((double)height / 2.0 - y) * tangens; //aktualne przesuni�cie wiersza wzgl�dem xstart

		for (int x = 0; x < width; x++) // przepisanie wiersza
		{
			Img_new->SetRGB(xstart + x + cur_dif_width, y, data[(y * width + x) * 3 + 0], data[(y * width + x) * 3 + 1], data[(y * width + x) * 3 + 2]);
		}
	}
	delete Img_Cpy;
	Img_Cpy = CutXborder(Img_new); //trzeba usun�� puste pole przed i za obrazkiem kt�re mog�o powsta� przy kilkukrotnym wywo�ywaniu
	Draw();
}

void GUIMyFrame1::ShearY(double angle)
{
	if (std::fmod(angle + 90, 180) == 0) return; //je�eli k�t prosty to pochylenie nie ma sensu bo obraz z�o�y�by si� do lini prostej

	wxClientDC dc(ImgScrolledWindow);
	//dc.SetBackground(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
	dc.Clear();
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();


	double tangens = tan(angle * M_PI / 180.0);
	int dif_height = (double)width * tangens;//warto�� o ile zmieni si� wysoko�� obrazka
	wxImage* Img_new = new wxImage(width, height + abs(dif_height));

	unsigned char* data = Img_Cpy->GetData();

	double ystart = abs((double)dif_height) / 2.0; // punkt referencyjny wzgl�dem kt�rego rysuj� (przesuni�cie centralnej kolumny)

	for (int x = 0; x < width; x++)
	{
		int cur_dif_height = ((double)width / 2.0 - x) * tangens; //aktualne przesuni�cie kolumny wzgl�dem ystart
		for (int y = 0; y < height; y++)// przepisanie kolumny
		{
			Img_new->SetRGB(x, ystart + y + cur_dif_height, data[(y * width + x) * 3 + 0], data[(y * width + x) * 3 + 1], data[(y * width + x) * 3 + 2]);
		}
	}
	delete Img_Cpy;
	Img_Cpy = CutYborder(Img_new);//trzeba usun�� puste pole nad i pod obrazkiem kt�re mog�o powsta� przy kilkukrotnym wywo�ywaniu
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
	if (cutpos == 0) //je�li spe�niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(width - 2 * cutpos, height);

	for (int y = 0; y < height; y++)//przepisanie punkt�w pomij�j�c "ramk�".
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
	if (cutpos == 0) //je�li spe�niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(width, height - 2 * cutpos);
	for (int y = 0; y < height - 2 * cutpos; y++) //przepisanie punkt�w pomij�j�c "ramk�".
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