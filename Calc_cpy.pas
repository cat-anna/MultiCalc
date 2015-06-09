unit Calc;

interface
{
 ((6,6-3-3/14)*(5+5/6))/((21-0,25)/2,5)
}
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, OpenGL;

type
  TCharType = (ctNumber, ctOperator, ctMinus, ctExpression, ctUnDefined, ctFunction, ctOpenN, ctCloseN, ctComma, ctVarible, ctEqual);
  TCharSet = set of TCharType;
  TExpressionType = (etUnknown, etExpression, etFunction, etEquation);
  TOperator = array[1..2] of char;

  TFunction = Function(Param: array of double): double;
  TProgressProc = Procedure(Prg: Byte);

  TFunctonListItemType = (flitExpression, flitFunction, flitLuaFuction);
  TParam = array of Double;
  
  TTranformationType = (ttUnknown, ttEquation, ttExpression);

  PFunctionList = ^TFunctionList;
  PExpression = ^TExpression;
  PFunctionListItem = ^TFunctionListItem;
  PList = ^TList;
  PVaribles = ^TVaribles;

  TVarible = record
    vFrom, vTo, Delta, Varible: Double;
    Active, Busy: bool;
  end;
  TVaribles = array['A'..'Z']of TVarible;    

  TSResult = record
    Varibles: TVaribles;
    Result: double;
  end;

  TFunctionListItem = class
    PList: PFunctionList;
    Fnc: TFunction;
    Expression, identifier, Script: string;
    parCount: integer;
    Typ: TFunctonListItemType;
    function Calculate(params: array of double; prec: integer): double;
  end;           

  TFunctionList = class
  protected
    FList: Tlist;
    procedure AddStandard;
    function GetCount: integer;
  public
    PExpression: PExpression;
    procedure AddFunction(Fnc: Pointer; identifier: string; parCount:integer); overload;
    procedure AddFunction(Expression, identifier: string; parCount:integer); overload;
    procedure AddLuaFunction(Id, Script: string; parCount:integer);
    function DoFunction(identifier: string; params: array of double; prec: integer):Double;
    function GetItem(index: integer): TFunctionListItem;
    procedure Delete(index: integer);
    procedure Modify(index: integer; id, ex, script: string; pc: integer);
    procedure AddFromText(Text: string);
    function SaveToText: string;
    procedure Clear;
    constructor Create;
    destructor Destroy; override;
    property Count: integer read GetCount;
  end;

  TWord = class
  protected
    Children: TList;
    procedure Clear; virtual;
    function GetExpression(List: TList): string; overload;
    function DoExpression(Expression: string; l1, l2: double): string;
  public
    Positive: bool;
    SelfExpression: string;
    SelfTyp: TCharType;
    constructor Create; virtual;
    destructor Destroy; override;
    function GetExpression: string; overload; virtual;     
  end;

  TTrWord = class(TWord)
  protected
    procedure ClearList(List: TList);
    procedure Clear; override;
    procedure Add(Expression: string; Typ: TCharType; pos: bool; List: TList);
  public
    procedure SetList(Expr: string; List: TList); //virtual;
    function Transform: string; virtual;
    constructor Create; override;
    destructor Destroy; override;
  end;

  TTransformation = class(TTrWord)
  protected
    RigthList, LeftList: TList;

    procedure SimplifyList(List: TList);
    function Multiply(Factor1, Factor2: string): string;
    function CutSimilar(List: TList): bool;
    function Calculate(List: TList): bool;
    procedure SolveEquation(LL, RL: TList);
  public
    SelfTyp: TTranformationType;

    procedure Clear; override;
    procedure InsertWords(Txt: string; Lst: TList; Index: integer);
    procedure Prepare(Expr: string);
    function Transform: string; override;
    function GetExpression: string; override;
    constructor Create; override;
    destructor Destroy; override;
  end;

  TExWord = class(TWord)
  protected
    FncId: string;
    param: array of string;
    PFncList: PFunctionList;
    PVaribles: PVaribles;

    procedure Add(Expression: string; Typ: TCharType; Pos: bool; FList: PFunctionList; PVar: PVaribles);
    function CalculateParam(par: array of string): TParam;
    function PrecisionFormat(Value: Double): string;
  public
    Precision: integer;

    function Calculate: double; virtual;
    procedure SetSelf(Expr: string); virtual;
    constructor Create; override;
    destructor Destroy; override;
    procedure SetIndicators(PL: PFunctionList; PV: PVaribles);
    procedure SetPrecision(cPrec: integer);
  end;

  TExpression = class(TExWord)
  protected
    Arranged, Simulating, Simulated: bool;
    ExpressionTemp: string;
    Charts, LastResultOperation: byte;

    function FindNextChar(var c: char; ic: char): bool;
    function TypePresent(Typ: TCharType; Txt: string): bool;
    procedure ClearVaribles(ClearResult: bool);
    procedure FillVaribleList(vF, vT, D, V: Double; Act, Bs: bool);
    procedure SimulateVarible(txt: string; VaribleQueue: TStringList);
    function GetResultInfo: string;
    Procedure AddResult(f: double);
    procedure AddSummary(Txt: string);
    procedure ClearSummary;
  public
    ExpressionTyp: TExpressionType;
    FunctionList: TFunctionList;
    Varibles: TVaribles;
    SimulatingResult: array of TSResult;
    Summary: TStrings;
    Transform: TTransformation;

    procedure GetVaribles(Txt: string);
    procedure GetVaribesList(list: TStrings);
    procedure GetVarible(var vFrom, vTo, Delta: double; v: string);
    procedure SetVarible(vFrom, vTo, Delta: double; v: string; var CalcCount: LongWord);
    procedure Render3DChartScene(Xv, Yv: char);
    procedure ClearChart(Bitmap: TBitmap; Width, Height, ScaleX, ScaleY: integer);
    procedure DrawChart(Bitmap: TBitmap; Width, Height, ScaleX, ScaleY: integer; Xv: char; DrawAdd: bool);
    procedure SetSelf(Expr: string); override;
    function Calculate: double; override;
    procedure Simulate(VaribleQueue: TStrings; PrgPros: TProgressProc);
    procedure SaveResultToFile(FileName: string);
    procedure LoadResultFromFile(FileName: string);
    property ResultInfo: string read GetResultInfo;
    procedure Clear; override;
    constructor Create; override;
    destructor Destroy; override;
  end;

function FindOperator(Op: TOperator; List: Tlist): integer;
function TypZnaku(zn: char): TCharType;
function FindType(Typ: TCharType; Txt: string): integer; overload;
function FindType(Typ: TCharType; List: TList): integer; overload;

const
  OP_PotegPierw: TOperator = ('^', '#');
  OP_MnozDziel: TOperator = ('*', '/');
  OP_DodOdejm: TOperator = ('+', '-');

  OP_Kolejnosc: array[1..3] of TOperator =
     (('^', '#'), ('*', '/'), ('+', '-'));

  Lst_Txt_Separator = '|';
  Res_Id = 'sfr';

  Color_List: array[1..10]of TColor = (clRed, clBlue, clGreen, clMaroon, clPurple, clTeal, clAqua, clOlive, clLime, clFuchsia);

implementation

uses math, FMainForm, Unit1, LuaBind;

{   Wolne procedury   }

procedure CutBracket(var Txt: string);
var l, j: integer;
begin
  j := 0;
  l := 0;
  while j < length(Txt)do
  begin
    if TypZnaku(Txt[j]) = ctOpenN then Inc(l);
    if TypZnaku(Txt[j]) = ctCloseN then dec(l);
    if (l = 0)and(j > 1)and(j < length(Txt)) then break;
    inc(j);
  end;
  if length(Txt) > 1 then
    if(j = length(Txt))and(TypZnaku(Txt[1]) = ctOpenN)then Txt := copy(Txt, 2, length(Txt) - 2);
  if length(Txt) = 0 then Txt := '0';
end;

function CharTypeToStr(ct: TCharType): string;
begin
  case ct of
    ctNumber: Result := 'number';
    ctOperator: Result := 'operator';
    ctMinus: Result := 'minus';
    ctExpression: Result := 'expression';
    ctUnDefined: Result := 'un-def';
    ctFunction: Result := 'function';
    ctOpenN: Result := 'openn';
    ctCloseN: Result := 'closen';
    ctComma: Result := 'comma';
    ctVarible: Result := 'varible';
    ctEqual: Result := 'equal';
  end;
end;

function TypZnaku(zn: char): TCharType;
begin
  case ord(zn) of
    ord('*'):Result := ctOperator;
    ord('/'):Result := ctOperator;
    ord('+'):Result := ctOperator;
    ord('-'):Result := ctMinus;
    ord('^'):Result := ctOperator;
    ord('('):Result := ctOpenN;
    ord(')'):Result := ctCloseN;
    ord('0')..ord('9'):Result := ctNumber;
    ord('A')..ord('Z'):Result := ctVarible;
    ord('a')..ord('z'):Result := ctfunction;
    ord(','):Result := ctComma;
    ord('.'):Result := ctComma;
    ord('='):Result := ctEqual;
    else Result := ctUnDefined;
  end;
