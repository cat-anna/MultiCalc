//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef FncFormH
#define FncFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Calculator.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFFncEdt : public TForm
{
__published:	// IDE-managed Components
	TButton *bModify;
	TListBox *lbFncs;
	TGroupBox *GroupBox1;
	TLabeledEdit *leName;
	TGroupBox *GroupBox2;
	TRadioButton *rbwew;
	TRadioButton *rbwers;
	TButton *bUsun;
	TButton *Button2;
	TGroupBox *GroupBox3;
	TLabel *Label1;
	TLabeledEdit *leData;
	TLabel *lParCount;
	void __fastcall lbFncsDrawItem(TWinControl *Control, int Index, TRect &Rect,
          TOwnerDrawState State);
	void __fastcall lbFncsClick(TObject *Sender);
	void __fastcall rbwersClick(TObject *Sender);
	void __fastcall rbwewClick(TObject *Sender);
	void __fastcall bModifyClick(TObject *Sender);
	void __fastcall bUsunClick(TObject *Sender);
	void __fastcall leDataChange(TObject *Sender);
private:
	TFunctionsList *List;
	int AParCount;

	void RefreshList();
	bool Check();
public:		// User declarations
	__fastcall TFFncEdt(TComponent* Owner);

	void LoadList(TFunctionsList* NewList);
};
//---------------------------------------------------------------------------
extern PACKAGE TFFncEdt *FFncEdt;
//---------------------------------------------------------------------------
#endif
