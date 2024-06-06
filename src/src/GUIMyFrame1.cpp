#include "GUIMyFrame1.h"
#include "vecmat.h"
#include <fstream>

GUIMyFrame1::GUIMyFrame1(wxWindow* parent)
	:
	MyFrame1(parent)
{
	ImgScrolledWindow->SetScrollbars(25, 25, 52, 40);
	Img_Cpy = new wxImage();
}

void GUIMyFrame1::LoadButtonOnButtonClick(wxCommandEvent& event)
{
	wxInitAllImageHandlers();

	wxString wildcard =
		"Wszystkie pliki zdjêæ (*.bmp;*.gif;*.jpeg;*.jpg;*.png;*.tiff;*.ppm)|"
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


	Draw();
}

void GUIMyFrame1::RotateButton1OnButtonClick(wxCommandEvent& event)
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

void GUIMyFrame1::RotateImagePlane(double angle, int cx, int cy)
{
	wxImage image = (*Img_Cpy).Copy();

	int width = image.GetWidth();
	int height = image.GetHeight();

	double rad = angle * M_PI / 180.0;

	// Corners of the image
	double corners[4][2] = {
		{0, 0},
		{width, 0},
		{0, height},
		{width, height}
	};

	// New corners after rotation
	double newCorners[4][2];
	for (int i = 0; i < 4; ++i)
	{
		double tx = corners[i][0] - cx;
		double ty = corners[i][1] - cy;

		newCorners[i][0] = cos(rad) * tx - sin(rad) * ty + cx;
		newCorners[i][1] = sin(rad) * tx + cos(rad) * ty + cy;
	}

	double minX = std::min({ newCorners[0][0], newCorners[1][0], newCorners[2][0], newCorners[3][0] });
	double maxX = std::max({ newCorners[0][0], newCorners[1][0], newCorners[2][0], newCorners[3][0] });
	double minY = std::min({ newCorners[0][1], newCorners[1][1], newCorners[2][1], newCorners[3][1] });
	double maxY = std::max({ newCorners[0][1], newCorners[1][1], newCorners[2][1], newCorners[3][1] });

	int newWidth = static_cast<int>(maxX - minX);
	int newHeight = static_cast<int>(maxY - minY);

	wxImage rotatedImage(newWidth, newHeight, true);

	double offsetX = minX;
	double offsetY = minY;

	for (int x = 0; x < newWidth; ++x)
	{
		for (int y = 0; y < newHeight; ++y)
		{
			double tx = x + offsetX - cx;
			double ty = y + offsetY - cy;

			double rx = cos(rad) * tx + sin(rad) * ty + cx;
			double ry = -sin(rad) * tx + cos(rad) * ty + cy;

			int srcX = static_cast<int>(rx);
			int srcY = static_cast<int>(ry);

			if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height)
			{
				rotatedImage.SetRGB(x, y, image.GetRed(srcX, srcY), image.GetGreen(srcX, srcY), image.GetBlue(srcX, srcY));
			}
			else
			{
				continue;
			}
		}
	}

	delete Img_Cpy;
	Img_Cpy = new wxImage(rotatedImage);
	Img_Cpy = CutXborder(Img_Cpy);
	Img_Cpy = CutYborder(Img_Cpy);
	Draw();
}