end;

function FindOperator(Op: TOperator; List: Tlist): integer;
var i, j: integer;
    Found: bool;
begin
  Result := -1;
  Found := false;
  i := 0;
  while(i < List.Count)and(not Found)do
  begin
    j := 0;
    while(j < 2)and(not Found)do
    begin
      inc(j);
      if(not found)and(TWord(List[i]).SelfExpression = Op[j])then
      begin
        Result := i;
        Found := true;
      end;
    end;
    inc(i);
  end;
end;

function FindType(Typ: TCharType; Txt: string): integer;
var i: integer;
    Found: bool;
begin
  i := 1;
  Found := false;
  while(i < Length(Txt) + 1)and(not Found)do
  begin
    if TypZnaku(Txt[i]) = Typ then Found := true;
    inc(i);
  end;
  Result := i - 1;
  if not Found then result := -1;
end;

function FindType(Typ: TCharType; List: TList): integer;
var i: integer;
    Found: bool;
begin
  i := 1;
  Found := false;
  while(i < list.Count)and(not Found)do
  begin
    if TWord(List[i]).SelfTyp = Typ then Found := true;
    inc(i);
  end;
  Result := i - 1;
  if not Found then result := -1;
end;

{   TExpression   }

function TExpression.Calculate: double;
begin
  Result := 0;
  if not Arranged then exit;
  case ExpressionTyp of
    etExpression:begin
                   result := inherited Calculate;
                   Arranged := false;
                 end;
    etFunction:;
    etEquation:;
  end;
end;

procedure TExpression.Render3DChartScene(Xv: Char; Yv: Char);
var cX, cY: char;
    i: integer;
begin
  inc(Charts);
  if Xv in ['A'..'Z'] then cX := Xv else cX := 'X';
  if Yv in ['A'..'Z'] then cY := Yv else cY := 'Y';

  glBegin(GL_LINE_STRIP);   
  glColor3f(0.2, 0.5, 1.0);
  for i := 0 to length(SimulatingResult) - 1 do
  begin
    glVertex3f(SimulatingResult[i].Varibles[cX].Varible,
               SimulatingResult[i].Result,
               SimulatingResult[i].Varibles[cY].Varible);
  end;
  glEnd();
end;

procedure TExpression.ClearChart(Bitmap: TBitmap; Width, Height, ScaleX, ScaleY: integer);
var cX, cY, i, mX, mY, j: integer;
begin
  if Bitmap = nil then exit;
  if(Width < 1)or(Height < 1)then
  begin
    Bitmap.Height := 100;
    Bitmap.Width := 100;
    Bitmap.Canvas.TextOut(10, 10, 'Z³y rozmiar bitmapy');
    exit;
  end;

  Bitmap.Height := Height;
  Bitmap.Width := Width;

  cX := Width div 2;
  cY := Height div 2;
  mX := cX mod ScaleX;
  mY := cY mod ScaleY;

  with Bitmap.Canvas do
  begin
    Pen.Width := 1;
    Brush.Color := clwhite;
    Brush.Style := bsSolid;
    Rectangle(-1, -1, Width + 1, Height + 1);
    Brush.Style := bsClear;

    Pen.Color := $E0E0E0;
    for i := 0 to cX div ScaleX do
    if i mod 5 = 0 then
      begin
        MoveTo(0, i * ScaleX + cX);
        LineTo(width, i * ScaleX + cX);
      end;
    for i := cX div ScaleX - 1 downto 0 do
    if i mod 5 = 0 then
      begin
        MoveTo(0, i * ScaleX + mX);
        LineTo(width, i * ScaleX + mX);
      end;

    for i := 0 to cY div ScaleY do
    if i mod 5 = 0 then
      begin
        MoveTo(i * ScaleY + cY, 0);
        LineTo(i * ScaleY + cY, Height);
      end;
    for i := cY div ScaleY downto 0 do
    if i mod 5 = 0 then
      begin
        MoveTo(i * ScaleY + mY, 0);
        LineTo(i * ScaleY + mY, Height);
      end;

    Font.Color := clGray;
    for i := 0 to cX div ScaleX do if (i+1) mod 5 = 0 then TextOut(cY - TextWidth(inttostr(-i)) - 5, i * ScaleX + cX + TextHeight(inttostr(i))div 3, '-'+inttostr(i + 1));
    j := 0;
    for i := cX div ScaleX - 1 downto 0 do
    if i mod 5 = 0 then
      begin
        inc(j, 5);
        TextOut(cY - TextWidth(inttostr(j)) - 5, (i - 1) * ScaleX - mX + TextWidth(inttostr(-i)) div 3, inttostr(j));
      end;

    for i := 0 to cY div ScaleY do if (i+1) mod 5 = 0 then TextOut((i + 1) * ScaleX + cX - TextWidth(inttostr(i))div 2, cY + 5, inttostr(i + 1));
    j := 0;
    for i := cY div ScaleY - 1 downto 0 do
    if i mod 5 = 0 then
      begin
        inc(j, 5);
        TextOut(i * ScaleX - 2 - TextWidth(inttostr(-j))div 2, cY + 5, inttostr(-j));
      end;

    Pen.Color := clBlack;
    MoveTo(cX, 0);
    LineTo(cX, Height);
    MoveTo(0, cY);
    LineTo(Width, cY);

    MoveTo(cX - 4, 8);
    LineTo(cX - 1, -1);
    MoveTo(cX + 4, 8);
    LineTo(cX + 1, -1);

    MoveTo(Width - 8, cY - 4);
    LineTo(Width + 1, cY);
    MoveTo(Width - 8, cY + 4);
    LineTo(Width + 1, cY);
  end;
  Charts := 0;
end;

procedure TExpression.DrawChart(Bitmap: TBitmap; Width, Height, ScaleX, ScaleY: integer; Xv: char; DrawAdd: bool);
var cX, cY, i: integer;
    c: char;
begin
  if Bitmap = nil then exit;
  if Simulating then exit;
  if not Simulated then
  begin
    Bitmap.Height := 100;
    Bitmap.Width := 100;
    Bitmap.Canvas.Brush.Style := bsSolid;
    Bitmap.Canvas.Rectangle(-1, -1, 101, 101);
    Bitmap.Canvas.TextOut(5, 10, 'Nie mam wartoœæi');
    exit;
  end;

  if(Width < 1)or(Height < 1) then
  begin
    Bitmap.Height := 100;
    Bitmap.Width := 100;
    Bitmap.Canvas.TextOut(10, 10, 'Z³y rozmiar bitmapy');
    exit;
  end;

  cX := Width div 2;
  cY := Height div 2;

  if not DrawAdd then ClearChart(Bitmap, Width, Height, ScaleX, ScaleY);
  inc(Charts);
  if Xv in ['A'..'Z'] then c := Xv else c := 'X';

  with Bitmap.Canvas do
  begin
    Font.Color := clBlack;

    TextOut(cX + 7, -2, 'y');
    TextOut(Width - 8, cY + 2, 'x');

    pen.Color := Color_List[Charts];
    Font.Color := Color_List[Charts];
    TextOut(3, (Charts - 1) * TextHeight(SelfExpression) + 1, 'y(' + c + ')=' + SelfExpression);

    MoveTo(Round(SimulatingResult[0].Varibles[c].Varible * ScaleX + cX),
           Round(-SimulatingResult[0].Result * ScaleY + cY));

    for i := 1 to length(SimulatingResult) - 1 do
      LineTo(Round(SimulatingResult[i].Varibles[c].Varible * ScaleX + cX),
             Round(-SimulatingResult[i].Result * ScaleY + cY));
  end;
end;

procedure TExpression.SetSelf(Expr: string);
begin
  Clear;

  inherited SetSelf(Expr);

  if TypePresent(ctEqual, Expr) then ExpressionTyp := etEquation
  else if TypePresent(ctVarible, Expr) then ExpressionTyp := etFunction
  else ExpressionTyp := etExpression;

  Arranged := true;
end;

procedure TExpression.Simulate(VaribleQueue: TStrings; PrgPros: TProgressProc);
var Progress, TempEffect, vTo, Delta: double;
    v: char;
    TxtTmp: string;
    Queue: TStringList;
    label Koniec;
