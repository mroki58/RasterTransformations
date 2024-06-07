#include "GUIMyFrame1.h"
#include "vecmat.h"

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


	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle of rotation in X-axis:"), 0, wxALL, 5);
	sizer->Add(slider1, 0, wxALL | wxEXPAND, 5);
	sizer->Add(new wxStaticText(panel, wxID_ANY, "Enter Angle of rotation in Y-xis:"), 0, wxALL, 5);
	sizer->Add(slider2, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, "Modify center point of rotation X-axis:"), 0, wxALL, 5);
	sizer->Add(slider4, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, "Modify center point of rotation Y-axis:"), 0, wxALL, 5);
	sizer->Add(slider3, 0, wxALL | wxEXPAND, 5);

	sizer->Add(new wxStaticText(panel, wxID_ANY, "Modify camera position:"), 0, wxALL, 5);
	sizer->Add(slider5, 0, wxALL | wxEXPAND, 5);


	sizer->Add(new wxButton(panel, wxID_OK), 0, wxALIGN_CENTER, 5);

	panel->SetSizer(sizer);
	panel->Fit();

	width = Img_Org.GetSize().GetWidth();
	height = Img_Org.GetSize().GetHeight();
	size = width * height;

	vectors = new Vector4[size];

	dialog->Bind(wxEVT_SCROLL_THUMBTRACK, &GUIMyFrame1::_Rotation, this);

	dialog->ShowModal();
	dialog->Destroy();

	delete dialog;
	delete[] vectors;

	Img_Cpy = CutXborder(Img_Cpy);
	Img_Cpy = CutYborder(Img_Cpy);


	Draw();

}

void GUIMyFrame1::_Rotation(wxCommandEvent& e)
{
	RotateOtherAxis(slider1->GetValue(), slider2->GetValue(), slider3->GetValue(), slider4->GetValue(), slider5->GetValue());
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