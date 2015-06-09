//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "Utils.h"
#include "ConstForm.h"
#include "FncForm.h"
#include "BinMath.hpp"
#include "VersionInfo.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FloatSpinEdit"
#pragma link "cspin"
#pragma link "perfgrap"
#pragma resource "*.dfm"
TFMainForm *FMainForm;
//---------------------------------------------------------------------------
TFChartWindow* TFMainForm::CreateChartWindow(TChart* NewChart)
{
	TFChartWindow* NewWindow = new TFChartWindow(this);

	NewWindow->OwnChart->Assign(NewChart);
	NewWindow->SetSize(400, 400);

	WindowsList->Add(NewWindow);
	return NewWindow;
}
//---------------------------------------------------------------------------
__fastcall TFMainForm::TFMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::FormCreate(TObject *Sender)
{
	if (ParamCount() > 0 && ParamStr(1).Pos("debug") > 0){
		DebugMode = true;
	} else DebugMode = false;
	TabSheet4->TabVisible = DebugMode;
	TabSheet5->TabVisible = DebugMode;

	ControllClass = new TControllClass(Memo1->Lines);
	CalcResult = new TCalcResult(ControllClass);
	Chart = new TChart(ControllClass);
	ControllClass->VariblesList->LoadVaribles(ReadIniSection("VariblesList", IniFileName));
	ControllClass->FunctionsList->LoadFunctions(ReadIniSection("FunctionsList", IniFileName));

	//wcztujemy opcje z pliku
	TIniFile *ini = new TIniFile(IniFileName);
	leChartExpression->Text = ini->ReadString("Last", "Chart", "sin(x)");
	PageControl1->TabIndex = ini->ReadInteger("Last", "Tab", 0);
	eTest->Text = ini->ReadString("Test", "Data", "2(3+4)");
	delete ini;
	
	//ró¿ne
	WindowsList = new TList();
	rgChartTypeClick(NULL);
	cbDrawChartAddClick(NULL);
	cbAnimateChartClick(NULL);
	cbHaveRangeClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::FormDestroy(TObject *Sender)
{
	TIniFile *ini = new TIniFile(IniFileName);
	ini->WriteString("Last", "Chart", leChartExpression->Text);
	ini->WriteInteger("Last", "Tab", PageControl1->TabIndex);
	ini->WriteString("Test", "Data", eTest->Text);
	delete ini;
	WriteIniSection("VariblesList", ControllClass->VariblesList->SaveVaribles(), IniFileName);
	WriteIniSection("FunctionsList", ControllClass->FunctionsList->SaveFunctions(), IniFileName);

	delete Chart;
	delete CalcResult;
	delete ControllClass;

	for (int i = 0; i < WindowsList->Count; i++) {
		delete (TFChartWindow*) WindowsList->Items[i];
	}
	WindowsList->Clear();
	delete WindowsList;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::rgChartTypeClick(TObject *Sender)
{
	LabelOY->Enabled = rgChartType->ItemIndex == 1;
	cbOY->Enabled = rgChartType->ItemIndex == 1;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::cbDrawChartAddClick(TObject *Sender)
{
	cbChartWindow->Enabled = cbDrawChartAdd->Checked;
	cbChartWindowDropDown(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::cbAnimateChartClick(TObject *Sender)
{
	lParam->Enabled = cbAnimateChart->Checked;
	lTimeStep->Enabled = cbAnimateChart->Checked;
	lParamName->Enabled = cbAnimateChart->Checked;
	fseTimeStep->Enabled = cbAnimateChart->Checked;
	if (cbOX->Items->Count > 1) {
		if (cbAnimateChart->Checked) {
			cbOX->ItemIndex++;
		} else cbOX->ItemIndex--;
		if (cbOX->ItemIndex < 0) cbOX->ItemIndex = 0;
	}
	if (cbOX->Items->Count > 0) {
		lParam->Caption = lbVaribles->Items->operator [](0);
	} else lParam->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::leChartExpressionChange(TObject *Sender)
{
	if (!Chart->SetExpression(leChartExpression->Text)) {
		StatusBar->SimpleText = "W wyra¿eniu jest b³¹d!";
		gbVaribles->Enabled = false;
		cDraw->Enabled = false;
	} else {
		gbVaribles->Enabled = true;
		cDraw->Enabled = true;
		lbVaribles->Items->Text = Chart->GetVariblesQueue();
		cbOY->Items->Text = Chart->GetVariblesQueue();
		cbOX->Items->Text = Chart->GetVariblesQueue();
		if (lbVaribles->Items->Count > 0){
			lbVaribles->ItemIndex = 0;
			cbOX->ItemIndex = 0;
			cbOY->ItemIndex = 0;
			if (cbOY->Items->Count > 1)cbOY->ItemIndex = 1;
		}
	}
	Modiffied = true;	
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::lbVariblesClick(TObject *Sender)
{
	if (lbVaribles->ItemIndex < 0) return;
	int Index = lbVaribles->ItemIndex;
	lbVaribles->ItemIndex = -1;

	TVarible *Var = Chart->GetVarible(lbVaribles->Items->operator [](Index));
	lseVarFrom->Value = Var->InitialValue;
	lseVarTo->Value = Var->EndValue;
	cbHaveRange->Checked = Var->HaveRange;

	lbVaribles->ItemIndex = Index;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::lseVarFromChange(TObject *Sender)
{
	if (lbVaribles->ItemIndex > -1) {
		TVarible *Var = Chart->GetVarible(lbVaribles->Items->operator [](lbVaribles->ItemIndex));
		Var->Set(lseVarFrom->Value, 0, lseVarTo->Value);
		Modiffied = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::CSpinButton1DownClick(TObject *Sender)
{
	if (lbVaribles->ItemIndex < 0) return;
	String Name = lbVaribles->Items->operator [](lbVaribles->ItemIndex);
	Chart->MoveVarible(lbVaribles->Items->operator [](lbVaribles->ItemIndex), false);
	lbVaribles->Items->Text = Chart->GetVariblesQueue();
	lbVaribles->ItemIndex = lbVaribles->Items->IndexOf(Name);
	cbAnimateChartClick(NULL);
}
//------------------------------------------------------------------------------
void __fastcall TFMainForm::CSpinButton1UpClick(TObject *Sender)
{
	if(lbVaribles->ItemIndex < 0) return;
	String Name = lbVaribles->Items->operator [](lbVaribles->ItemIndex);
	Chart->MoveVarible(lbVaribles->Items->operator [](lbVaribles->ItemIndex), true);
	lbVaribles->Items->Text = Chart->GetVariblesQueue();
	lbVaribles->ItemIndex = lbVaribles->Items->IndexOf(Name);
	cbAnimateChartClick(NULL);
}
//------------------------------------------------------------------------------
void TFMainForm::ChartWindowClose(TFChartWindow* Window)
{
	WindowsList->Remove(Window);
}
//------------------------------------------------------------------------------
void __fastcall TFMainForm::cDrawClick(TObject *Sender)
{
	TFChartWindow *NewWindow;
	if (cbDrawChartAdd->Checked && cbChartWindow->ItemIndex != -1 &&
		WindowsList->Count > cbChartWindow->ItemIndex){
		NewWindow = (TFChartWindow*)WindowsList->Items[cbChartWindow->ItemIndex];
	} else NewWindow = CreateChartWindow(Chart);

	switch (rgChartType->ItemIndex) {
		case 0:{//2D
			NewWindow->Add2DChart(Chart, cbOX->Text, lParam->Caption,
					cbAnimateChart->Checked, fseTimeStep->Value);
		}
		break;
		case 1:{//3D
			NewWindow->Add3DChart(Chart, cbOX->Text, cbOY->Text,
				cbAnimateChart->Checked, fseTimeStep->Value, lParam->Caption);
		}
		break;
	}
}
//------------------------------------------------------------------------------
void __fastcall TFMainForm::mExpressionChange(TObject *Sender)
{
	TWord *Word = new TWord(ControllClass);
	if(Word->CheckProperity(mExpression->Lines->Text) &&
	   Word->SetSelf(mExpression->Lines->Text)) {
		leCalcResult->Text = Word->StringCalculate();
	} else leCalcResult->Text = "W wyra¿eniu jest b³¹d!!";
	delete Word;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::cbChartWindowDropDown(TObject *Sender)
{
	cbChartWindow->Items->BeginUpdate();
	int AIndex = cbChartWindow->ItemIndex;
	cbChartWindow->Items->Clear();
	for (int i = 0; i < WindowsList->Count; i++) {
		cbChartWindow->Items->Add(((TFChartWindow*)WindowsList->Items[i])->Caption);
	}
	if(AIndex == -1)AIndex = 0;
	if(cbChartWindow->Items->Count <= AIndex)cbChartWindow->ItemIndex = 0;
	else cbChartWindow->ItemIndex = AIndex;

	cbChartWindow->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::Button1Click(TObject *Sender)
{
	TFIntConst *Window = new TFIntConst(this);
	Window->LoadList(ControllClass->VariblesList);
	Window->ShowModal();
	delete Window;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::fseTimeStepChange(TObject *Sender)
{
	if (fseTimeStep->Value < 1) fseTimeStep->Value = 1;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::Button2Click(TObject *Sender)
{
	TFFncEdt *Window = new TFFncEdt(this);
	Window->LoadList(ControllClass->FunctionsList);
	Window->ShowModal();
	delete Window;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::bTestClick(TObject *Sender)
{
	Memo1->Lines->Clear();
	TTransform *T = new TTransform(ControllClass);
	String Data = eTest->Text;
	if (T->Simplify(&Data)) {
		eOutTest->Text = Data;
	} else eOutTest->Text = "b³¹d!!";
	delete T;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::Button3Click(TObject *Sender)
{
	unsigned int StartTime = GetTickCount();
	unsigned int ATime;
	Memo2->Lines->Add("");
	Memo2->Lines->Add(RadioGroup1->Items->operator [](RadioGroup1->ItemIndex));
	switch (RadioGroup1->ItemIndex) {
		case 0:{
			TWord *Pattern = new TWord(ControllClass);
			TWord *Calculator = new TWord(ControllClass);
			Pattern->SetSelf(Edit1->Text);
			for (int i = 0; i < 5; i++) {
				ATime = GetTickCount();
				for (int j = 0; j < 10000; j++) {
					Calculator->Assign(Pattern);
				   //	Memo3->Lines->Add(
					Calculator->Calculate();
				   //	);
				}
				Memo2->Lines->Add("czas: " + IntToStr(GetTickCount() - ATime));
			}
			delete Pattern;
			delete Calculator;
		}
		break;
		case 1:{
			TONPCalculator *Calculator = new TONPCalculator(ControllClass);
			Calculator->SetSelf(Edit1->Text);
			for (int i = 0; i < 5; i++) {
				ATime = GetTickCount();
				for (int j = 0; j < 10000; j++) {
				   //	Memo3->Lines->Add(
					Calculator->Calculate();
				   //	);
				}
				Memo2->Lines->Add("czas: " + IntToStr(GetTickCount() - ATime));
			}
			delete Calculator;
		}
		break;
		case 2:{
			TObliczenia *ONP = new TObliczenia();
			ONP->Wyrazenie(Edit1->Text);
			ONP->ZmienneDodaj("x", 5.0);
			for (int i = 0; i < 5; i++) {
				ATime = GetTickCount();
				for (int j = 0; j < 10000; j++) {
				   //	Memo3->Lines->Add(
					ONP->Wynik();
				   //	);
				}
				Memo2->Lines->Add("czas: " + IntToStr(GetTickCount() - ATime));
			}
			delete ONP;
		}
		break;
		case 3:
			for (int i = 0; i < 5; i++) {
				ATime = GetTickCount();
				for (int j = 0; j < 10000; j++) {
					Interpreter(Edit1->Text);
				}
				Memo2->Lines->Add("czas: " + IntToStr(GetTickCount() - ATime));
			}
		break;
		case 4:
			for (int i = 0; i < 5; i++) {
				ATime = GetTickCount();
				for (int j = 0; j < 10000; j++) {
					Eval(Edit1->Text);
				}
				Memo2->Lines->Add("czas: " + IntToStr(GetTickCount() - ATime));
			}
		break;
	}
	Memo2->Lines->Add("ca³kowity czas: " + IntToStr(GetTickCount() - StartTime));
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::Button4Click(TObject *Sender)
{
	Memo2->Lines->Clear();	
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::cbOYDropDown(TObject *Sender)
{
	String Text = cbOX->Text;
	cbOX->Items->Text = lbVaribles->Items->Text;
	int index = cbOX->Items->IndexOf(Text);
	if (index > 0) cbOX->ItemIndex = index;
	else cbOX->ItemIndex = 0;

	Text = cbOY->Text;
	cbOY->Items->Text = lbVaribles->Items->Text;
	index = cbOY->Items->IndexOf(Text);
	if (index >= 0) cbOY->ItemIndex = index;
	else cbOY->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFMainForm::cbHaveRangeClick(TObject *Sender)
{
	Label3->Enabled = cbHaveRange->Checked;
	Label4->Enabled = cbHaveRange->Checked;
	lseVarFrom->Enabled = cbHaveRange->Checked;
	lseVarTo->Enabled = cbHaveRange->Checked;

	if (lbVaribles->ItemIndex > -1) {
		TVarible *Var = Chart->GetVarible(lbVaribles->Items->operator [](lbVaribles->ItemIndex));
		Var->Set(lseVarFrom->Value, 0, lseVarTo->Value);
		Var->HaveRange = cbHaveRange->Checked;
		Modiffied = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFMainForm::Button5Click(TObject *Sender)
{
	Button5->Caption = GetVersion(Application->ExeName);
}
//---------------------------------------------------------------------------