begin
  LastResultOperation := 0;
  ClearSummary;
  Summary.BeginUpdate;
  if ExpressionTyp <> etFunction then
  begin
    AddSummary('Niemo¿na symulowaæ tego wyra¿enia');
    exit;
  end;

  SetLength(SimulatingResult, 0);
  ExpressionTemp := SelfExpression;
  Queue := TStringList.Create;
  Queue.AddStrings(VaribleQueue);

  v := VaribleQueue[0][1];
  Queue.Delete(0);

  Progress := varibles[v].vFrom;
  vTo := varibles[v].vTo;
  Delta := varibles[v].Delta;

  if(Delta = 0)or(vTo <= Progress)then
  begin
    AddSummary('Nie mogêsymulowaæ z tymi wartoœciami');
    goto Koniec;
  end;
  AddSummary('Symuljê: ' + SelfExpression);
  Simulating := true;    

  while progress <= vTo do
  begin
    varibles[v].Varible := Progress;

    TxtTmp := 'Dla ' + v + '=' + PrecisionFormat(Progress);

    if Queue.Count > 0 then SimulateVarible(TxtTmp, Queue)
    else
    begin
      if not Arranged then
      begin
        Clear;
        SetSelf(ExpressionTemp);
      end;
      ExpressionTyp := etExpression;
      TempEffect := Calculate;
      AddResult(TempEffect);
      AddSummary(TxtTmp + ', wynik=' + PrecisionFormat(TempEffect));
    end;
    Progress := Progress + Delta;
    PrgPros(Round((Progress - varibles[v].vFrom)/(varibles[v].vTo - varibles[v].vFrom) * 100));
  end;

  AddSummary('Symulowanie zakoñczone powodzniem');

  Koniec:
  LastResultOperation := 2;
  Simulating := false;
  Simulated := true;
  Queue.Free;
  Summary.EndUpdate;
end;

procedure TExpression.SimulateVarible(txt: string; VaribleQueue: TStringList);
var Progress, TempEffect, vTo, Delta: double;
    v: char;
    TxtTmp: string;
    Queue: TStringList;
begin
  Queue := TStringList.Create;
  Queue.Assign(VaribleQueue);
  v := Queue[0][1];
  Queue.Delete(0);
  Progress := varibles[v].vFrom;
  vTo := varibles[v].vTo;
  Delta := varibles[v].Delta;
  TxtTmp := txt;
  if Delta <= 0 then exit;

  while progress <= vTo do
  begin
    varibles[v].Varible := Progress;

    TxtTmp := txt + ', ' + v + '=' + FloatToStr(Progress);

    if Queue.Count > 0 then SimulateVarible(TxtTmp, Queue) 
    else
    begin
      if not Arranged then
      begin
        Clear;
        SetSelf(ExpressionTemp);
      end;
      ExpressionTyp := etExpression;
      TempEffect := Calculate;
      AddResult(TempEffect);
      AddSummary(TxtTmp + ', wynik=' + PrecisionFormat(TempEffect));
    end;
    Progress := Progress + Delta;
  end;
  Queue.Free;
end;

procedure TExpression.AddSummary(Txt: string);
begin
  if Summary <> nil then Summary.Add(Txt);
end;

procedure TExpression.ClearSummary;
begin
  if Summary <> nil then Summary.Clear;
end;

procedure TExpression.AddResult(f: double);
var l: integer;
begin
  l := length(SimulatingResult) + 1;
  SetLength(SimulatingResult, l);
  SimulatingResult[l - 1].Varibles := Varibles;
  SimulatingResult[l - 1].Result := f;
end;

function TExpression.GetResultInfo: string;  
begin
  case LastResultOperation of
    0:Result := 'B³¹d!';
    1:Result := 'Brak wartoœci';
    2:Result := 'Wyliczono ' + IntToStr(length(SimulatingResult)) + ' wartoœci';
    3:Result := 'Wczytano ' + IntToStr(length(SimulatingResult)) + ' wartoœci';
    4:Result := 'Zapisano ' + IntToStr(length(SimulatingResult)) + ' wartoœci';
    else Result := 'B³¹d!';
  end;
end;

function TExpression.FindNextChar(var c: char; ic: char): bool;
var i: integer;
begin
  result := false;
  i := ord(ic) + 1;
  while(not Varibles[chr(i)].Active)and(i < ord('Z') + 1)and(not Varibles[chr(i)].Busy) do inc(i);
  if Varibles[chr(i)].Active then
  if chr(i) in ['A'..'Z'] then
  begin
    Result := true;
    c := chr(i);
    Varibles[chr(i)].Busy := true;
  end;
end;

function TExpression.TypePresent(Typ: TCharType; Txt: string): bool;
var i: integer;
begin
  i := 1;
  Result := false;
  while(i < Length(Txt) + 1)and(not Result)do
  begin
    if TypZnaku(Txt[i]) = Typ then Result := true;
    inc(i);
  end;
end;

procedure TExpression.Clear;
begin
  inherited Clear;
  ExpressionTyp := etUnknown;
  Arranged := false;
end;

procedure TExpression.ClearVaribles(ClearResult: bool);
var i: byte;
begin
  for i := ord('A') to ord('Z') do
     with Varibles[chr(i)]do
     begin
       Varible := 0;
       Active := false;
       Busy := false;
     end;
  if ClearResult then
  begin
    SetLength(SimulatingResult, 0);
    Simulated := false;
    LastResultOperation := 0;
  end;
end;

procedure TExpression.FillVaribleList(vF, vT, D, V: Double; Act, Bs: bool);
var i: byte;
begin
  for i := ord('A') to ord('Z') do
     with Varibles[chr(i)]do
     begin
       Varible := V;
       Active := Act;
       Busy := Bs;
       vFrom := vF;
       vTo := vT;
       Delta := D;
     end;
end;

procedure TExpression.GetVaribles(Txt: string);
var i: integer;
begin
  ClearVaribles(false);
  for i := 1 to length(Txt) do
  begin
    if TypZnaku(Txt[i]) = ctVarible then Varibles[Txt[i]].Active := true;
  end;
end;

procedure TExpression.GetVaribesList(list: TStrings);
var i: byte;
begin
  if list = nil then exit;
  list.Clear;

  for i := ord('A') to ord('Z') do
  begin
    if Varibles[chr(i)].Active then list.Add(chr(i));
  end;
end;

procedure TExpression.GetVarible(var vFrom, vTo, Delta: double; v: string);
begin
  vFrom := Varibles[v[1]].vFrom;
  vTo := Varibles[v[1]].vTo;
  Delta := Varibles[v[1]].Delta;
end;

procedure TExpression.SetVarible(vFrom, vTo, Delta: double; v: string; var CalcCount: LongWord);
var c: integer;
    tc, dt, rt: double;
begin
  if not Varibles[v[1]].Active then exit;

  Varibles[v[1]].vFrom := vFrom;
  Varibles[v[1]].vTo := vTo;
  Varibles[v[1]].Delta := Delta;

  tc := 1;
  for c := ord('A') to ord('Z') do
   if Varibles[chr(c)].Active then
   begin
     dt := Varibles[chr(c)].Delta;
     if dt <= 0 then dt := 1;
     rt := abs(Varibles[chr(c)].vFrom - Varibles[chr(c)].vTo) / dt + 1;
     if rt = 0 then rt := 1;
     tc := tc * rt;
   end;
  CalcCount := Round(tc);
end;

procedure TExpression.LoadResultFromFile(FileName: string);
var FStr: TFileStream;
    i, l, j: integer;
    c: char;
    f: double;
    temp: array[0..255]of char;
    tVar: TVaribles;
    v: byte;
    Txt: string;
begin
  Clear;
  LastResultOperation := 0;
  ClearSummary;

  FStr := TFileStream.Create(FileName, fmOpenRead);

  FStr.Read(temp, sizeof(Res_Id));

  if copy(temp, 0, sizeof(Res_Id)) <> Res_Id then exit;

  FStr.Read(l, sizeof(l));
  FStr.Read(temp, l);
  SelfExpression := copy(temp, 0, l);
  FStr.Read(v, sizeof(v));

  for c := 'A' to 'Z' do
  begin
    FStr.Read(f, sizeof(f));
    tVar[c].vFrom := f;
    FStr.Read(f, sizeof(f));
    tVar[c].vTo := f;
    FStr.Read(f, sizeof(f));
    tVar[c].Delta := f;
    tVar[c].Active := false;
    tVar[c].Busy := false;
  end;

  FStr.Read(l, sizeof(l));

  for i := 0 to l - 1 do
  begin
    SetLength(SimulatingResult, i + 1);
    FStr.Read(f, sizeof(f));
    SimulatingResult[i].Result := f;
    Txt := 'Dla ';

    for j := 1 to v do
    begin
      FStr.Read(c, sizeof(c));
      FStr.Read(f, sizeof(f));
      tVar[c].Varible := f;
      tVar[c].Active := true;
      Txt := Txt + c + '=' + PrecisionFormat(f) + ', ';
    end;
    SimulatingResult[i].Varibles := tVar;
    Txt := Txt + 'wynik=' + PrecisionFormat(SimulatingResult[i].Result);
    AddSummary(Txt);
  end;
  Varibles := tVar;
  FStr.Free;
  Simulated := true;
  LastResultOperation := 3;
end;

procedure TExpression.SaveResultToFile(FileName: string);
var FStr: TFileStream;
    i, l: integer;
    c: char;
    f: double;
    v: byte;