void GUIMyFrame1::RotateButton2OnButtonClick(wxCommandEvent& event)
{
	Img_Org = Img_Cpy->Copy();
	wxDialog* dialog = new wxDialog(this, wxID_ANY, "Options", wxDefaultPosition, wxSize(300, 460), wxDEFAULT_DIALOG_STYLE);

	wxPanel* panel = new wxPanel(dialog, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);


	slider1 = new wxSlider(panel, wxID_ANY, 0, -90, 90, wxDefaultPosition, wxDefaultSize);
	slider2 = new wxSlider(panel, wxID_ANY, 0, -90, 90, wxDefaultPosition, wxDefaultSize);
	slider3 = new wxSlider(panel, wxID_ANY, 0, -100, 100, wxDefaultPosition, wxDefaultSize);
	slider4 = new wxSlider(panel, wxID_ANY, 0, -100, 100, wxDefaultPosition, wxDefaultSize);
	slider5 = new wxSlider(panel, wxID_ANY, -600, -800, -400, wxDefaultPosition, wxDefaultSize);


	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle(YOZ):"), 0, wxALL, 5);
	sizer->Add(slider1, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle(XOZ):"), 0, wxALL, 5);
	sizer->Add(slider2, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, "Modify rotation axis (YOZ):"), 0, wxALL, 5);
	sizer->Add(slider3, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Modify rotation axis(XOZ):"), 0, wxALL, 5);
	sizer->Add(slider4, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Modify camera position:"), 0, wxALL, 5);
	sizer->Add(slider5, 0, wxALL | wxEXPAND, 5);


	sizer->Add(new wxButton(panel, wxID_OK), 0, wxALIGN_CENTER, 5);

	panel->SetSizer(sizer);
	panel->Fit();

	width = Img_Org.GetSize().GetWidth();
	height = Img_Org.GetSize().GetHeight();
	size = width * height;


	unsigned char* old = Img_Org.GetData(); // stare dane z obrazka
	vectors = new Vector4[size];

	dialog->Bind(wxEVT_SCROLL_THUMBTRACK, &GUIMyFrame1::_Rotation, this);

	dialog->ShowModal();
	dialog->Destroy();

	delete dialog;
	delete[] vectors;

}

void GUIMyFrame1::_Rotation(wxCommandEvent& e)
{
	RotateOtherAxis(slider1->GetValue(), slider2->GetValue(), slider3->GetValue(), slider4->GetValue(), slider5->GetValue());
}


void GUIMyFrame1::RotateOtherAxis(int angle1, int angle2, int zmiana1, int zmiana2, int camera_pos)
{
	camera_pos = camera_pos / 100.;
	double d_zmiana1 = zmiana1 / 100.0;
	double d_zmiana2 = zmiana2 / 100.0;
	//d_zmiana1 *= (height/2);
	//d_zmiana2 *= (width/2);

	unsigned char* old = Img_Org.GetData(); // stare dane z obrazka
	
	for (int i = 0; i < size; ++i)
	{
		vectors[i].SetColor(old[3 * i], old[3 * i + 1], old[3 * i + 2]);
		vectors[i].data[0] = (i % width - width / 2) / (double)width*2;
		vectors[i].data[1] = ((i / width) - height / 2) / (double)height*2;
		vectors[i].data[2] = 0;
		vectors[i].data[3] = 1;
	}

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

	Matrix4 m5;
	m5.data[0][0] = 1;
	m5.data[0][3] = d_zmiana1;
	m5.data[1][1] = 1;
	m5.data[1][3] = d_zmiana2;
	m5.data[2][2] = 1;
	m5.data[2][3] = 0;
	m5.data[3][3] = 1;

	Matrix4 m6;
	m6.data[0][0] = fabs(camera_pos);
	m6.data[1][1] = fabs(camera_pos);
	m6.data[3][2] = 1.0;
	m6.data[3][3] = fabs(camera_pos);

	Matrix4 m7;
	m7.data[0][0] = width/2.;
	m7.data[1][1] = height/2.;

	Matrix4 m8;
	m8.data[0][0] = 1;
	m8.data[0][3] = -d_zmiana1;
	m8.data[1][1] = 1;
	m8.data[1][3] = -d_zmiana2;
	m8.data[2][2] = 1;
	m8.data[2][3] = 0;
	m8.data[3][3] = 1;


	if (old_angle1 != angle1 || old_angle2 != angle2 || old_camera != camera_pos)
	{
		for (int i = 0; i < size; ++i)
		{
			vectors[i].data[3] = 1;
			vectors[i] = m5 * Yaxis * Xaxis * m8 * vectors[i];
			vectors[i] = m7 * m6 * vectors[i];

			vectors[i].data[0] /= vectors[i].data[3];
			vectors[i].data[1] /= vectors[i].data[3];
			vectors[i].data[0] += width / 2;
			vectors[i].data[1] += height / 2;
		}

		delete Img_Cpy;
		Img_Cpy = new wxImage(width, height);


		for (int i = 0; i < width - 1; ++i)
		{
			for (int j = 0; j < height - 1; ++j)
			{
				for (int x = (int)(vectors[j * width + i].data[0]); x < vectors[j * width + i + 1].data[0]; ++x)
				{
					for (int y = (int)(vectors[j * width + i].data[1]); y < vectors[(j + 1) * width + i].data[1]; ++y)
					{
						Img_Cpy->SetRGB(x, y, vectors[j * width + i].GetRed(), vectors[j * width + i].GetGreen(), vectors[j * width + i].GetBlue());
					}
				}
			}
		}
	}
	old_angle1 = angle1;
	old_angle2 = angle2;
	old_camera = camera_pos;
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

	int cutpos_left = FindCutLeft(Img); //pierwsza niepusta kolumna
	int cutpos_right = FindCutRight(Img); //ostatnia niepusta kolumna

	if (cutpos_left == 0 && cutpos_right == width) //jeœli spe³niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(cutpos_right - cutpos_left, height);

	for (int y = 0; y < height; y++)//przepisanie punktów pomij¹j¹c "ramkê".
	{
		for (int x = 0; x < cutpos_right - cutpos_left; x++)
			Img_cut->SetRGB(x, y, data[(y * width + x + cutpos_left) * 3 + 0], data[(y * width + x + cutpos_left) * 3 + 1], data[(y * width + x + cutpos_left) * 3 + 2]);
	}
	delete Img;
	return Img_cut;
}

int GUIMyFrame1::FindCutLeft(wxImage* Img)
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
	return cutpos;
}

int GUIMyFrame1::FindCutRight(wxImage* Img)
{
	int width = Img->GetSize().GetWidth();
	int height = Img->GetSize().GetHeight();

	unsigned char* data = Img->GetData();

	int cutpos = width; //pierwsza niepusta kolumna

	for (int x = width; x > 0; x--) //znalezienie cutpos
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
		if (cut) cutpos--;
		else break;
	}
	return cutpos;
}

