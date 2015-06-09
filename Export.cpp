//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Export.h"
#include "MainForm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FloatSpinEdit"
#pragma resource "*.dfm"
TFExport *FExport;
//---------------------------------------------------------------------------
__fastcall TFExport::TFExport(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TFExport::Prepare(TCalcResult* Result)
{
	Expression = Result->Expression;
	Label1->Caption = "Wzór: " + Expression;

	lbVaribles->Items->BeginUpdate();
	lbVaribles->Items->Clear();
	for (int i = 0; i < Result->VariblesList->VariblesCount; i++) {
		TVarible *V = Result->VariblesList->GetVarible(i);
		V->Set(-8.0, 0.1, 8.0);
		ExportVar->AddVarible(V);
		lbVaribles->Items->Add(V->Name);
	}
	lbVaribles->Items->EndUpdate();
	lbVaribles->ItemIndex = 0;
	lbVariblesClick(NULL);
	fseDeltaChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFExport::FormCreate(TObject *Sender)
{
	ExportVar = new TVariblesList(FMainForm->ControllClass);
}
//---------------------------------------------------------------------------
void __fastcall TFExport::FormDestroy(TObject *Sender)
{
	delete ExportVar;
}
//---------------------------------------------------------------------------
void __fastcall TFExport::Button1Click(TObject *Sender)
{
	if (!SaveDialog->Execute()) return;

 /*	ExcelApp->Connect();
	ExcelWork->ConnectTo(ExcelApp->Workbooks->Add(void, 0));
	TExcelWorksheet *Arkusz = new TExcelWorkSheet(this);
	Arkusz->ConnectTo(ExcelWork->Worksheet->Add(NULL, NULL, NULL, NULL, 0));
	ExcelApp->Visible[0] = false;
	ExcelApp->ScreenUpdating[0] = false;



	ExcelWork->Disconnect();
	ExcelApp->Disconnect();
	Arkusz->Free;
				 */
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFExport::lbVariblesClick(TObject *Sender)
{
	if (lbVaribles->ItemIndex == -1) return;
	TVarible *V = ExportVar->GetVarible(lbVaribles->ItemIndex);
	int Index = lbVaribles->ItemIndex;
	lbVaribles->ItemIndex = -1;

	lseVarFrom->Value = V->InitialValue;
	lseVarTo->Value = V->EndValue;
	fseDelta->Value = V->Delta;

	lbVaribles->ItemIndex = Index;
}
//---------------------------------------------------------------------------
void __fastcall TFExport::fseDeltaChange(TObject *Sender)
{
	if (lbVaribles->ItemIndex == -1) return;
	TVarible *Var = ExportVar->GetVarible(lbVaribles->ItemIndex);
	Var->Set(lseVarFrom->Value, fseDelta->Value, lseVarTo->Value);

	int CalcCount = 1;
	for (int i = 0; i < ExportVar->VariblesCount; i++) {
		CalcCount *= ExportVar->GetVaribleCalcCount(i);
	}
	Label2->Caption = "Iloœæ kobinacji: " + IntToStr(CalcCount);
}
//---------------------------------------------------------------------------