begin
  if Length(SimulatingResult) = 0 then exit;
  LastResultOperation := 0;

  FStr := TFileStream.Create(FileName, fmCreate);
  FStr.Write(Pchar(Res_Id)^, sizeof(Res_Id));

  l := length(SelfExpression);
  FStr.Write(l, sizeof(l));
  FStr.Write(Pchar(SelfExpression)^, l);

  v := 0;
  c := chr(ord('A') - 1);
  while FindNextChar(c, c) do inc(v);

  FStr.Write(v, sizeof(v));

  for c := 'A' to 'Z' do
  begin
    f := SimulatingResult[1].Varibles[c].vFrom;
    FStr.Write(f, sizeof(f));
    f := SimulatingResult[1].Varibles[c].vTo;
    FStr.Write(f, sizeof(f));
    f := SimulatingResult[1].Varibles[c].Delta;
    FStr.Write(f, sizeof(f));
  end;

  l := length(SimulatingResult);
  FStr.Write(l, sizeof(l));

  for i := 0 to length(SimulatingResult) - 1 do
  begin
    f := SimulatingResult[i].Result;
    FStr.Write(f, sizeof(f));
    for c := 'A' to 'Z' do
      if SimulatingResult[i].Varibles[c].Active then
      begin
        FStr.Write(c, sizeof(c));
        f := SimulatingResult[i].Varibles[c].Varible;
        FStr.Write(f, sizeof(f));
      end;
  end;

  FStr.Free;
  LastResultOperation := 4; 
end;

constructor TExpression.Create;
begin
  inherited Create;

  ClearVaribles(true);
  FillVaribleList(-5, 5, 0.5, 0, false, false);

  FunctionList := TFunctionList.Create;
  Transform := TTransformation.Create;

  FunctionList.PExpression := @Self;
  PFncList := @FunctionList;
  PVaribles := @Varibles;

  LastResultOperation := 1;
  Simulating := false;
  Simulated := false;
  Arranged := false;
  Charts := 0;
end;

destructor TExpression.Destroy;
begin
  FunctionList.Free;
  Transform.Free;
  inherited Destroy;
end;

{   TWord   }

function TWord.GetExpression: string;
begin
  if SelfTyp = ctExpression then Result := GetExpression(Children)
  else
    if Positive then Result := SelfExpression
      else Result := '-' + SelfExpression;
end;

function TWord.GetExpression(List: TList): string;
var i: integer;
begin
  result := '';
  for i := 0 to List.Count - 1 do
  begin
    case TWord(List[i]).SelfTyp of
//      ctNumber:if TWord(List[i]).Positive then Result := Result + TWord(List[i]).SelfExpression else Result := Result +  '(-' + TWord(List[i]).SelfExpression + ')';
      ctOperator:if TWord(List[i]).SelfExpression[1] = '+' then
                 begin
                   if(TWord(List[i + 1]).Positive) then Result := Result + '+'
                     else Result := Result + '-'
                 end
                 else Result := Result + TWord(List[i]).SelfExpression;
//      ctMinus: ;
//      ctExpression:Result := Result + '(' + TWord(List[i]).GetExpression +')';
//      ctUnDefined: ;
//      ctFunction: ;
//      ctVarible: if TWord(List[i]).Positive then Result := Result + TWord(List[i]).SelfExpression else Result := Result + '(-' + TWord(List[i]).SelfExpression + ')';
      else if Positive then Result := Result + TWord(List[i]).SelfExpression
             else Result := Result + '(-' + TWord(List[i]).SelfExpression + ')';
    end;
  end;
end;

function TWord.DoExpression(Expression: string; l1, l2: double): string;
var temp: double;
begin
  temp := 0;
  case ord(Expression[1]) of
    ord('*'):temp := l1 * l2;
    ord('/'):if l2 = 0 then temp := 0 else temp := l1 / l2;
    ord('+'):temp := l1 + l2;
    ord('-'):temp := l1 - l2;
    ord('^'):if l1 = 0 then temp := 0 else temp := Power(l1, l2);
  end;
  Result := FloatToStr(temp);
end;

procedure TWord.Clear;
var i: integer;
begin
  for i := 0 to Children.Count - 1 do TWord(Children[i]).Free;
  Children.Clear;
  positive := true;
  SelfExpression := '';
  SelfTyp := ctUnDefined;
end;

constructor TWord.Create;
begin
  Children := TList.Create;
end;

destructor TWord.Destroy;
begin
  Clear;
  Children.Free;
end;

{   TExWord   }

function TExWord.Calculate: double;
var i, k: integer;
    lt1, lt2: double;
begin
  case SelfTyp of
    ctNumber:Result := StrToFloat(StringReplace(SelfExpression, '.', ',', []));
    ctVarible:Result := Pvaribles[SelfExpression[1]].Varible;
    ctFunction:Result := PFncList^.DoFunction(FncId, CalculateParam(param), Precision);
    ctExpression:if Children.Count = 1 then Result := TExWord(Children[0]).Calculate else
        begin
          for k := 1 to 3 do
          begin
            i := FindOperator(OP_Kolejnosc[k], Children);
            while i <> -1 do
            begin
              lt1 := TExWord(Children[i - 1]).Calculate;
              lt2 := TExWord(Children[i + 1]).Calculate;

              TExWord(Children[i]).SelfExpression := DoExpression(TExWord(Children[i]).SelfExpression, lt1, lt2);
              TExWord(Children[i]).SelfTyp := ctNumber;
              TExWord(Children[i - 1]).Free;
              TExWord(Children[i + 1]).Free;
              Children.Delete(i + 1);
              Children.Delete(i - 1);
              i := FindOperator(OP_Kolejnosc[k], Children);
            end;
          end;
          Result := StrToFloat(TExWord(Children[0]).SelfExpression);
        end;
    ctUnDefined: Result := 0;
    else Result := 0;
  end;
  if not positive then Result := -Result; 
  Result := StrToFloat(PrecisionFormat(Result));
end;

procedure TExWord.SetSelf(Expr: string);
var i, j, l, n: integer;
    Expression, Txt: string;
    pos: bool;
begin
  positive := true;
  Expression := Expr;
  SelfExpression := Expression;
  SelfTyp := ctExpression;

  CutBracket(Expression);

  i := 1;
  while i < length(Expression) + 1 do
  begin
    case TypZnaku(Expression[i]) of
      ctNumber:begin
                 Txt := Expression[i];
                 j := 1;
                 while(TypZnaku(Expression[i + j]) = ctNumber)or(TypZnaku(Expression[i + j]) = ctComma) do
                 begin
                   Txt := Txt + Expression[i + j];
                   inc(j);
                 end;
                 pos := true;
                 if i > 0 then if Expression[i - 1] = '-' then pos := false;
                 Add(Txt, ctNumber, pos, PFncList, PVaribles);
                 inc(i, j);
                 if TypZnaku(Expression[i]) = ctOpenN then Add('*', ctOperator, true, PFncList, PVaribles);
                 if TypZnaku(Expression[i]) = ctVarible then Add('*', ctOperator, true, PFncList, PVaribles);
                 if TypZnaku(Expression[i]) = ctFunction then Add('*', ctOperator, true, PFncList, PVaribles);
               end;
      ctOperator:begin
                   Add(Expression[i], ctOperator, true, PFncList, PVaribles);
                   inc(i);
                 end;
      ctMinus:begin
                if i > 1 then Add('+', ctOperator, true, PFncList, PVaribles);
                inc(i);
              end;
      ctOpenN:begin
                 l := 1;
                 j := 1;
                 Txt := Expression[i];
                 while((l > 0)and(i + j < length(Expression) + 1))do
                 begin
                   if TypZnaku(Expression[i + j]) = ctOpenN then Inc(l);
                   if TypZnaku(Expression[i + j]) = ctCloseN then dec(l);
                   Txt := Txt + Expression[i + j];
                   inc(j);
                 end;
                 pos := true;
                 if i > 0 then if Expression[i - 1] = '-' then pos := false;
                 Add(Txt, ctExpression, pos, PFncList, PVaribles);
                 inc(i, j);
                 if TypZnaku(Expression[i]) = ctOpenN then Add('*', ctOperator, true, PFncList, PVaribles);
                 if TypZnaku(Expression[i]) = ctVarible then Add('*', ctOperator, true, PFncList, PVaribles);
                 if TypZnaku(Expression[i]) = ctFunction then Add('*', ctOperator, true, PFncList, PVaribles);
                 if TypZnaku(Expression[i]) = ctNumber then Add('*', ctOperator, true, PFncList, PVaribles);
               end;
      ctVarible:begin
                  pos := true;
                  if i > 0 then if Expression[i - 1] = '-' then pos := false;
                  Add(Expression[i], ctVarible, pos, PFncList, PVaribles);
                  inc(i);
                  if TypZnaku(Expression[i]) = ctOpenN then Add('*', ctOperator, true, PFncList, PVaribles);
                  if TypZnaku(Expression[i]) = ctNumber then Add('*', ctOperator, true, PFncList, PVaribles);
                  if TypZnaku(Expression[i]) = ctVarible then Add('*', ctOperator, true, PFncList, PVaribles);
                  if TypZnaku(Expression[i]) = ctFunction then Add('*', ctOperator, true, PFncList, PVaribles);
                end;
      ctUnDefined:begin
                    inc(i);
                  end;
      ctComma:begin
                inc(i);
              end;
      ctCloseN:begin
                 inc(i);
               end;
      ctFunction:begin
                   Txt := Expression[i];
                   j := 1;
                   while TypZnaku(Expression[i + j]) = ctFunction do
                   begin
                     Txt := Txt + Expression[i + j];
                     inc(j);
                   end;
                   n := 0;
                   if TypZnaku(Expression[i + j]) = ctOpenN then
                   begin
                     l := 1;
                     n := 1;
                     Txt := Txt + Expression[i + j];
                     while((l > 0)and(i + n + j < length(Expression) + 1))do
                     begin
                       if TypZnaku(Expression[i + j + n]) = ctOpenN then Inc(l);
                       if TypZnaku(Expression[i + j + n]) = ctCloseN then dec(l);
                       Txt := Txt + Expression[i + j + n];
                       inc(n);
                     end;
                   end;
                   pos := true;
                   if i > 0 then if Expression[i - 1] = '-' then pos := false;
                   Add(Txt, ctFunction, pos, PFncList, PVaribles);
                   inc(j, n);
                   inc(i, j);
                   if TypZnaku(Expression[i]) = ctOpenN then Add('*', ctOperator, true, PFncList, PVaribles);
                   if TypZnaku(Expression[i]) = ctVarible then Add('*', ctOperator, true, PFncList, PVaribles);
                   if TypZnaku(Expression[i]) = ctFunction then Add('*', ctOperator, true, PFncList, PVaribles);
                 end;
      ctEqual:begin
                inc(i);
              end;
    end;
  end;
