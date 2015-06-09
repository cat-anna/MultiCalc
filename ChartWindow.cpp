//$$---- Form CPP ----
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <mem.h>

#include "MainForm.h"
#include "ChartWindow.h"
#include "gl.h"
#include "ChartPropertis.h"
#include "FncProperity.h"
#include "Export.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "glWindow"
#pragma resource "*.dfm"
TFChartWindow *FChartWindow;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TTabData::TTabData(TCalcResult *Result)
{
	CalcData = new TCalcResult(FMainForm->ControllClass);
	CalcData->Assign(Result);
	LastDraw = new Graphics::TBitmap();
	ChartInfo.Offset = Classes::Point(0, 0);
	Paused = false;
	TimeAdd = 0;
	TimeFlow = 1;
	Transform = -20;
	rX = 0;
	rY = -60;   
	ChartInfo.Scale = 20;
	ChartInfo.TimePos = 0;
	ChartInfo.Type = RT_LINES;

	ChartInfo.LineColor = clRed;
}
//---------------------------------------------------------------------------
TTabData::~TTabData()
{
	delete LastDraw;
	if (CalcData != NULL) delete CalcData;
}
//---------------------------------------------------------------------------
void TTabData::Set(String aXv, String aYv, bool aHaveParam, String aPv,
	int aTimeStep, TChartMode aMode)
{
	TimeStep = aTimeStep;
	ChartInfo.HaveParam = aHaveParam;
	ChartInfo.vX = aXv;
	ChartInfo.vY = aYv;
	ChartInfo.vP = aPv;
	ChartInfo.Mode = aMode;
	if (ChartInfo.HaveParam) {
		TimeAdd = 1;
	} else TimeAdd = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TFChartWindow::TFChartWindow(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TFChartWindow::Add2DChart(TChart *Chart, String vX, String vP,
		bool HaveParam,	double TimeStep, bool RefActive)
{
	Show();
	TTabData *Tab;
	if (!RefActive) Tab = AddTab(NULL);
	else Tab = ActiveTab;
	Tab->Set(vX, "", HaveParam, vP, TimeStep, cm2DChart);
	if(!RefActive) {
		Tab->Expression = Chart->Expression;
		Tab->AdditionalData = Chart->GetVariblesQueue();
		Tab->Name = Tab->Expression;
		Tab->CalcData->Clear();
		RefreshTabs(!RefActive);
	} else D2Redraw();
}
//---------------------------------------------------------------------------
void TFChartWindow::D2Redraw(bool ReCalc)
{
	if (ActiveTab == NULL || OwnChart->Working)return;
	ActiveTab->ChartInfo.Width = iDraw->Width;
	ActiveTab->ChartInfo.Height = iDraw->Height;
	if(ReCalc){
		ActiveTab->CalcData->Clear();
		ActiveTab->ChartInfo.TimePos = 0;
	}
	if (ActiveTab->CalcData->TableEnd && !ReCalc){
		OwnChart->Draw2DChart(ActiveTab->CalcData, iDraw->Picture->Bitmap, ActiveTab->ChartInfo);
	} else {
		OwnChart->SetExpression(ActiveTab->Expression);
		OwnChart->SetVariblesQueue(ActiveTab->AdditionalData);
		OwnChart->QuickChartDraw(iDraw->Picture->Bitmap, &ActiveTab->ChartInfo, ActiveTab->CalcData, true);
		OwnChart->PrepareChartInfo(&ActiveTab->ChartInfo, ActiveTab->CalcData);
	}
	iDraw->Picture->Bitmap->Canvas->TextOut(0, 0, ActiveTab->Name);
	ActiveTab->LastDraw->Assign(iDraw->Picture->Bitmap);
}
//---------------------------------------------------------------------------
void TFChartWindow::Add3DChart(TChart *Chart, String Xvar, String Yvar,
		bool nHaveParam, float nTimeStep, String Pvar, bool RefActive)
{
	Show();
	TTabData *Tab;
	if (!RefActive) Tab = AddTab(NULL);
	else Tab = ActiveTab;
	Tab->Set(Xvar, Yvar, nHaveParam, Pvar, nTimeStep, cm3DChart);
	if (Tab->ChartInfo.HaveParam)Tab->ChartInfo.Type = RT_LINES;
	if(!RefActive) {
		Tab->Expression = Chart->Expression;
		Tab->AdditionalData = Chart->GetVariblesQueue();
		Tab->Name = Tab->Expression;
		Tab->CalcData->Clear();
		RefreshTabs(!RefActive);
	} else {
		Chart->PrepareChartInfo(&Tab->ChartInfo, Tab->CalcData);
		TabControlChange(NULL);
		FormResize(NULL);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormCreate(TObject *Sender)
{
	Tabs = new TList;
	OwnChart = new TChart(FMainForm->ControllClass);
	TabControl->Align = alClient;
	TabControl->Tabs->Clear();
	ActiveTab = NULL;
	glWindow->Align = alClient;
	glWindow->Resize();
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormDestroy(TObject *Sender)
{
	FMainForm->ChartWindowClose(this);

	for (int i = 0; i < Tabs->Count; i++) {
		delete (TTabData*)Tabs->Items[i];
	}
	Tabs->Clear();

	delete Tabs;
	delete OwnChart;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormClose(TObject *Sender, TCloseAction &Action)
{
	Timer1->Enabled = false;
  	Action = caFree;
}
//---------------------------------------------------------------------------
void TFChartWindow::SetSize(int H, int W)
{
	ClientHeight = H;
	ClientWidth = W;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Timer1Timer(TObject *Sender)
{
	if (ActiveTab == NULL)return;

	if (ActiveTab->ChartInfo.HaveParam && !ActiveTab->Paused) {
		ActiveTab->ChartInfo.TimePos += ActiveTab->TimeAdd;
		if (ActiveTab->ChartInfo.TimePos >= ActiveTab->ChartInfo.PCount ||
			ActiveTab->ChartInfo.TimePos - 1 < 0) {
			if (ActiveTab->TimeFlow == 1) {
				ActiveTab->ChartInfo.TimePos = 0;
			} else {
				ActiveTab->TimeAdd = -ActiveTab->TimeAdd;
				ActiveTab->ChartInfo.TimePos += ActiveTab->TimeAdd;
			}
		}
	}
	if (ActiveTab->CalcData->TableEnd)Status->Panels->operator [](2)->Text = "";
	else Status->Panels->operator [](2)->Text = "Liczenie " + FloatToStr(RoundTo(((double)ActiveTab->ChartInfo.TimePos / (double)ActiveTab->ChartInfo.PCount) * 100, 0)) + "%     ";
	
	switch (ActiveTab->ChartInfo.Mode) {
		case cm2DChart:
			D2Redraw();
		break;
		case cm3DChart:
			glWindow->ReDraw();
		break;
	}  
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::glWindowDraw(TObject *Sender)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glTranslatef(0, 0, ActiveTab->Transform);
	glRotatef(ActiveTab->rX, 0.0, 1.0, 0.0);
	glRotatef(ActiveTab->rY, 1.0, 0.0, 0.0);

	if (ActiveTab->CalcData->TableEnd){
		OwnChart->Render3DChart(ActiveTab->CalcData, ActiveTab->ChartInfo);
	} else {
		OwnChart->SetExpression(ActiveTab->Expression);
		OwnChart->SetVariblesQueue(ActiveTab->AdditionalData);

		OwnChart->Quick3DChart(ActiveTab->CalcData, &ActiveTab->ChartInfo);

		OwnChart->PrepareChartInfo(&ActiveTab->ChartInfo, ActiveTab->CalcData);
	}     
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::glWindowInit(TObject *Sender)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::glWindowMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
	if (MouseDown && ActiveTab != NULL){
		ActiveTab->rX -= (LastMousePos.x - X) / 2;
		ActiveTab->rY -= (LastMousePos.y - Y) / 2;       
		LastMousePos = Classes::Point(X, Y);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::glWindowMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (ActiveTab != NULL && Button == mbLeft) {
		LastMousePos = Classes::Point(X, Y);
		MouseDown = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::glWindowMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft) {
		LastMousePos = Classes::Point(X, Y);
		MouseDown = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Zapisz1Click(TObject *Sender)
{
	SaveDialog->Filter = "Obraz wykresu (*.bmp)|*.bmp";
	SaveDialog->DefaultExt = "bmp";
	if (!SaveDialog->Execute()) return;
	if(ActiveTab->ChartInfo.Mode != cm3DChart){//2D
		iDraw->Picture->SaveToFile(SaveDialog->FileName);
	} else {//3D
		int Length = 3 * glWindow->Width * glWindow->Height;
		unsigned char *Bitmap = new unsigned char[Length];
		glReadPixels(0, 0, glWindow->Width, glWindow->Height, GL_RGB,
					 GL_UNSIGNED_BYTE, Bitmap);

		Graphics::TBitmap *BMP = new Graphics::TBitmap();
		BMP->SetSize(glWindow->Width, glWindow->Height);
		BMP->PixelFormat = pf24bit;
		int Pos = 0;
		for (int i = 0; i < BMP->Height; i++) {
			unsigned char *ScanMap = (unsigned char *)BMP->ScanLine[i];
			for (int j = BMP->Width - 1 ; j >= 0 ; j--) {
				ScanMap[j * 3] = Bitmap[Length - Pos + 2];
				ScanMap[j * 3 + 1] = Bitmap[Length - Pos + 1];
				ScanMap[j * 3 + 2] = Bitmap[Length - Pos];
				Pos += 3;
			}
		}
		BMP->Canvas->Brush->Style = bsClear;
		BMP->Canvas->Font->Color = clWhite;
		BMP->Canvas->TextOut(3, 3, ActiveTab->Name);
		BMP->SaveToFile(SaveDialog->FileName);
		delete BMP;
		delete[] Bitmap;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Zapiszdanewykresu1Click(TObject *Sender)
{
	if (ActiveTab == NULL)return;

	SaveDialog->Filter = "Plik kalkulacji (*.mcf)|*.mcf";
	SaveDialog->DefaultExt = "mcf";
	if (!SaveDialog->Execute()) return;
	ActiveTab->CalcData->SaveToFile(SaveDialog->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Opjewykresu1Click(TObject *Sender)
{
	if (ActiveTab == NULL) return;
	Timer1->Enabled = false;
	TFChartPropertis *NewForm = new TFChartPropertis(this);

	if (ActiveTab->ChartInfo.Mode == 0)NewForm->cbOX->Items->Text = ActiveTab->AdditionalData;
	else ActiveTab->CalcData->GetVaribles(NewForm->cbOX->Items);
	NewForm->cbOY->Items->Assign(NewForm->cbOX->Items);

	NewForm->rgChartType->ItemIndex = ActiveTab->ChartInfo.Mode;
	NewForm->cbShowNet->Checked = ActiveTab->ChartInfo.Type == RT_LINES;

	NewForm->cbOY->ItemIndex = NewForm->cbOY->Items->IndexOf(ActiveTab->ChartInfo.vY);
	NewForm->cbOX->ItemIndex = NewForm->cbOX->Items->IndexOf(ActiveTab->ChartInfo.vX);

	NewForm->cbAnimateChart->Checked = ActiveTab->ChartInfo.HaveParam;
	NewForm->fseTimeStep->Value = ActiveTab->TimeStep;

	NewForm->pLineColor->Color = ActiveTab->ChartInfo.LineColor;

	if (NewForm->ShowModal() == mrOk) {
		if (NewForm->cbShowNet->Checked) ActiveTab->ChartInfo.Type = RT_LINES;
		else ActiveTab->ChartInfo.Type = RT_QUADS;

		ActiveTab->ChartInfo.TimePos = 0;

		glWindow->Visible = false;
		Timer1->Enabled = false;
		glWindow->OnDraw = NULL;
		ActiveTab->ChartInfo.LineColor = NewForm->pLineColor->Color;
		switch (NewForm->rgChartType->ItemIndex) {
			case cm2DChart:
				Add2DChart(OwnChart, NewForm->cbOX->Text,
					NewForm->lParam->Caption, NewForm->cbAnimateChart->Checked,
					NewForm->fseTimeStep->Value, true);
			break;
			case cm3DChart:
				Add3DChart(OwnChart, NewForm->cbOX->Text,
					NewForm->cbOY->Text, NewForm->cbAnimateChart->Checked,
					NewForm->fseTimeStep->Value, NewForm->lParam->Caption, true);
			break;
		}
	}

	delete NewForm;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormResize(TObject *Sender)
{
	
	if (ActiveTab == NULL || ActiveTab->ChartInfo.HaveParam) return;
	switch (ActiveTab->ChartInfo.Mode) {
		case cm2DChart:
			D2Redraw();
		break;
		case cm3DChart:
			glWindow->Resize();
		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormMouseWheelDown(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled)
{
	if (ActiveTab == NULL)return;
	if (ActiveTab->ChartInfo.Mode == cm3DChart)ActiveTab->Transform += 2;
	else{
		ActiveTab->ChartInfo.Scale -= 5;
		if (ActiveTab->ChartInfo.Scale < 5) ActiveTab->ChartInfo.Scale = 5;
		D2Redraw(true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormMouseWheelUp(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled)
{
	if (ActiveTab == NULL)return;
	if (ActiveTab->ChartInfo.Mode == cm3DChart) ActiveTab->Transform -= 2;
	else{
		ActiveTab->ChartInfo.Scale += 5;
		if (ActiveTab->ChartInfo.Scale > 300) ActiveTab->ChartInfo.Scale = 300;
		D2Redraw(true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::i2DChartMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (ActiveTab != NULL && Button == mbLeft) {
		MouseDown = true;
		LastMousePos = Classes::Point(X, Y);
		DrawOffset = Classes::Point(X, Y);
		Timer1->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::i2DChartMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (ActiveTab != NULL && Button == mbLeft) {
		MouseDown = false;
		Timer1->Enabled = ActiveTab->ChartInfo.HaveParam;
		D2Redraw(true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::i2DChartMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
	if (ActiveTab == NULL)return;
	if(MouseDown){
		ActiveTab->ChartInfo.Offset.x += LastMousePos.x - X;
		ActiveTab->ChartInfo.Offset.y += LastMousePos.y - Y;
		LastMousePos = Classes::Point(X, Y);

		iDraw->Picture->Bitmap->Canvas->Brush->Style = bsSolid;
		iDraw->Picture->Bitmap->Canvas->Rectangle(-1, -1, iDraw->Width + 1, iDraw->Height + 1);
		iDraw->Picture->Bitmap->Canvas->Draw(X - DrawOffset.x, Y - DrawOffset.y,
				 ActiveTab->LastDraw);
	} else {                  
		Status->Panels->operator [](0)->Text = "x: " + FloatToStr(RoundTo((X - ActiveTab->ChartInfo.Width  / 2 + ActiveTab->ChartInfo.Offset.x) / (double)ActiveTab->ChartInfo.Scale, -3));
		Status->Panels->operator [](1)->Text = "y: " + FloatToStr(RoundTo(-(Y - ActiveTab->ChartInfo.Height / 2 + ActiveTab->ChartInfo.Offset.y) / (double)ActiveTab->ChartInfo.Scale, -3));
	}
}
//---------------------------------------------------------------------------
TTabData* TFChartWindow::GetTab(int Index)
{
	if (Index >= Tabs->Count || Index < 0)return NULL;
	return (TTabData*)Tabs->Items[Index];
}
//---------------------------------------------------------------------------
TTabData* TFChartWindow::AddTab(TCalcResult* NewResult)
{
	TTabData *NewTab = new TTabData(NewResult);
	if (NewResult != NULL) NewTab->Name = NewResult->Expression;
	NewTab->ChartInfo.LineColor = (TColor)RGB(Random(256), Random(256), Random(256));
	Tabs->Add(NewTab);
	return NewTab;
}
//---------------------------------------------------------------------------
void TFChartWindow::RefreshTabs(bool SelectLast)
{
	Timer1->Enabled = false;
	if (Tabs->Count == 0) {
		Close();
		return;
	}
	TabControl->Tabs->BeginUpdate();
	String TabName = "";
	if (TabControl->TabIndex != -1) {
		TabName = TabControl->Tabs->operator [](TabControl->TabIndex);
	}
	
	TabControl->Tabs->Clear();
	for (int i = 0; i < Tabs->Count; i++) {
		TabControl->Tabs->Add(GetTab(i)->Name);
	}

	if(TabControl->Tabs->IndexOf(TabName) == -1 || TabName == ""){
		TabControl->TabIndex = 0;
	} else
		TabControl->TabIndex = TabControl->Tabs->IndexOf(TabName);
	if (SelectLast)TabControl->TabIndex = TabControl->Tabs->Count - 1;
	TabControl->Tabs->EndUpdate();
	iDraw->Align = alClient;  
	TabControlChange(NULL);
	FormResize(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::TabControlChange(TObject *Sender)
{
	Timer1->Enabled = false;
	ActiveTab = GetTab(TabControl->TabIndex);
	if (ActiveTab == NULL)return;
	
	glWindow->Visible = ActiveTab->ChartInfo.Mode == cm3DChart;
	if (ActiveTab->ChartInfo.Mode != cm3DChart) {
		iDraw->Picture->Bitmap->Assign(ActiveTab->LastDraw);
		glWindow->OnDraw = NULL;
		ActiveTab->ChartInfo.TimePos = 0;
	}else{
		glWindow->OnDraw = glWindowDraw;
	}
	if (ActiveTab->ChartInfo.HaveParam) {
		Timer1->Interval = 500/(1 + ActiveTab->TimeStep * 2.5) + 1;
	} else {
		Timer1->Interval = 100;
	}
	Timer1->Enabled = ActiveTab->ChartInfo.Mode == cm3DChart || ActiveTab->ChartInfo.HaveParam;
	Caption = ActiveTab->Name;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::PopupMenu1Popup(TObject *Sender)
{
	Pauza1->Visible = ActiveTab->ChartInfo.HaveParam;
	N3->Visible = Pauza1->Visible;
	Zaptlczas1->Visible = Pauza1->Visible;
	Pauza1->Checked = ActiveTab->Paused;
	Zaptlczas1->Checked = ActiveTab->TimeFlow == 2;
	Zapiszdanewykresu1->Enabled = ActiveTab->CalcData->TableEnd;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Zamknij1Click(TObject *Sender)
{
	if (ActiveTab == NULL)return;

	Tabs->Remove(ActiveTab);
	delete ActiveTab;
	ActiveTab = NULL;
	int ATab = TabControl->TabIndex;
	if (Tabs->Count == 0) {
		Close();
		return;
	}
	RefreshTabs();
	if (ATab >= Tabs->Count) {
		TabControl->TabIndex = Tabs->Count - 1;
	} else
		TabControl->TabIndex = ATab;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Reset1Click(TObject *Sender)
{
	ActiveTab->ChartInfo.Offset = Classes::Point(0, 0);
	ActiveTab->ChartInfo.Scale = 20;
	ActiveTab->Transform = -20;
	ActiveTab->rX = 0;
	ActiveTab->rY = -60;
	if (Sender == NULL) return;
	if (ActiveTab->ChartInfo.Mode == cm2DChart) D2Redraw(true);
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Pauza1Click(TObject *Sender)
{
	if (ActiveTab == NULL || !ActiveTab->ChartInfo.HaveParam) return;
	ActiveTab->Paused = !ActiveTab->Paused;
	Pauza1->Checked = ActiveTab->Paused;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	switch (Key) {
		case 'z': Zamknij1Click(NULL); break;
		case 's': /*if (Shift.Contains(ssCtrl))Zapiszdanewykresu1Click(NULL);
				  else */Zapisz1Click(NULL); break;
		case 'o': Opjewykresu1Click(NULL); break;
		case 'r': Reset1Click(NULL); break;
		case 'p': Pauza1Click(NULL); break;
		case 'c': Zaptlczas1Click(NULL); break;
		case 'w': Wasnocifunkcji1Click(NULL); break;
     //	case 'e': EksportujdoplikuExcel1Click(NULL); break;
//		case '':  break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Zaptlczas1Click(TObject *Sender)
{
	if (ActiveTab == NULL) return;
	if (ActiveTab->TimeFlow == 1){
		ActiveTab->TimeFlow = 2;
	}else {
		ActiveTab->TimeFlow = 1;
		ActiveTab->TimeAdd = 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::Wasnocifunkcji1Click(TObject *Sender)
{
	if (ActiveTab == NULL) return;
	TFFncProperity *Window = new TFFncProperity(this);
	Window->ShowProperity(ActiveTab->CalcData);
	Window->ShowModal();
	delete Window;
}
//---------------------------------------------------------------------------
void __fastcall TFChartWindow::EksportujdoplikuExcel1Click(TObject *Sender)
{
	TFExport *E = new TFExport(this);
	E->Prepare(ActiveTab->CalcData);
	E->ShowModal();
	delete E;
}
//---------------------------------------------------------------------------