wxImage* GUIMyFrame1::CutYborder(wxImage* Img)
{
	int width = Img->GetSize().GetWidth();
	int height = Img->GetSize().GetHeight();

	unsigned char* data = Img->GetData();
	int cutpos_top = FindCutTop(Img); //pierwszy niepusty wiersz
	int cutpos_bottom = FindCutBottom(Img); //ostatni niepusty wiersz


	if (cutpos_top == 0 && cutpos_bottom == height) //jeœli spe³niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(width, cutpos_bottom - cutpos_top);
	for (int y = 0; y < cutpos_bottom - cutpos_top; y++) //przepisanie punktów pomij¹j¹c "ramkê".
	{
		for (int x = 0; x < width; x++)
			Img_cut->SetRGB(x, y, data[((y + cutpos_top) * width + x) * 3 + 0], data[((y + cutpos_top) * width + x) * 3 + 1], data[((y + cutpos_top) * width + x) * 3 + 2]);
	}
	return Img_cut;
}

int GUIMyFrame1::FindCutTop(wxImage* Img)
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
	return cutpos;
}

int GUIMyFrame1::FindCutBottom(wxImage* Img)
{
	int width = Img->GetSize().GetWidth();
	int height = Img->GetSize().GetHeight();

	unsigned char* data = Img->GetData();
	int cutpos = height; //pierwszy niepusty wiersz

	for (int y = height; y > 0; y--) //znalezienie cutpos
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
		if (cut) cutpos--;
		else break;
	}
	return cutpos;
}