end;

procedure TExWord.Add(Expression: string; Typ: TCharType; Pos: bool; FList: PFunctionList; PVar: PVaribles);
var Word: TExWord;
    i, c, l: integer;
    temp: string;
begin
  Word := TExWord.Create;
  Word.positive := Pos;
  Word.SelfTyp := Typ;
  Word.SetIndicators(FList, PVar);
  Word.Precision := Precision;
  case Typ of
    ctExpression:begin
                   if Expression[1] = '(' then Expression := copy(Expression, 2, length(Expression) - 2);
                   Word.SetSelf(Expression);
                 end;
    ctFunction:begin
                 temp := Expression;
                 i := FindType(ctOpenN, temp);
                 Word.FncId := Copy(temp, 0, i - 1);
                 Delete(temp, 1, i - 1);
                 Temp := Copy(temp, 2, length(temp)-2);
                 c := 0;
                 l := 0;
                 i := 0;

                 if length(temp) > 0 then
                 while(i < length(temp))do
                 begin
                   inc(i);
                   if TypZnaku(temp[i]) = ctOpenN then inc(l);
                   if TypZnaku(temp[i]) = ctCloseN then dec(l);

                   if((temp[i] = ';')or(i = length(temp)))and(l = 0)then
                   begin
                     inc(c);
                     SetLength(Word.param, c);
                     Word.param[c - 1] := copy(temp, 0, i);
                     if Word.param[c - 1][length(Word.param[c - 1])] = ';' then Word.param[c - 1] := copy(Word.param[c - 1], 1, length(Word.param[c - 1]) - 1);
                     Delete(temp, 1, i);
                     i := 0;
                   end;
                 end;
                 Word.SelfTyp := ctFunction;
               end;
    else Word.SelfExpression := Expression;
  end;
  Children.Add(Word);
end;

constructor TExWord.Create;
begin
  inherited Create;
  SetPrecision(10);
end;

destructor TExWord.Destroy;
begin
  inherited Destroy;
end;

function TExWord.PrecisionFormat(Value: Double): string;
var Txt: string;
    i: integer;
begin
  Txt := '0.';
  for i := 1 to Precision do Txt := Txt + '#';
  Result := formatFloat(Txt, Value);
end;

procedure TExWord.SetIndicators(PL: PFunctionList; PV: PVaribles);
begin
  PFncList := PL;
  PVaribles := PV;
end;

procedure TExWord.SetPrecision(cPrec: integer);
begin
  Precision := cPrec;
end;

function TExWord.CalculateParam(par: array of string): TParam;
var E: TExpression;
    i, j: integer;
    tmp: string;
begin
  E := TExpression.Create;
  SetLength(Result, length(par));
  for i := 0 to length(par) - 1 do
  begin
    tmp := par[i];
    for j := ord('A') to ord('Z') do
      if PVaribles^[chr(j)].Active then tmp := StringReplace(tmp, chr(j), FloatToStr(PVaribles^[chr(j)].Varible), [rfReplaceAll]);
    E.SetIndicators(PFncList, PVaribles);
    E.SetSelf(tmp);
    Result[i] := E.Calculate;
  end;
  E.Free;
end; 

{   TFunctionList   }

function TFunctionList.DoFunction(identifier: string; params: array of double; prec: integer):Double;
var i: integer;
begin
  Result := 0;
  if FList.Count < 1 then exit;
  i := 0;
  while(TFunctionListItem(FList[i]).identifier <> identifier)or(i < FList.Count - 1)do
  begin
    if TFunctionListItem(FList[i]).identifier = identifier then break;
    inc(i);
  end;
  
  if i >= FList.Count then exit;
  if TFunctionListItem(FList[i]).identifier = identifier then
  Result := TFunctionListItem(FList[i]).Calculate(params, prec);
end;

function SortList(I1, I2: Pointer): integer;
var Item1, Item2: TFunctionListItem;
begin
  Item1 := TFunctionListItem(I1);
  Item2 := TFunctionListItem(I2);
  Result := 0;
  case Item1.Typ of
    flitExpression:case Item2.Typ of
                     flitExpression:Result := CompareStr(Item1.identifier, Item2.identifier);
                     flitFunction:Result := -1;
                     flitLuaFuction:Result := -1;
                   end;
    flitFunction:case Item2.Typ of
                     flitExpression:Result := 1;
                     flitFunction:Result := CompareStr(Item1.identifier, Item2.identifier);
                     flitLuaFuction:Result := -1;
                   end;
    flitLuaFuction:case Item2.Typ of
                     flitExpression:Result := 1;
                     flitFunction:Result := -1;
                     flitLuaFuction:Result := CompareStr(Item1.identifier, Item2.identifier);
                   end;
  end;
end;

procedure TFunctionList.AddFunction(Fnc: Pointer; identifier: string; parCount:integer);
var Item: TFunctionListItem;
begin
  Item := TFunctionListItem.Create;

  Item.identifier := AnsiLowerCase(identifier);
  Item.parCount := parCount;
  Item.Fnc := Fnc;
  Item.Expression := identifier;
  Item.Typ := flitFunction;
  Item.PList := @Self;

  FList.Add(Item);
end;

procedure TFunctionList.AddLuaFunction(Id, Script: string; parCount:integer);
var Item: TFunctionListItem;
begin
  Item := TFunctionListItem.Create;

  Item.Typ := flitLuaFuction;
  Item.parCount := parCount;
  Item.PList := @Self;
  Item.identifier := AnsiLowerCase(Id);
  Item.Script := Script;

  FList.Add(Item);
  FList.Sort(SortList);
end;

procedure TFunctionList.AddFunction(Expression, identifier: string; parCount:integer);
var Item: TFunctionListItem;
begin
  Item := TFunctionListItem.Create;

  Item.identifier := AnsiLowerCase(identifier);
  Item.parCount := parCount;
  Item.Expression := Expression;
  Item.Typ := flitExpression;
  Item.PList := @Self;

  FList.Add(Item);
  FList.Sort(SortList);
end;

procedure TFunctionList.AddFromText(Text: string);
var i, parc: integer;
    Temp, ex, id: string;
    Typ: TFunctonListItemType;
begin
  temp := Text;
  while Length(Temp) > 3 do
  begin
    case ord(temp[1]) of
      ord('e'):Typ := flitExpression;
      ord('l'):Typ := flitLuaFuction;
      else Typ := flitExpression;
    end;
    System.Delete(Temp, 1, 2);
    i := AnsiPos(Lst_Txt_Separator, Temp);
    id := copy(Temp, 0, i - 1);
    system.Delete(Temp, 1, i);
    if Length(Temp) = 0 then exit;
    i := AnsiPos(Lst_Txt_Separator, Temp);
    ex := copy(Temp, 0, i - 1);
    system.Delete(Temp, 1, i);
    if Length(Temp) = 0 then exit;
    i := AnsiPos(Lst_Txt_Separator, Temp);
    parc := strtoint(copy(Temp, 0, i - 1));
    system.Delete(Temp, 1, i);
    case Typ of
      flitExpression:AddFunction(ex, id, parc);
      flitLuaFuction:AddLuaFunction(id, ex, parc);
    end;
  end;
end;

function TFunctionList.SaveToText: string;
var i: integer;
    item: TFunctionListItem;
