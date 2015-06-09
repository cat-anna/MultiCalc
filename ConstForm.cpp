//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ConstForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FloatSpinEdit"
#pragma resource "*.dfm"
TFIntConst *FIntConst;
//---------------------------------------------------------------------------
__fastcall TFIntConst::TFIntConst(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TFIntConst::LoadList(TVariblesList *List)
{
	VariblesList = List;
	Reload();
}
//---------------------------------------------------------------------------
void TFIntConst::Reload()
{
	ListView1->Items->BeginUpdate();
	ListView1->Items->Clear();
	for (int i = 0; i < VariblesList->VariblesCount; i++) {
		TVarible *Varible = VariblesList->GetVarible(i);
		if (Varible->Constans) {
			TListItem *Item = ListView1->Items->Add();
			Item->Caption = Varible->Name;
			Item->SubItems->Add(Varible->Value);
		}
	}
	ListView1->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFIntConst::Button2Click(TObject *Sender)
{
	if (VariblesList->VaribleExists(leConstName->Text)) {
		VariblesList->ModifyVarible(leConstName->Text, FloatSpinEdit1->Value, 0, 0);
	} else {
		VariblesList->AddVarible(leConstName->Text, FloatSpinEdit1->Value);
	}
	Reload();
}
//---------------------------------------------------------------------------
void __fastcall TFIntConst::Button1Click(TObject *Sender)
{
	VariblesList->DeleteVarible(leConstName->Text);
	Reload();
}
//---------------------------------------------------------------------------
void __fastcall TFIntConst::ListView1Click(TObject *Sender)
{
	if (ListView1->ItemIndex > -1) {
		TListItem* itm = ListView1->Items->Item[ListView1->ItemIndex];
		leConstName->Text = itm->Caption;
		FloatSpinEdit1->Value = VariblesList->GetValue(leConstName->Text);
	}
}
//---------------------------------------------------------------------------