void GUIMyFrame1::CorrectDisortion(double A, double B, double C, double D, int flag)
{
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();

	unsigned char* old = Img_Cpy->GetData(); // stare dane z obrazka

	Vector4* vectors = new Vector4[width * height];
	Vector4* vectors_copy = new Vector4[width * height]; // potrzebna kopia wartosci starych



	// wczytanie wartoœci kolorów dla wektorów
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			vectors[j * width + i].SetColor(0, 0, 0);
			vectors[j * width + i].data[0] = i;
			vectors[j * width + i].data[1] = j;
			vectors[j * width + i].data[2] = 0.0;

			vectors_copy[j * width + i].SetColor(old[3 * (j * width + i)], old[3 * (j * width + i) + 1], old[3 * (j * width + i) + 2]);
			vectors_copy[j * width + i].data[0] = i;
			vectors_copy[j * width + i].data[1] = j;
			vectors_copy[j * width + i].data[2] = 0.0;
		}
	}
	// skala
	int d = width > height ? height : width;
	d = d / 2;

	double paramA = A; // to powinno tylko wplywac na najdalsze pixele
	double paramB = B; // takie bardzije uniwerslane i chyba to jest ten parametr alpha z wymagan podstawowych
	double paramC = C; // kolejne najbardziej uniwersalne wierzac zrodlu
	double paramD = 1.0 - paramA - paramB - paramC;// skalowanie liniowe albo w sumie w tym wypadku jego brak
	if (flag == 1) paramD = D;

	double centerX = (double)(width - 1) / 2.0;
	double centerY = (double)(height - 1) / 2.0;

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			//transoframcje
			double deltaX = ((double)i - centerX) / (double)d;
			double deltaY = (centerY - (double)j) / (double)d;
			double dstR = sqrt(deltaX * deltaX + deltaY * deltaY);
			double srcR = (paramA * dstR * dstR * dstR + paramB * dstR * dstR + paramC * dstR + paramD) * dstR;
			double factor = abs(srcR / dstR);
			double srcXd = centerX + (deltaX * factor * d);
			double srcYd = centerY - (deltaY * factor * d);

			//bez interpolacji rzutowanie na najblizszy pixel
			int srcX = (int)srcXd;
			int srcY = (int)srcYd;

			//przypisanie
			if (srcX >= 0 && srcY >= 0 && srcX < width && srcY < height)
				vectors[j * width + i].SetColor(vectors_copy[srcY * width + srcX].GetRed(), vectors_copy[srcY * width + srcX].GetGreen(), vectors_copy[srcY * width + srcX].GetBlue());

		}
	}

	delete Img_Cpy;
	Img_Cpy = new wxImage(width, height);

	// rysowanie
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			Img_Cpy->SetRGB(vectors[j * width + i].GetX(), vectors[j * width + i].GetY(), vectors[j * width + i].GetRed(), vectors[j * width + i].GetGreen(), vectors[j * width + i].GetBlue());
		}
	}


	//wywalenie wektorow
	delete[] vectors;
	delete[] vectors_copy;
	Draw();
}
//metoda do sprawdzania eventu jak sie w okienku dialogowym zazanczy czheckbox
void GUIMyFrame1::OnCheckBoxToggle(wxCommandEvent& event)
{
	if (checkBox_handler->IsChecked())
	{
		text_handler->Disable();
		(*flag_handler) = 0;
	}
	else
	{
		text_handler->Enable();
		(*flag_handler) = 1;
	}
}/*
 ogolnie moja metoda dziala tak, ze patrze sobie na pixel i na podstawie podanych parametrow obliczam
 jaki pixel z niezdeformowanego obrazka powinien tu byc
 jedyny szkopul jest taki ze jak mamy barrel disortion to ten algorytm ucina troche obrazek
 nie jestem pewien czy jest to blad czy nie
 fianlnie nie potrzebowalem interpolacji
 i jestem (niestety prawie tylko) pewien ze te parametry to beda rozszerzone wlasciwosci obiektywu patrzac na stronke na podstawie ktorej robilem ten algorytm
 */