begin
  Result := '';
  for i := 0 to FList.Count - 1 do
  begin
    item := GetItem(i);
    case item.Typ of
      flitExpression:Result := Result + 'e|' + item.identifier + Lst_Txt_Separator + item.Expression + Lst_Txt_Separator + IntToStr(item.parCount) + Lst_Txt_Separator;
      flitLuaFuction:Result := Result + 'l|' + item.identifier + Lst_Txt_Separator + item.Script + Lst_Txt_Separator + IntToStr(item.parCount) + Lst_Txt_Separator;
    end;
  end;
end;

procedure TFunctionList.Delete(Index: integer);
begin
  if GetItem(index).Typ = flitFunction then exit;
  TFunctionListItem(FList[index]).Free;
  FList.Delete(index);
end;

procedure TFunctionList.Modify(index: integer; id, ex, script: string; pc: integer);
var item: TFunctionListItem;
begin
  item := getItem(index);
  if item.Typ = flitFunction then exit;

  item.identifier := id;
  item.Expression := ex;
  item.parCount := pc;
  item.Script := script;
end;

constructor TFunctionList.Create;
begin
  Flist := TList.Create;
  AddStandard;
end;

destructor TFunctionList.Destroy;
begin
  Clear;
  Flist.Free;
end;

procedure TFunctionList.Clear;
var i: integer;
begin
  for i := 0 to FList.Count - 1 do TFunctionListItem(FList[i]).Free;
  FList.Clear;
  AddStandard;
end;

function TFunctionList.GetCount: integer;
begin
  Result := FList.Count;
end;

function TFunctionList.GetItem(index: integer): TFunctionListItem;
begin
  Result := TFunctionListItem(FList[index]);
end;

{  Internal math functions  }

function fSin(Param: array of double): double;
begin
  Result := Sin(Param[0]);
end;
function fCos(Param: array of double): double;
begin
  Result := Cos(Param[0]);
end;
function fTg(Param: array of double): double;
begin
  Result := Tan(Param[0]);
end;
function fCtg(Param: array of double): double;
begin
  if Param[0] = 0 then result := 0
  else Result := Cotan(Param[0]);
end;
function fAbs(Param: array of double): double;
begin
  Result := Abs(Param[0]);
end;
function fPi(Param: array of double): double;
begin
  Result := Pi;
end;
function fSignum(Param: array of double): double;
begin
  if Param[0] > 0 then Result := 1
  else if Param[0] = 0 then Result := 0
  else Result := -1;
end;
function fLoga(Param: array of double): double;
begin
  Result := Log2(Param[0]);
end;
function fLogb(Param: array of double): double;
begin
  Result := Log10(Param[0]);
end;
function fLogc(Param: array of double): double;
begin
  if Param[1] < 1 then Result := 0 else
  if Param[0] < 2 then Result := 0 else
    Result := LogN(Param[0], Param[1]);
end;
function fPower(Param: array of double): double;
begin
  if(Param[0] = 0)or(Param[1] = 0) then Result := 0 else
  Result := Power(Param[0], Param[1]);
end;
function fRandom(Param: array of double): double;
begin
  Randomize;
  Result := Random(trunc(Param[0]));
end;
function fExp(Param: array of double): double;
begin
  Result := Exp(Param[0]);
end;
function fSqrt(Param: array of double): double;
begin
  if Param[0] <= 0 then Result := 0 else Result := Sqrt(Param[0]);
end;
function fSinH(Param: array of double): double;
begin
   Result := Sinh(Param[0]);
end;
function fCosH(Param: array of double): double;
begin
   Result := Cosh(Param[0]);
end;
function fTgH(Param: array of double): double;
begin
   Result := Tanh(Param[0]);
end;
function fCtgH(Param: array of double): double;
begin
   if param[0] = 0 then result := 0 else   
   Result := CotH(Param[0]);
end;
function fArcSin(Param: array of double): double;
begin
   Result := ArcSin(Param[0]);
end;
function fArcCos(Param: array of double): double;
begin
   Result := ArcCos(Param[0]);
end;
function fArcTg(Param: array of double): double;
begin
   Result := ArcTan(Param[0]);
end;
function fArcCtg(Param: array of double): double;
begin
   Result := ArcCot(Param[0]);
end;
function fMax(Param: array of double): double;
begin
   Result := MaxValue(Param);
end;
function fMin(Param: array of double): double;
begin
   Result := MinValue(Param);
end;

{function f(Param: array of double): double;
begin
   Result := (Param[0]);
end;}

{  TFunctionListItem  }

procedure TFunctionList.AddStandard;
begin
 // AddFunction(@f, '', 1);

  AddFunction(@fSin, 'sin', 1);
  AddFunction(@fCos, 'cos', 1);
  AddFunction(@fTg,  'tg', 1);
  AddFunction(@fCtg, 'ctg', 1);
  AddFunction(@fSinH, 'sinh', 1);
  AddFunction(@fCosH, 'cosh', 1);
  AddFunction(@fTgH, 'tgh', 1);
  AddFunction(@fCtgH, 'ctgh', 1);
  AddFunction(@fArcSin, 'arcsin', 1);
  AddFunction(@fArcCos, 'arccos', 1);
  AddFunction(@fArcTg, 'arctg', 1);
  AddFunction(@fArcCtg, 'arcCtg', 1);

  AddFunction(@fAbs, 'abs', 1);
  AddFunction(@fPi, 'pi', 0);
  AddFunction(@fSignum, 'sgn', 1);
  AddFunction(@fPower, 'power', 2);
  AddFunction(@fExp, 'exp', 1);
  AddFunction(@fSqrt, 'sqrt', 1);

  AddFunction(@fLoga, 'loga', 1);
  AddFunction(@fLogb, 'logb', 1);
  AddFunction(@fLogc, 'logc', 2);
  
  AddFunction(@fRandom, 'random', 1);

  AddFunction(@fMax, 'Max', 2);
  AddFunction(@fMin, 'Min', 2);
end;

function TFunctionListItem.Calculate(params: array of double; prec: integer): double;
var Expr: TExpression;
    e: string;
    i: integer;
begin
  Result := 0;
  case Typ of
    flitExpression:begin
                     e := Expression;
                     for i := 0 to parCount - 1 do e := StringReplace(e, 'p[' + inttostr(i) + ']', FloatToStr(params[i]), [rfReplaceAll, rfIgnoreCase]);
                     Expr := TExpression.Create;
                     Expr.SetPrecision(prec);
                     Expr.SetSelf(e);
                     Result := Expr.Calculate;
                     Expr.Free;
                   end;
    flitFunction:Result := Fnc(params);
    flitLuaFuction:begin
                     if not LibraryPresent then
                     begin
                       Result := 0;
                       Exit;
                     end;
                     if not DoLuaFunction(identifier, Script, params, Result) then raise Exception.Create('Nie mogê wykon¹c funcji skryptowej!');
                   end;

  end;
end;

{   TTrWord   }

function TTrWord.Transform: string;
var i: integer;
begin
  Result := '';

  case SelfTyp of
//    ctNumber:if Positive then Result := Result + SelfExpression else
//         Result := '(-' + SelfExpression + ')';
//    ctOperator: ;
//    ctMinus: ;
    ctExpression:begin    
                   if Positive then Result := SelfExpression else
                   begin
                     if TTrWord(Children[0]).SelfTyp = ctNumber then
                       if(Positive <> TTrWord(Children[0]).Positive)then Result := Result + '-';
                     for i := 0 to Children.Count - 1 do
                     begin
                       if TTrWord(Children[i]).SelfTyp = ctOperator then
                       begin
                         if TTrWord(Children[i]).SelfExpression[1] = '+' then
                         begin
                           if(Positive = TTrWord(Children[i + 1]).Positive)then
                           begin
                             Result := Result + '+';
                           end
                           else Result := Result + '-';
                         end
                         else Result := Result + TTrWord(Children[i]).SelfExpression;
                       end
                       else Result := Result + TTrWord(Children[i]).SelfExpression;
                     end;
                   end;
                 end;
//    ctFunction: ;
//    ctVarible:if Positive then Result := Result + SelfExpression else
//         Result := '(-' + SelfExpression + ')';
    else Result := GetExpression;//Result := Result + SelfExpression
  end;
end;

