//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef ChartPropertisH
#define ChartPropertisH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FloatSpinEdit.hpp"
#include <ExtCtrls.hpp>
#include "cspin.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFChartPropertis : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox2;
	TLabel *Label6;
	TLabel *LabelOY;
	TLabel *lParamName;
	TLabel *lTimeStep;
	TRadioGroup *rgChartType;
	TComboBox *cbOX;
	TComboBox *cbOY;
	TCheckBox *cbAnimateChart;
	TButton *Button1;
	TButton *Button2;
	TCheckBox *cbShowNet;
	TCSpinEdit *fseTimeStep;
	TLabel *lParam;
	TPanel *pLineColor;
	TColorDialog *ColorDialog1;
	void __fastcall rgChartTypeClick(TObject *Sender);
	void __fastcall cbAnimateChartClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall pLineColorClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFChartPropertis(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFChartPropertis *FChartPropertis;
//---------------------------------------------------------------------------
#endif