void GUIMyFrame1::DistortionButtonOnButtonClick(wxCommandEvent& event)
{
	// TODO: Implement DistortionButtonOnButtonClick
	wxDialog* dialog = new wxDialog(this, wxID_ANY, "Correction parameters", wxDefaultPosition, wxSize(630, 490), wxDEFAULT_DIALOG_STYLE);
	int flag = 1;
	wxPanel* panel = new wxPanel(dialog, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sizerA = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizerB = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizerC = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizerD = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizerCheckBox = new wxBoxSizer(wxHORIZONTAL);

	wxTextValidator validator(wxFILTER_NUMERIC);

	wxTextCtrl* textCtrl1 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxTextCtrl* textCtrl2 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxTextCtrl* textCtrl3 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxTextCtrl* textCtrl4 = new wxTextCtrl(panel, wxID_ANY, "1.0", wxDefaultPosition, wxDefaultSize, 0, validator);
	wxCheckBox* checkbox = new wxCheckBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
	flag_handler = &flag;
	text_handler = textCtrl4;
	checkBox_handler = checkbox;
	checkbox->Bind(wxEVT_CHECKBOX, &GUIMyFrame1::OnCheckBoxToggle, this);

	checkbox->SetValue(1);
	text_handler->Disable();
	(*flag_handler) = 0;

	sizerA->Add(new wxStaticText(panel, wxID_ANY, "A (Affects only the outermost pixels of the image)"), 0, wxALL, 5);
	sizerA->Add(textCtrl1, 0, wxALL | wxEXPAND, 5);
	sizerB->Add(new wxStaticText(panel, wxID_ANY, "B (Most cases only require B optimization)"), 0, wxALL, 5);
	sizerB->Add(textCtrl2, 0, wxALL | wxEXPAND, 5);
	sizerC->Add(new wxStaticText(panel, wxID_ANY, "C (Most uniform correction)"), 0, wxALL, 5);
	sizerC->Add(textCtrl3, 0, wxALL | wxEXPAND, 5);
	sizerD->Add(new wxStaticText(panel, wxID_ANY, "D (Describes the linear saclling of the image)"), 0, wxALL, 5);
	sizerD->Add(textCtrl4, 0, wxALL | wxEXPAND, 5);
	sizerCheckBox->Add(checkbox, 0, wxALL | wxEXPAND, 5);
	sizerCheckBox->Add(new wxStaticText(panel, wxID_ANY, "Set D = 1 - A - B - C for no image scalling"), 0, wxALL, 5);


	sizer->Add(new wxStaticText(panel, wxID_ANY, "Use negative (A, B, C) up to  -1.0 to shift distant points away from center"), 0, wxALL, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Use positive (A, B, C) up to  1.0 to shift distant points towards the center"), 0, wxALL, 5);
	sizer->Add(sizerA, 0, wxALL | wxEXPAND, 5);
	sizer->Add(sizerB, 0, wxALL | wxEXPAND, 5);
	sizer->Add(sizerC, 0, wxALL | wxEXPAND, 5);
	sizer->Add(sizerD, 0, wxALL | wxEXPAND, 5);
	sizer->Add(sizerCheckBox, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, ""), 0, wxALL, 5);

	wxButton* okButton = new wxButton(panel, wxID_OK);
	
	sizer->Add(okButton, 0, wxALIGN_CENTER, 5);

	panel->SetSizer(sizer);
	panel->Fit();
	dialog->ShowModal();	
	dialog->Destroy();
	if (textCtrl1->GetValue() == "") textCtrl1->SetValue("0.0");
	if (textCtrl2->GetValue() == "") textCtrl2->SetValue("0.0");
	if (textCtrl3->GetValue() == "") textCtrl3->SetValue("0.0");
	if (textCtrl4->GetValue() == "") textCtrl4->SetValue("1.0");
	CorrectDisortion(atof(textCtrl1->GetValue()), atof(textCtrl2->GetValue()), atof(textCtrl3->GetValue()), atof(textCtrl4->GetValue()), flag);

}

void GUIMyFrame1::GridBoxToggle(wxCommandEvent& event)
{
	if (GridBox->IsChecked())
	{
		LoadButton->Disable();
		RotateButton1->Disable();
		RotateButton2->Disable();
		ShearButton->Disable();
		DistortionButton->Disable();
		SaveButton->Disable();
		ShowGrid();
	}
	else
	{
		LoadButton->Enable();
		RotateButton1->Enable();
		RotateButton2->Enable();
		ShearButton->Enable();
		DistortionButton->Enable();
		SaveButton->Enable();
		HideGrid();
	}
}

void GUIMyFrame1::ShowGrid()
{

}
void GUIMyFrame1::HideGrid()
{

}
void GUIMyFrame1::SaveButtonOnButtonClick(wxCommandEvent& event)
{
	wxString wildcard = "Pliki BMP (*.bmp)|*.bmp|"
		"Pliki GIF (*.gif)|*.gif|"
		"Pliki JPEG (*.jpeg;*.jpg)|*.jpeg;*.jpg|"
		"Pliki PNG (*.png)|*.png|"
		"Pliki TIFF (*.tiff)|*.tiff|"
		"Pliki PPM (*.ppm)|*.ppm";

	wxFileDialog saveFileDialog(this, _("Save Image file"), "", "", wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;

	wxString path = saveFileDialog.GetPath();
	wxBitmapType type;
	switch (saveFileDialog.GetFilterIndex())
	{
	case 0:type = wxBITMAP_TYPE_BMP; break;
	case 1:type = wxBITMAP_TYPE_GIF; break;
	case 2:type = wxBITMAP_TYPE_JPEG; break;
	case 3:type = wxBITMAP_TYPE_PNG; break;
	case 4:type = wxBITMAP_TYPE_TIFF; break;
	case 5:type = wxBITMAP_TYPE_PNM; break;
	}


	if (!Img_Cpy->IsOk())
	{
		return;
	}

	if (!Img_Cpy->SaveFile(path, type))
	{
		wxLogError("Cannot save image to file '%s'.", path);
	}
}

void GUIMyFrame1::ImgScrolledWindowOnUpdateUI(wxUpdateUIEvent& event)
{
	Draw();
}


void GUIMyFrame1::Draw()
{
	wxBitmap bitmap(*Img_Cpy);
	wxClientDC dc(ImgScrolledWindow);
	wxBufferedDC buff(&dc);
	ImgScrolledWindow->DoPrepareDC(buff);
	buff.Clear();
	buff.DrawBitmap(bitmap, 0, 0, false);
}