//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Calculator.h"
#include <IniFiles.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "FloatSpinEdit.hpp"
#include "cspin.h"
#include "ChartWindow.h"
#include <Dialogs.hpp>
#include "perfgrap.h"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFMainForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TMemo *mExpression;
	TLabeledEdit *leCalcResult;
	TLabeledEdit *leChartExpression;
	TGroupBox *gbVaribles;
	TListBox *lbVaribles;
	TCSpinButton *CSpinButton1;
	TButton *cDraw;
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	TTabSheet *TabSheet3;
	TGroupBox *GroupBox4;
	TButton *Button1;
	TTabSheet *TabSheet4;
	TGroupBox *GroupBox1;
	TMemo *Memo1;
	TButton *Button2;
	TEdit *eTest;
	TButton *bTest;
	TEdit *eOutTest;
	TTabSheet *TabSheet5;
	TMemo *Memo2;
	TRadioGroup *RadioGroup1;
	TButton *Button3;
	TEdit *Edit1;
	TButton *Button4;
	TRadioGroup *rgChartType;
	TCheckBox *cbDrawChartAdd;
	TComboBox *cbChartWindow;
	TCheckBox *cbHaveRange;
	TLabel *Label6;
	TLabel *LabelOY;
	TComboBox *cbOX;
	TComboBox *cbOY;
	TGroupBox *GroupBox2;
	TLabel *Label3;
	TLabel *Label4;
	TFloatSpinEdit *lseVarFrom;
	TFloatSpinEdit *lseVarTo;
	TLabel *lParamName;
	TLabel *lTimeStep;
	TLabel *lParam;
	TCheckBox *cbAnimateChart;
	TCSpinEdit *fseTimeStep;
	TMainMenu *MainMenu1;
	TStatusBar *StatusBar;
	TLabel *Label2;
	TButton *Button5;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall rgChartTypeClick(TObject *Sender);
	void __fastcall cbDrawChartAddClick(TObject *Sender);
	void __fastcall cbAnimateChartClick(TObject *Sender);
	void __fastcall leChartExpressionChange(TObject *Sender);
	void __fastcall lbVariblesClick(TObject *Sender);
	void __fastcall lseVarFromChange(TObject *Sender);
	void __fastcall CSpinButton1DownClick(TObject *Sender);
	void __fastcall CSpinButton1UpClick(TObject *Sender);
	void __fastcall cDrawClick(TObject *Sender);
	void __fastcall mExpressionChange(TObject *Sender);
	void __fastcall cbChartWindowDropDown(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall fseTimeStepChange(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall bTestClick(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall cbOYDropDown(TObject *Sender);
	void __fastcall cbHaveRangeClick(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
private:
	bool Modiffied;

	TFChartWindow* CreateChartWindow(TChart* NewChart);
public:		// User declarations
	__fastcall TFMainForm(TComponent* Owner);
	TControllClass *ControllClass;
	TChart *Chart;
	TCalcResult *CalcResult;

	TList* WindowsList;

	bool DebugMode;

	void ChartWindowClose(TFChartWindow* Window);
}; 
#define IniFileName ChangeFileExt(Application->ExeName, ".ini")
//---------------------------------------------------------------------------
extern PACKAGE TFMainForm *FMainForm;
//---------------------------------------------------------------------------
#endif
