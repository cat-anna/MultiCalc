//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ChartPropertis.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FloatSpinEdit"
#pragma link "cspin"
#pragma resource "*.dfm"
TFChartPropertis *FChartPropertis;
//---------------------------------------------------------------------------
__fastcall TFChartPropertis::TFChartPropertis(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFChartPropertis::rgChartTypeClick(TObject *Sender)
{
	LabelOY->Enabled = rgChartType->ItemIndex == 1;
	cbOY->Enabled = rgChartType->ItemIndex == 1;
	cbShowNet->Enabled = rgChartType->ItemIndex == 1;
	pLineColor->Visible = rgChartType->ItemIndex != 1;
}
//---------------------------------------------------------------------------
void __fastcall TFChartPropertis::cbAnimateChartClick(TObject *Sender)
{
	lParam->Enabled = cbAnimateChart->Checked;
	lTimeStep->Enabled = cbAnimateChart->Checked;
	lParamName->Enabled = cbAnimateChart->Checked;
	fseTimeStep->Enabled = cbAnimateChart->Checked;
	if (cbOX->Items->Count > 0) {
		lParam->Caption = cbOX->Items->operator [](0);
	} else lParam->Caption = "";
	cbShowNet->Enabled = !cbAnimateChart->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFChartPropertis::Button1Click(TObject *Sender)
{
	if((rgChartType->ItemIndex == 0 && cbOX->ItemIndex == -1) ||
	   (rgChartType->ItemIndex == 1 && (cbOX->ItemIndex == -1 || cbOY->ItemIndex == -1))) {
		ShowMessage("Zosta³y nie uzupe³nione pola!!");
		return;
	}
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFChartPropertis::FormShow(TObject *Sender)
{
	cbAnimateChartClick(NULL);
	rgChartTypeClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFChartPropertis::pLineColorClick(TObject *Sender)
{
	ColorDialog1->Color = pLineColor->Color;
	if (ColorDialog1->Execute()) pLineColor->Color = ColorDialog1->Color;
}
//---------------------------------------------------------------------------

