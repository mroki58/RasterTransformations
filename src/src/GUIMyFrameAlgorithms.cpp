#include "GUIMyFrame1.h"
#include "vecmat.h"

void GUIMyFrame1::RotateImagePlane(double angle, int cx, int cy)
{
	wxImage image = (*Img_Cpy).Copy();

	int width = image.GetWidth();
	int height = image.GetHeight();

	double rad = angle * M_PI / 180.0;

	double corners[4][2] = {
		{0, 0},
		{width, 0},
		{0, height},
		{width, height}
	};

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

	ImagesHistory.push_back(Img_Cpy);
	Img_Cpy = new wxImage(rotatedImage);
	Img_Cpy = CutXborder(Img_Cpy);
	Img_Cpy = CutYborder(Img_Cpy);
	Draw();
}


void GUIMyFrame1::RotateOtherAxis(int angle1, int angle2, int zmiana1, int zmiana2, int camera_pos)
{
	camera_pos = camera_pos / 100.;
	double d_zmiana1 = zmiana1 / 100.0; 
	double d_zmiana2 = zmiana2 / 100.0;

	unsigned char* old = Img_Org.GetData(); // stare dane z obrazka

	for (int i = 0; i < size; ++i)
	{
		vectors[i].SetColor(old[3 * i], old[3 * i + 1], old[3 * i + 2]);
		vectors[i].data[0] = (i % width - width / 2) / (double)width * 2;
		vectors[i].data[1] = ((i / width) - height / 2) / (double)height * 2;
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
	m7.data[0][0] = width / 2.;
	m7.data[1][1] = height / 2.;

	Matrix4 m8;
	m8.data[0][0] = 1;
	m8.data[0][3] = -d_zmiana1;
	m8.data[1][1] = 1;
	m8.data[1][3] = -d_zmiana2;
	m8.data[2][2] = 1;
	m8.data[2][3] = 0;
	m8.data[3][3] = 1;
    
	
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
	if(Img_Cpy != ImagesHistory.back())
		delete Img_Cpy;

	Img_Cpy = new wxImage(width + width / 2., height + height / 2.);


	for (int i = 0; i < width - 1; ++i)
	{
		for (int j = 0; j < height - 1; ++j)
		{
			for (int x = (int)(vectors[j * width + i].data[0]); x < vectors[j * width + i + 1].data[0]; ++x)
			{
				for (int y = (int)(vectors[j * width + i].data[1]); y < vectors[(j + 1) * width + i].data[1]; ++y)
				{
					Img_Cpy->SetRGB(width / 4. + x, height / 4. + y, vectors[j * width + i].GetRed(), vectors[j * width + i].GetGreen(), vectors[j * width + i].GetBlue());
				}
			}
		}
	}

	Draw();

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
	ImagesHistory.push_back(Img_Cpy);
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

	int cutpos_left = FindCutLeft(Img); //pierwsza niepusta kolumna, default=0
	int cutpos_right = FindCutRight(Img); //ostatnia niepusta kolumna licz¹c od prawej, default=width-1

	if (cutpos_left == 0 && cutpos_right == width) //jeœli spe³niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(cutpos_right - cutpos_left + 1, height);

	for (int y = 0; y < height; y++)//przepisanie punktów pomij¹j¹c "ramkê".
	{
		for (int x = 0; x <= cutpos_right - cutpos_left; x++)
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

	int cutpos = width - 1; //pierwsza niepusta kolumna od prawej

	for (int x = width - 1; x > 0; x--) //znalezienie cutpos
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
	int cutpos_top = FindCutTop(Img); //pierwszy niepusty wiersz, default=0
	int cutpos_bottom = FindCutBottom(Img); //ostatni niepusty wiersz licz¹c od do³u, default=height-1


	if (cutpos_top == 0 && cutpos_bottom == height) //jeœli spe³niony -> brak "ramki"
		return Img;

	wxImage* Img_cut = new wxImage(width, cutpos_bottom - cutpos_top + 1);
	for (int y = 0; y <= cutpos_bottom - cutpos_top; y++) //przepisanie punktów pomij¹j¹c "ramkê".
	{
		for (int x = 0; x < width; x++)
			Img_cut->SetRGB(x, y, data[((y + cutpos_top) * width + x) * 3 + 0], data[((y + cutpos_top) * width + x) * 3 + 1], data[((y + cutpos_top) * width + x) * 3 + 2]);
	}
	delete Img;
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
	int cutpos = height - 1; //pirwszy niepusty wiersz od do³u

	for (int y = height - 1; y > 0; y--) //znalezienie cutpos
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

	ImagesHistory.push_back(Img_Cpy);
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
	Img_Cpy = CutXborder(Img_Cpy);
	Img_Cpy = CutYborder(Img_Cpy);
	Draw();
}

void GUIMyFrame1::Mirror(int flag)
{
	int width = Img_Cpy->GetSize().GetWidth();
	int height = Img_Cpy->GetSize().GetHeight();

	unsigned char* old = Img_Cpy->GetData(); 

	Vector4* vectors = new Vector4[width * height];
	Vector4* temp_vect = new Vector4;

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			vectors[j * width + i].SetColor(old[3 * (j * width + i)], old[3 * (j * width + i) + 1], old[3 * (j * width + i) + 2]);
			vectors[j * width + i].data[0] = i;
			vectors[j * width + i].data[1] = j;
			vectors[j * width + i].data[2] = 0.0;
		}
	}

	if (flag)
	{
		int n = width / 2;
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				temp_vect->SetColor(vectors[j * width + i].GetRed(), vectors[j * width + i].GetGreen(), vectors[j * width + i].GetBlue());
				vectors[j * width + i].SetColor(vectors[j * width + width - 1 - i].GetRed(), vectors[j * width + width - 1 - i].GetGreen(), vectors[j * width + width - 1 - i].GetBlue());
				vectors[j * width + width - 1 - i].SetColor(temp_vect->GetRed(), temp_vect->GetGreen(), temp_vect->GetBlue());
			}
		}
	}
	else
	{
		int n = height / 2;
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				temp_vect->SetColor(vectors[j * width + i].GetRed(), vectors[j * width + i].GetGreen(), vectors[j * width + i].GetBlue());
				vectors[j * width + i].SetColor(vectors[(height - 1 - j) * width + i].GetRed(), vectors[(height - 1 - j) * width + i].GetGreen(), vectors[(height - 1 - j) * width + i].GetBlue());
				vectors[(height - 1 - j) * width + i].SetColor(temp_vect->GetRed(), temp_vect->GetGreen(), temp_vect->GetBlue());
			}
		}
	}
	ImagesHistory.push_back(Img_Cpy);
	Img_Cpy = new wxImage(width, height);

	// rysowanie
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			Img_Cpy->SetRGB(vectors[j * width + i].GetX(), vectors[j * width + i].GetY(), vectors[j * width + i].GetRed(), vectors[j * width + i].GetGreen(), vectors[j * width + i].GetBlue());
		}
	}
	delete[] vectors;
	delete temp_vect;
	Draw();
}