//$$---- Form HDR ----
//---------------------------------------------------------------------------
#ifndef ChartWindowH
#define ChartWindowH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Calculator.h"
#include <ExtCtrls.hpp>
#include "glWindow.hpp"
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TTabData
{
public:
	String Name, Expression, AdditionalData;
	Graphics::TBitmap *LastDraw;
	TCalcResult *CalcData;
	TChartInfo ChartInfo;
	float Transform, rX, rY;
	bool Paused; 
	int TimeFlow, TimeAdd, TimeStep;

	void Set(String aXv, String aYv, bool aHaveParam, String aPv, int aTimeStep,
		TChartMode aMode);
	TTabData(TCalcResult *Result);
	~TTabData();
};

class TFChartWindow : public TForm
{
__published:	// IDE-managed Components
	TTimer *Timer1;
	TPopupMenu *PopupMenu1;
	TMenuItem *Zapisz1;
	TMenuItem *N1;
	TMenuItem *Opjewykresu1;
	TMenuItem *Zapiszdanewykresu1;
	TSaveDialog *SaveDialog;
	TTabControl *TabControl;
	TImage *iDraw;
	TglWindow *glWindow;
	TMenuItem *Reset1;
	TMenuItem *Zamknij1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *Pauza1;
	TMenuItem *Zaptlczas1;
	TMenuItem *Wasnocifunkcji1;
	TStatusBar *Status;
	TMenuItem *EksportujdoplikuExcel1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall glWindowDraw(TObject *Sender);
	void __fastcall glWindowInit(TObject *Sender);
	void __fastcall glWindowMouseMove(TObject *Sender, TShiftState Shift, int X,
		  int Y);
	void __fastcall glWindowMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall glWindowMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Zapisz1Click(TObject *Sender);
	void __fastcall Zapiszdanewykresu1Click(TObject *Sender);
	void __fastcall Opjewykresu1Click(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall i2DChartMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall i2DChartMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall i2DChartMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall TabControlChange(TObject *Sender);
	void __fastcall PopupMenu1Popup(TObject *Sender);
	void __fastcall Zamknij1Click(TObject *Sender);
	void __fastcall Reset1Click(TObject *Sender);
	void __fastcall Pauza1Click(TObject *Sender);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Zaptlczas1Click(TObject *Sender);
	void __fastcall Wasnocifunkcji1Click(TObject *Sender);
	void __fastcall EksportujdoplikuExcel1Click(TObject *Sender);
private:
	float dx, dy, trans;
	int mx, my;

	bool MouseDown;
	TPoint LastMousePos, DrawOffset;
	TList *Tabs;
    TTabData* ActiveTab;

	void D2Redraw(bool ReCalc = false);
//	void D3Recalc();
	TTabData* GetTab(int Index);
	void RefreshTabs(bool SelectLast = false);
	TTabData* AddTab(TCalcResult* NewResult);
public:
	__fastcall TFChartWindow(TComponent* Owner);
	TChart* OwnChart;

	//proc ustawiania wykresów
	void Add2DChart(TChart *Chart, String vX, String vP,
		bool HaveParam,	double TimeStep, bool RefActive = false);
	void Add3DChart(TChart *Chart, String Xvar, String Yvar,
		bool nHaveParam, float nTimeStep, String Pvar, bool RefActive = false);

	//inne opcje
	void SetSize(int H, int W);
};
//---------------------------------------------------------------------------
extern PACKAGE TFChartWindow *FChartWindow;
//---------------------------------------------------------------------------
#endif