procedure TTrWord.Add(Expression: string; Typ: TCharType; pos: bool; List: TList);
var Word: TTrWord;
//   i, {c,} l: integer;
//    temp: string;
begin
  Word := TTrWord.Create;
  Word.positive := true;
  Word.SelfTyp := Typ;
  Word.Positive := pos;
  Word.SelfExpression := Expression;

  case Typ of
    ctExpression:begin
                   if TypZnaku(Expression[1]) = ctOpenN then Expression := copy(Expression, 2, length(Expression) - 2);
                   Word.SetList(Expression, nil);
                   Word.SelfTyp := ctExpression;
                 end;
{    ctFunction:begin
                 temp := Expression;
                 i := FindType(ctOpenN, temp);
//                 Word.FncId := Copy(temp, 0, i - 1);
                 Delete(temp, 1, i - 1);
                 Temp := Copy(temp, 2, length(temp)-2);
//                 c := 0;
                 l := 0;
                 i := 0;

                 if length(temp) > 0 then
                 while(i < length(temp))do
                 begin
                   inc(i);
                   if TypZnaku(temp[i]) = ctOpenN then inc(l);
                   if TypZnaku(temp[i]) = ctCloseN then dec(l);

                   if((temp[i] = ';')or(i = length(temp)))and(l = 0)then
                   begin
//                     inc(c);
//                     SetLength(Word.param, c);
//                     Word.param[c - 1] := copy(temp, 0, i);
//                     if Word.param[c - 1][length(Word.param[c - 1])] = ';' then Word.param[c - 1] := copy(Word.param[c - 1], 1, length(Word.param[c - 1]) - 1);
                     Delete(temp, 1, i);
                     i := 0;
                   end;
                 end;
                 Word.SelfTyp := ctFunction;
               end;             }
    else Word.SelfExpression := Expression;
  end;
  List.Add(Word);
end;

procedure TTrWord.SetList(Expr: string; List: TList);
var i, j, l, n: integer;
    Expression, Txt: string;
    pos: bool;
    Lst: TList;
begin
  Expression := Expr;

  CutBracket(Expression);  

  if List = nil then Lst := Children else Lst := List;

  i := 1;
  while i < length(Expression) + 1 do
  begin
    case TypZnaku(Expression[i]) of
      ctNumber:begin
                 Txt := Expression[i];
                 j := 1;
                 while(TypZnaku(Expression[i + j]) = ctNumber)or(TypZnaku(Expression[i + j]) = ctComma) do
                 begin
                   Txt := Txt + Expression[i + j];
                   inc(j);
                 end;
                 pos := true;
                 if i > 0 then if Expression[i - 1] = '-' then pos := false;
                 Add(Txt, ctNumber, pos, Lst);
                 inc(i, j);
                 if TypZnaku(Expression[i]) in [ctOpenN, ctVarible, ctFunction] then Add('*', ctOperator, true, Lst);
               end;
      ctOperator:begin
                   Add(Expression[i], ctOperator, true, Lst);
                   inc(i);
                 end;
      ctMinus:begin
                if i > 0 then Add('+', ctOperator, true, Lst);
                inc(i);
              end;
      ctOpenN:begin
                 l := 1;
                 j := 1;
                 Txt := Expression[i];
                 while((l > 0)and(i + j < length(Expression) + 1))do
                 begin
                   if TypZnaku(Expression[i + j]) = ctOpenN then Inc(l);
                   if TypZnaku(Expression[i + j]) = ctCloseN then dec(l);
                   Txt := Txt + Expression[i + j];
                   inc(j);
                 end;
                 pos := true;
                 if i > 0 then if Expression[i - 1] = '-' then pos := false;
                 Add(Txt, ctExpression, pos, Lst);
                 inc(i, j);
                 if TypZnaku(Expression[i]) in [ctOpenN, ctVarible, ctFunction, ctNumber] then Add('*', ctOperator, true, Lst);
               end;
      ctVarible:begin
                  pos := true;
                  if i > 0 then if Expression[i - 1] = '-' then pos := false;
                  Add(Expression[i], ctVarible, pos, Lst);
                  inc(i);
                  if TypZnaku(Expression[i]) in [ctOpenN, ctVarible, ctFunction, ctNumber] then Add('*', ctOperator, true, Lst);
                end;
      ctUnDefined:begin
                    inc(i);
                  end;
      ctComma:begin
                inc(i);
              end;
      ctCloseN:begin
                 inc(i);
               end;
      ctFunction:begin
                   Txt := Expression[i];
                   j := 1;
                   while TypZnaku(Expression[i + j]) = ctFunction do
                   begin
                     Txt := Txt + Expression[i + j];
                     inc(j);
                   end;
                   n := 0;
                   if TypZnaku(Expression[i + j]) = ctOpenN then
                   begin
                     l := 1;
                     n := 1;
                     Txt := Txt + Expression[i + j];
                     while((l > 0)and(i + n + j < length(Expression) + 1))do
                     begin
                       if TypZnaku(Expression[i + j + n]) = ctOpenN then Inc(l);
                       if TypZnaku(Expression[i + j + n]) = ctCloseN then dec(l);
                       Txt := Txt + Expression[i + j + n];
                       inc(n);
                     end;
                   end;
                   pos := true;
                   if i > 0 then if Expression[i - 1] = '-' then pos := false;
                   Add(Txt, ctFunction, pos, Lst);
                   inc(j, n);
                   inc(i, j);
                   if TypZnaku(Expression[i]) in [ctOpenN, ctVarible, ctFunction, ctNumber] then Add('*', ctOperator, true, Lst);                end;
      ctEqual:begin
                inc(i);
              end;
    end;
  end;
end;

procedure TTrWord.Clear;
begin
  ClearList(Children);
  SelfExpression := '';
end;

procedure TTrWord.ClearList(List: TList);
var i: integer;
begin
  for i := 0 to List.Count - 1 do TTrWord(List[i]).Free;
  List.Clear;
end;

constructor TTrWord.Create;
begin
  Children := TList.Create;
end;

destructor TTrWord.Destroy;
begin
  Clear;
end;

{   TTransformation   }

procedure TTransformation.Prepare(Expr: string);
var i: integer;
    Expression, Txt: string;
begin
  Clear;
  Expression := Expr;
  positive := true;
  SelfExpression := Expression;

  i := AnsiPos('=', Expression);
  if i < 1 then
  begin
    SelfTyp := ttExpression;
    SetList(Expression, LeftList);
  end else
  begin
    SelfTyp := ttEquation;
    Txt := copy(Expression, 0, i - 1);
    SetList(Txt, LeftList);
    Delete(Expression, 1, i);
    SetList(Expression, RigthList);
  end;

  Form1.Add('prepared ll:'+GetExpression(LeftList));
  Form1.Add('prepared rl:'+GetExpression(RigthList));
end;

function TTransformation.CutSimilar(List: TList): bool;
var i, j: integer;
    can: bool;
    label Koniec;
begin
  Result := false;
  for i := 0 to List.Count - 1 do
  for j := 0 to List.Count - 1 do
    if(TTrWord(List[i]).SelfTyp in [ctNumber, ctVarible])and
      (TTrWord(List[j]).SelfTyp in [ctNumber, ctVarible])and
      (TTrWord(List[i]).SelfExpression = TTrWord(List[j]).SelfExpression)and
      (TTrWord(List[i]).Positive = not TTrWord(List[j]).Positive)then
    begin
      can := true;
      if i > 0 then
        if(TTrWord(List[i - 1]).SelfTyp = ctOperator)and
          not (TTrWord(List[i - 1]).SelfExpression[1] in ['+', '-'])then can := false;
      if i < List.Count - 1 then
        if(TTrWord(List[i + 1]).SelfTyp = ctOperator)and
          not (TTrWord(List[i + 1]).SelfExpression[1] in ['+', '-'])then can := false;

      if j > 0 then
        if(TTrWord(List[j - 1]).SelfTyp = ctOperator)and
          not (TTrWord(List[j - 1]).SelfExpression[1] in ['+', '-'])then can := false;
      if j < List.Count - 1 then
        if(TTrWord(List[j + 1]).SelfTyp = ctOperator)and
          not (TTrWord(List[j + 1]).SelfExpression[1] in ['+', '-'])then can := false;

      if not can then continue;

      List.Delete(j);
      if j > 0 then List.Delete(j - 1);
      List.Delete(i);
      if i > 0 then List.Delete(i - 1);
      Result := true;
      goto Koniec;
    end;
  Koniec:
end;

function TTransformation.Multiply(Factor1, Factor2: string): string;
var F1, F2: TList;
    i, j, n1, n2, l: integer;
    T1, T2, tmp:string;
    Mark: byte;
begin
  Result := '';
  if(Length(Factor1) = 0)or(Length(Factor2) = 0)then exit;

  form1.add('multiply: '+factor1+'    '+factor2);

  F1 := TList.Create;
  F2 := TList.Create;

  SetList(Factor1, F1);
  SetList(Factor2, F2);

  i := 0;

while i < F1.Count do
begin
  j := -1;
  while j < F2.Count - 1 do
  begin
    inc(j);
    if not(TTrWord(F1[i]).SelfTyp in [ctVarible, ctNumber, ctExpression])or
       not(TTrWord(F2[j]).SelfTyp in [ctVarible, ctNumber, ctExpression])then
       Continue;
    if length(Result) > 0 then
      if Result[length(Result)] <> '+' then Result := Result + '+';

    Mark := 0;
    if i > 0 then
    begin
      if TTrWord(F1[i - 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
    end else inc(mark);
    if i < F1.Count - 1 then
    begin
      if TTrWord(F1[i + 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
    end else inc(mark);
    if j > 0 then
    begin
      if TTrWord(F2[j - 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
    end else inc(mark);
    if j < F2.Count - 1 then
    begin
      if TTrWord(F2[j + 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
    end else inc(mark);
    if mark > 3 then
    begin
      T1 := TTrWord(F1[i]).GetExpression;
      T2 := TTrWord(F2[j]).GetExpression;
      if length(T1) > 1 then T1 := '(' + T1 + ')';
      if length(T2) > 1 then T2 := '(' + T2 + ')';
      Result := Result + T1 + '*' + T2;
      Continue;
    end;

    n1 := 0;
    n2 := 0;

    if i < F1.Count then
      while(i + n1 < F1.Count - 1)do
      begin
        if i + n1 + 1 < F1.Count then
          if (TTrWord(F1[i + n1 + 1]).SelfExpression[1] in ['+', '-']) then break;
        inc(n1);
      end;
    if j < F2.Count then
      while(j + n2 < F2.Count - 1)do
      begin
        if j + n2 + 1 < F2.Count then
          if(TTrWord(F2[j + n2 + 1]).SelfExpression[1] in ['+', '-']) then break;
        inc(n2);
      end;
    if(n1 > 0)or(n2 > 0)then
    begin
      T1 := '';
      T2 := '';
      for l := i to i + n1 do
      begin
        tmp := TTrWord(F1[l]).GetExpression;
        if length(tmp) > 1 then tmp := '(' + tmp + ')';
        T1 := T1 + tmp;
      end;
      for l := j to j + n2 do
      begin
        tmp := TTrWord(F2[l]).GetExpression;
        if length(tmp) > 1 then tmp := '(' + tmp + ')';
        T2 := T2 + tmp;
      end;
      Result := Result + T1 + '*' + T2;
      inc(j, n2);

     Continue;
    end;
  end;
  inc(i);
end;

  ClearList(F1);
  ClearList(F2);
  F1.Free;
  F2.Free;

  if Result[Length(Result)] = '+' then Result := Copy(Result, 0, length(Result) - 1);
end;

function TTransformation.Calculate(List: TList): bool;
var i, j, o: integer;
    can: bool;
    lt1, lt2: Double;
    label Koniec;
begin
  Result := false;
  if List.Count < 3 then exit;

  for j := 1 to 3 do
  for o := 1 to 2 do
  begin
    i := 0;
    while i < List.Count do
    begin
      if(i = 0)and(TTrWord(List[i]).SelfTyp = ctOperator)then
      begin
        TTrWord(List[i]).Free;
        List.Delete(i);
        Continue;
      end;

      if(TTrWord(List[i]).SelfExpression[1] = OP_Kolejnosc[j, o])and
        (TTrWord(list[i - 1]).SelfTyp in [ctNumber])and
        (TTrWord(list[i + 1]).SelfTyp in [ctNumber])then
      begin
        can := true;

        if i > 2 then
          if TTrWord(List[i - 2]).SelfExpression <> TTrWord(List[i]).SelfExpression then can := false;

        if i < List.Count - 2 then
          if TTrWord(List[i + 2]).SelfExpression <> TTrWord(List[i]).SelfExpression then can := false;

        if can then
        begin
          lt1 := StrToFloat(TTrWord(List[i - 1]).GetExpression);
          lt2 := StrToFloat(TTrWord(List[i + 1]).GetExpression);

          TTrWord(List[i]).SelfExpression := DoExpression(TTrWord(List[i]).SelfExpression, lt1, lt2);
          TTrWord(List[i]).SelfTyp := ctNumber;
          TTrWord(List[i + 1]).Free;
          TTrWord(List[i - 1]).Free;
          List.Delete(i + 1);
          List.Delete(i - 1);
          goto Koniec;
        end;
      end;
      inc(i);
    end;
  end;
  Koniec:
end;

procedure TTransformation.SimplifyList(List: TList);
var JobsDone: integer;
    i, c: integer;
    Txt, Factor: string;
begin
  JobsDone := 1;
  i := 0;

  while(JobsDone > 0)or(i < LeftList.Count)do
  begin
    if CutSimilar(LeftList) then inc(JobsDone);
    if Calculate(LeftList) then inc(JobsDone);

    if i < LeftList.Count then
    if TTrWord(LeftList[i]).SelfTyp = ctExpression then
    begin
      c := LeftList.Count;

      Txt := TTrWord(LeftList[i]).Transform;
      LeftList.Delete(i);
      if i < LeftList.Count then
      if(TTrWord(LeftList[i]).SelfTyp = ctOperator)and
        (TTrWord(LeftList[i]).SelfExpression[1] = '*') then
      begin
        Factor := TTrWord(LeftList[i + 1]).Transform;
        Txt := '(' +  Multiply(Txt, Factor) + ')';
        LeftList.Delete(i);
        LeftList.Delete(i);
      end;

      InsertWords('('+Txt+')', LeftList, i);
      if c <> LeftList.Count then i := 0;
    end;

    inc(i);
    if JobsDone > 0 then dec(JobsDone);
  end;

  if LeftList.Count = 0 then Add('0', ctNumber, true, LeftList)
  else if TTrWord(LeftList[0]).SelfTyp = ctOperator then LeftList.Delete(0);
end;

procedure TTransformation.SolveEquation(LL, RL: TList);
  function MoveFree(L1, L2: TList; What: TCharSet): bool;
  var i, Mark: integer;
      Tmp: string;
      label Koniec;
  begin
    result := false;
    for i := 0 to L1.Count - 1 do
      if(TTrWord(L1[i]).SelfTyp in What)then
      begin
        Mark := 0;
        if i > 0 then
        begin
        if TTrWord(L1[i - 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
        end else inc(mark);
        if i < L1.Count - 1 then
        begin
          if TTrWord(L1[i + 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
        end else inc(mark);

        if Mark = 2 then
        begin
          Tmp := TTrWord(L1[i]).GetExpression;
          if Tmp[1] = '-' then Tmp := '+' + copy(Tmp, 2, length(tmp)) else Tmp := '-' + Tmp;

          TTrWord(L1[i]).Free;
          L1.Delete(i);

          if i > 0 then
          begin
            TTrWord(L1[i - 1]).Free;
            L1.Delete(i - 1);
          end;

          InsertWords(Tmp, L2, L2.Count);
          Result := true;
          goto Koniec;
        end;
      end;
    Koniec:
  end;

var i, JobsDone, j: integer;
    tmp: string;
begin
  tmp := '';
  for i := 0 to LL.Count - 1 do
    if(TTrWord(LL[i]).SelfTyp = ctVarible)and(pos(TTrWord(LL[i]).SelfExpression, tmp) = 0)then
       tmp := tmp + TTrWord(LL[i]).SelfExpression;
  for i := 0 to RL.Count - 1 do
    if(TTrWord(RL[i]).SelfTyp = ctVarible)and(pos(TTrWord(RL[i]).SelfExpression, tmp) = 0)then
       tmp := tmp + TTrWord(RL[i]).SelfExpression;

  if Length(tmp) > 1 then raise Exception.Create('To nie jest równanie pierwszego stopnia!!!');

  JobsDone := 1;
  while JobsDone > 0 do
  begin
    if MoveFree(LL, RL, [ctNumber]) then inc(JobsDone);
    if MoveFree(RL, LL, [ctVarible]) then inc(JobsDone);
    if CutSimilar(LeftList) then inc(JobsDone);
    if Calculate(LeftList) then inc(JobsDone);
    if CutSimilar(RigthList) then inc(JobsDone);
    if Calculate(RigthList) then inc(JobsDone);

    j := FindType(ctVarible, LL);
    if j > 0 then
    

    if JobsDone > 0 then dec(JobsDone);
  end;
end;

function TTransformation.Transform: string;
begin
  Result := '0';

  SimplifyList(LeftList);
  form1.Add('left-simplified');

  if SelfTyp = ttEquation then
  begin
    SimplifyList(RigthList);
    form1.Add('right-simplified');
    SolveEquation(LeftList, RigthList);
    form1.Add('Solved');
  end;

  Result := GetExpression(LeftList);
  if SelfTyp = ttEquation then Result := Result + '=' + GetExpression(RigthList);
end;

procedure TTransformation.InsertWords(Txt: string; Lst: TList; Index: integer);
var i: integer;
    TmpL: TList;
begin
  TmpL := TList.Create;
  SetList(Txt, TmpL);

  for i := 0 to TmpL.Count - 1 do Lst.Insert(i + index, TmpL[i]);

  TmpL.Free;
end;

function TTransformation.GetExpression: string;
begin
  case SelfTyp of
    ttUnknown: result := 'B³¹d!';
    ttEquation: Result := GetExpression(LeftList) + '=' + GetExpression(RigthList);
    ttExpression: Result := GetExpression(LeftList);
  end;
end;

procedure TTransformation.Clear;
begin
  ClearList(RigthList);
  ClearList(LeftList);
  SelfTyp := ttUnknown;
end;

constructor TTransformation.Create;
begin
  RigthList := TList.Create;
  LeftList := TList.Create;
end;

destructor TTransformation.Destroy;
begin
  Clear;
  LeftList.Free;
  RigthList.Free;
end;

end.
