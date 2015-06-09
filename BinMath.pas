unit BinMath;

interface
uses Classes;

type TFunkcjaObliczen0arg = function : Double;
type TFunkcjaObliczen1arg = function(A : Double) : Double;
type TFunkcjaObliczen2arg = function(A,B : Double) : Double;
type TFunkcjaObliczen3arg = function(A,B,C : Double) : Double;
type TFunkcjaObliczen4arg = function(A,B,C,D : Double) : Double;


type TStosDane = class
      private
        Nazwa     : String;
        Priorytet : Integer;
      public
        constructor Create(S : String; I : Integer);
     end;

type TStos = class
        private
         FLista : TList;
        public
         constructor Create;
         destructor Destroy; override;

         procedure Dodaj(S : String; Priorytet : Integer = 0);
         function Zdejmij : String;

         function Istnieje : Boolean;

         function AktualnyPriorytet : Integer;
         procedure Wyczysc;
     end;

type TZmienneDane = class
       private
          Nazwa      : String;
          Wartosc    : Double;
       public
          constructor Create(N : String;W : Double);
       end;

type TFunkcjeDane = class
       private
          Nazwa      : String;
          Priorytet  : Integer;
          Adres      : Pointer;
          Argumentow : Integer;
       public
          constructor Create(N : String; P : Integer; D : Pointer; A : Integer);
       end;

type TObliczenia = class
   private
    FListaFunkcji : TList;                   //lista funkcji
    FZmienne      : TList;                   //lista zmiennych
    FStosONP      : TStos;                   //stos do konwersji na ONP
    FStosWynik    : TStos;                   //stos do obliczenia wyniku
    FWyrazenie    : String;                  //wprowadzone wyraøenie
    FONP          : String;                  //wyraøenie zamienione na ONP

    FKodError  : Integer;                    //kod b≥Ídu

    procedure GenerujONP;                    //generowanie ONP

   public
    procedure Wyrazenie(S : String);      //wprowadzenie wyraøenia
    function Wynik : Double;              //odczytanie wyniku

    function ONP   : String;              //odczytanie wyraøenia ONP
    function Blad  : Integer;             //odczytanie kodu b≥Ídu
    function BladOpis :String;            //opis wystÍpujπcego b≥Ídu

    //zarzπdzanie obs≥ugiwanymi funkcjami
    procedure FunkcjeRejestruj(Nazwa : String; Priorytet : Integer; Arg : Integer; Adres : Pointer);
    function FunkcjeID(Nazwa : String) : Integer;
    function FunkcjeInfo(Id : Integer) : TFunkcjeDane;
    procedure FunkcjeWyczysc;

    //obs≥uga zmiennych
    procedure ZmienneDodaj(N : String; W : Double);
    procedure ZmienneWyczysc;

    //konstruktor i destruktor
    constructor Create;
    destructor Destroy; override;
  end;

function Interpreter(Input: string): string;
function Eval(Expression: String): String;

implementation
uses SysUtils,Math;

function Eval(Expression: String): String;
{
Kod pochodzi z http://4programmers.net/Delphi/Gotowce/Interpreter_rÛwnaÒ
Oryginalny autor - Pixel (markovcd@gmail.com)
}
type
  TCharSet = set of char;

  function PosEx(substr: TCharSet; str: String): Integer;
  var
    i: Integer;
  begin
    Result := 0;
    for i := 1 to Length(str) do
    if str[i] in substr then begin
      Result := i;
      Break;
    end;
  end;

  function Eval2(Expression: String): String;
  var
    i, j: Integer;
    s, s1, s2, s3, left, right: String;
    e: Extended;
  begin
    s := Expression;
    i := PosEx(['*', '/'], s); // pierwszy znak "*" lub "/"
    if i <> 0 then begin // znaleziono znak "*" lub "/"
      left := Copy(s, 1, i - 1); // pierwsze wyrazenie wielomianu
      s1 := Copy(s, i + 1, Length(s));
      j := PosEx(['*', '/'], s1);
      if j <> 0 then
        right := Copy(s1, 1, j - 1) // drugie wyrazenie wielomianu
      else
        right := s1;

      if s[i] = '*' then // wykonanie dzialania
        e := StrToFloat(left) * StrToFloat(right)
      else
        e := StrToFloat(left) / StrToFloat(right);

      s2 := Format('%g', [e]);

      if j <> 0 then
        s3 := Copy(s1, j, Length(s1));

      Result := Eval(s2 + s3);
    end else
      Result := s;

  end;

var
  i, j, k, l, m: Integer;
  s, s1, s2, s3, left, right: String;
  e: Extended;
label
  a;
begin
  s := StringReplace(Expression, ' ', '', [rfReplaceAll]); // pozbywamy sie spacji
  if s[1] in ['+', '-', '*', '/', '^'] then s[1] := ' ';
  if s[Length(s)] in ['+', '-', '*', '/', '^'] then s[Length(s)] := ' '; // pozbywamy sie znakow z brzegow
  s := Trim(s);
a:// obsluga nawiasow
  k := 0; l := 0;
  for i := 1 to Length(s) do begin
    if s[i] = '(' then begin
      if k = 0 then m := i; // pozycja pierwszego nawiasu
      Inc(k); // znaleziono nawias otwierajacy
    end
    else if s[i] = ')' then Inc(l) // znaleziono nawias zamykajacy
    else Continue;

    if (k = l) and (k <> 0) then begin // jezeli tyle samo nawiasow zamykajacych i otwierajacych
      s3 := Eval(Copy(s, m + 1, i - m - 1)); // obliczenie wszystkiego w srodku nawiasu
      s := Copy(s, 1, m - 1) + s3 + Copy(s, i + 1, Length(s)); // zamienienie tego co w nawiasie na wartosc obliczona
      goto a; // idziemy do poczatku aby szukac nawiasow
    end;

  end;


  i := PosEx(['+', '-'], s); // pierwszy znak "+" lub "-"
  if i <> 0 then begin // znaleziono znak "+" lub "-"
    left := Copy(s, 1, i - 1); // pierwsze wyrazenie wielomianu
    s1 := Copy(s, i + 1, Length(s));
    j := PosEx(['+', '-'], s1);
    if j <> 0 then
      right := Copy(s1, 1, j - 1) // drugie wyrazenie wielomianu
    else
      right := s1;

    left := Eval2(left);
    right := Eval2(right);

    if s[i] = '+' then // wykonanie dzialania
      e := StrToFloat(left) + StrToFloat(right)
    else
      e := StrToFloat(left) - StrToFloat(right);

    s2 := Format('%g', [e]);

    if j <> 0 then
      s3 := Copy(s1, j, Length(s1));

    Result := Eval(s2 + s3);
  end else
    Result := Eval2(s);
end;

function Interpreter(Input: string): string;
(******************************************************************************
uses Math require !

function Interpreter(Input: string): string;
--------------------------------------------

[   v 0.9.1   ]]
================

This code is owned by HAKGERSoft, any modifications without HAKGERSoft
permision is prohibited!

Author:
  DetoX [detox@satanbsd.org]
===============================================================================

Funkcja realizuje obliczenia matematyczne z danego wyraøenia np: 4+(4*sin(5-2))
Podany algorytm jest w≥asnoúciπ HAKGERSoft - opiera siÍ na atomizacji wyraøenia
i nie wykorzystuje odwrotnej notacji polskiej.

(TODO)
 - odpornoúÊ na dzielenie przez zero
 - odpornoúÊ przed przekroczeniem zakresu funkcji arcus, np. arcsin(7)
 - bardziej szczegÛ≥owe sprawdzenie poprawnoúci zapisu wyraøenia
 - moøna dodaÊ wiÍcej funkcji

*******************************************************************************)

const
  H_NULL = '';
  H_PRECISION = '0.00000000000000000000'; // Dok≥adnoúÊ obliczeÒ
  H_RESULT = '0.###'; // Zaokrπglenie wyniku
  H_POINT = ',';
  H_B1 = '('; // Znak otwarcia nawiasa - start nowego "atomu"
  H_B2 = ')'; // Znak zamkniÍcia nawiasa - koniec "atomu"
  H_PI = 3.14159265358979323846; // WartoúÊ liczby Pi
  //H_ERROR = 'B≥πd sk≥adni'; // B≥πd
  
  Numbers: set of char = ['0'..'9']; // Cyfry
  Letters: set of char = ['a'..'z', 'A'..'Z', '≥', 'π', 'Í', 'ú', 'Ê', 'Ò', 'ü', 'ø', 'Û', '£', '•', ' ', 'å', '∆', '—', 'è', 'Ø', '”']; // Litery alfabetu
  BasicOpr: array [1..5] of Char = ('^', '*', '/', '+', '-'); // Podstawowe operatory matematyczne
  (* Powyøsza kolejnoúÊ elementÛw w tablicy decyduje o kolejnoúci wykonywania dzia≥aÒ! *)
  MathFunc: array [1..11] of string = ('arcsin', 'arccos', 'arctg', 'arcctg', 'sin', 'cos', 'tg', 'ctg', 'ln', 'exp', 'sqrt'); // Funkcje
  Atom_Begin: array [1..2] of Char = ('[', '{'); // Nawiasy otwierajπce
  Atom_End: array [1..2] of Char = (']', '}'); // Nawiasy zamykajπce

  function Optimize(Input: string): string;
  var
    i: Integer;
  begin
    Input := Trim(AnsiLowerCase(Input));
    Input := StringReplace(Input, ' ', H_NULL, [rfReplaceAll]);
    for i := Low(Atom_Begin) to High(Atom_Begin) do
      Input := StringReplace(Input, Atom_Begin[i], H_B1, [rfReplaceAll]);
    for i := Low(Atom_End) to High(Atom_End) do
      Input := StringReplace(Input, Atom_End[i], H_B2, [rfReplaceAll]);
    Input := StringReplace(Input, '.', ',', [rfReplaceAll]);
    Result := Input;
  end;

  function Valid(Input: string): Boolean;
  var
    i, B1_Count, B2_Count: Integer;
  begin
    Result := True;
    B1_Count := 0;
    B2_Count := 0;
    for i := 1 to length(Input) do
      if Input[i] = H_B1 then
        Inc(B1_Count)
      else if Input[i] = H_B2 then
        Inc(B2_Count);
    Result := Boolean(B1_Count = B2_Count);

    for i := Low(MathFunc) to High(MathFunc) do
      Input := StringReplace(Input, MathFunc[i], H_NULL, [rfReplaceAll]);
    for i := 1 to length(Input) do
      if Input[i] in Letters then
        Result := False;
  end;

  function RightValue(Input: string): string;
  var
    i: Integer;
  begin
    Result := H_NULL;
    for i := 1 to length(Input) do
    begin
      if (Input[i] in Numbers) or (Input[i] = H_POINT) or ((Input[i] = '-') and (i = 1)) then
        Result := Result + Input[i]
      else
        Break;
    end;
  end;

  function LeftValue(Input: string): string;
  var
    i: Integer;
    Reverse: string;
  begin
    Reverse := H_NULL;
    Result := H_NULL;
    for i := length(Input) downto 1 do
    begin
      if (Input[i] in Numbers) or (Input[i] = H_POINT) then
        Reverse := Reverse + Input[i]
      else if (Input[i] = '-') then
      begin
        if i < length(Input) then
          if Input[i + 1] in Numbers then
          begin
            Reverse := Reverse + Input[i];
            Break;
          end;
      end
      else
        Break;
    end;
    for i := length(Reverse) downto 1 do
      Result := Result + Reverse[i];
  end;
  
  function GetBracket(Input: string): Integer;
  var
    i: Integer;
    Counter: ShortInt;
  begin
    Counter := 0;
    for i := 1 to length(Input) do
    begin
      if Input[i] = H_B1 then
        Counter := Counter + 1
      else if (Input[i] = H_B2) and (Counter > 0) then
        Counter := Counter - 1
      else if (Input[i] = H_B2) and (Counter = 0) then
      begin
        Result := i;
        Break;
      end;
    end;
  end;

  function FunctionAtomize(Value: string; FunctionIndex: Integer): string;
  var
    V, Return: Extended;
  begin
    V := StrToFloat(Value);
    case FunctionIndex of
      1: Return := Arcsin(V); // Sinus [ dla radiana ]
      2: Return := Arccos(V); // Cosinus [ dla radiana ]
      3: Return := Arctan(V); // Tangens [ dla radiana ]
      4: Return := Arccot(V); // Cotangens [ dla radiana ]
      5: Return := Sin(V); // Arcus sinus [ dla radiana ]
      6: Return := Cos(V); // Arcus cosinus [ dla radiana ]
      7: Return := Tan(V); // Arcus tangens [ dla radiana ]
      8: Return := Cotan(V); // Arcus cotangens [ dla radiana ]
      9: Return := Ln(V); // Logarytm naturalny
      10: Return := Exp(V); // Exponent
      11: Return := sqrt(V); // Pierwiastek
      // Moøna dodaÊ nowe funkcje
    end;
    Result := FormatFloat(H_PRECISION, Return);
  end;

  function ValueAtomize(Value1, Value2: string; FunctionOperator: Char): string;
    var
      V1, V2, Return: Extended;
    begin
      V1 := StrToFloat(Value1);
      V2 := StrToFloat(Value2);
      case FunctionOperator of
        '+': Return := V1 + V2;
        '-': Return := V1 - V2;
        '*': Return := V1 * V2;
        '/': Return := V1 / V2;
        '^': Return := Power(V1, V2);
      end;
      Result := FormatFloat(H_PRECISION, Return);
    end;

  function SimplyCount(Input: string): string;
  var
    i: Integer;
    Value: string;
  begin
    while pos('--', Input) > 0 do
      Input := StringReplace(Input, '--', '+', []);
    for i := Low(MathFunc) to High(MathFunc) do
    begin
      while pos(MathFunc[i], Input) > 0 do
      begin
        Value := RightValue(Copy(Input, pos(MathFunc[i], Input) + length(MathFunc[i]), length(Input) - pos(MathFunc[i], Input) - length(MathFunc[i]) + 1));
        Input := StringReplace(Input, MathFunc[i] + Value, FunctionAtomize(Value, i), []); 
        while pos('--', Input) > 0 do
          Input := StringReplace(Input, '--', '+', []);
      end;
    Result := Input;
    end;
  end;

  function OnlyBasic(Input: string): string;
  var
    Value1, Value2: string;
    i: Integer;
  begin
    while pos('--', Input) > 0 do
      Input := StringReplace(Input, '--', '+', []);
    for i := Low(BasicOpr) to High(BasicOpr) do
      while pos(BasicOpr[i], Input) > 1 do
      begin
        Value1 := LeftValue(Copy(Input, 1, pos(BasicOpr[i], Input) - 1));
        Value2 := RightValue(Copy(Input, pos(BasicOpr[i], Input) + 1, length(Input) - pos(BasicOpr[i], Input)));
        Input := StringReplace(Input, Value1 + BasicOpr[i] + Value2, ValueAtomize(Value1, Value2, BasicOpr[i]), []);
        while pos('--', Input) > 0 do
          Input := StringReplace(Input, '--', '+', []);
      end;
    Result := FormatFloat(H_PRECISION, StrToFloat(Input));
  end;

  function AtomIntoValue(Input: string): string;
  begin
    while pos(H_B1, Input) > 0 do
      Input := StringReplace(Input, H_B1 + Copy(Input,  pos(H_B1, Input) + 1, GetBracket(Copy(Input, pos(H_B1, Input) + 1, length(Input) - pos(H_B1, Input))) - 1) + H_B2, AtomIntoValue(Copy(Input, pos(H_B1, Input) + 1, GetBracket(Copy(Input, pos(H_B1, Input) + 1, length(Input) - pos(H_B1, Input))) - 1)), []);
    Result := SimplyCount(Input);
    Result := OnlyBasic(Result);
  end;

begin
  Result := Optimize(Input); // Optymalizacja
  if Valid(Result) then // Walidacja
    Result := FormatFloat(H_RESULT, StrToFloat(AtomIntoValue(Result)));
end;

const
   MY_PI=3.14159265358979323846;
   MY_E=2.7182818285;

//sprawdzenie, czy podany ciπg jest liczbπ
function CzyLiczba(S : String) : Boolean;
var Blad: Integer;
    Wart: Double;
begin
  Val(S, Wart, Blad);
  Result := (Blad = 0);
  if Wart = 0 then exit;  
end;

//zamiana napisu na liczbÍ typu DOUBLE, z podmianπ kropki na przecinek
function STF(A : String) : Double;
var i : Integer;
begin
   for i:=1 to Length(A) do if A[i]='.' then A[i]:=',';
   Result:=StrToFloat(A);
end;

//zamiana liczby DOUBLE na napis, z podmianπ przecinka na kropkÍ
function FTS(A : Double) : String;
var i : Integer;
begin
   Result:=FloatToStr(A);
   for i:=1 to length(Result) do if Result[i]=',' then Result[i]:='.';
end;

function BIN_DODAWANIE(A,B : Double) : Double;
begin
   Result:=A+B;
end;

function BIN_ODEJMOWANIE(A,B : Double) : Double;
begin
   Result:=A-B;
end;

function BIN_MNOZENIE(A,B : Double) : Double;
begin
   Result:=A*B;
end;

function BIN_DZIELENIE(A,B : Double) : Double;
begin
   Result:=A/B;
end;

function BIN_SILNIA(A : Double) : Double;
var X,W,I : Integer;
begin
   X:=Round(A);
   W:=1;
   for i:=1 to X do W:=W*i;
   Result:=W;
end;

function BIN_SIN(A : Double) : Double;
begin
   Result:=Sin(A);
end;

function BIN_COS(A : Double) : Double;
begin
   Result:=Cos(A);
end;

function BIN_COSH(A : Double) : Double;
begin
   Result:=CosH(A);
end;

function BIN_SINH(A : Double) : Double;
begin
   Result:=SinH(A);
end;

function BIN_TGH(A : Double) : Double;
begin
   Result:=TanH(A);
end;

function BIN_CTGH(A : Double) : Double;
begin
   Result:=CotH(A);
end;

function BIN_ACOS(A : Double) : Double;
begin
   Result:=ArcCos(A);
end;

function BIN_ASIN(A : Double) : Double;
begin
   Result:=ArcSin(A);
end;

function BIN_ATG(A : Double) : Double;
begin
   Result:=ArcTan(A);
end;

function BIN_ACTG(A : Double) : Double;
begin
   Result:=(MY_PI/2)-ArcTan(A);
end;

function BIN_TG(A : Double) : Double;
begin
   Result:=Tan(A);
end;

function BIN_CTG(A : Double) : Double;
begin
   Result:=CoTan(A) ;
end;

function BIN_KOMBINACJE(K,N : Double) : Double;
begin
 Result:=BIN_SILNIA(N) / ( BIN_SILNIA(N-K)*BIN_SILNIA(K) );
end;

function BIN_ZMIENZNAK(A : Double) : Double;
begin
   Result:=-A;
end;

function BIN_STALAPI : Double;
begin
   Result:=MY_PI;
end;

function BIN_STALAE : Double;
begin
   Result:=MY_E;
end;

function BIN_SECANT(A : Double) : Double;
begin
   Result:=Secant(A);
end;

function BIN_COSECANT(A : Double) : Double;
begin
   Result:=CoSecant(A);
end;

function BIN_RADTODEG(A : Double) : Double;
begin
   Result:=RadToDeg(A);
end;

function BIN_DEGTORAD(A : Double) : Double;
begin
   Result:=DegToRad(A);
end;

function BIN_GRADTODEG(A : Double) : Double;
begin
   Result:=GradToDeg(A);
end;

function BIN_DEGTOGRAD(A : Double) : Double;
begin
   Result:=DegToGrad(A);
end;

function BIN_RADTOGRAD(A : Double) : Double;
begin
   Result:=RadToGrad(A);
end;

function BIN_GRADTORAD(A : Double) : Double;
begin
   Result:=GradToRad(A);
end;

function BIN_LOG2(A : Double) : Double;
begin
   Result:=Log2(A);
end;

function BIN_LOG10(A : Double) : Double;
begin
   Result:=Log10(A);
end;

function BIN_LOG(A,B : Double) : Double;
begin
   Result:=LogN(A,B);
end;

function BIN_LOGE(A : Double) : Double;
begin
   Result:=BIN_LOG10(A)/BIN_LOG10(MY_E);
end;

function BIN_POW(A,B : Double) : Double;
begin
   Result:=Power(A,B);
end;

function BIN_ABS(A : Double) : Double;
begin
   Result:=Abs(A);
end;

function BIN_SQRT(A : Double) : Double;
begin
  Result:=Sqrt(A);
end;

function BIN_SQR(A : Double) : Double;
begin
   Result:=Sqr(A);
end;

//============================== STOS ==========================================

constructor TStosDane.Create(S : String; I : Integer);
begin
    inherited Create;
    Nazwa:=S;
    Priorytet:=I;
end;

constructor TStos.Create;
begin
    inherited;
    FLista:=TList.Create;
end;

destructor TStos.Destroy;
begin
    Wyczysc;
    FLista.Free;
    inherited;
end;

procedure TStos.Dodaj(S : String; Priorytet : Integer = 0);
begin
  FLista.Add(TStosDane.Create(S,Priorytet));
end;

function TStos.Zdejmij : String;
begin
  if FLista.Count>0 then
  begin
      Result:=TStosDane(FLista.Last).Nazwa;
      TStosDane(FLista.Last).Free;
      FLista.Delete(FLista.Count-1);
  end else Result:='';
end;

function TStos.Istnieje : Boolean;
begin
  Result:=FLista.Count>0;
end;

function TStos.AktualnyPriorytet : Integer;
begin
  if FLista.Count>0 then Result:=TStosDane(FLista.Last).Priorytet
                    else Result:=0;
end;
procedure TStos.Wyczysc;
var i : Integer;
begin
  for i:=0 to FLista.Count-1 do TStosDane(FLista.Items[i]).Free;
  FLista.Clear;
end;

//================ F U N K C J E   i  Z M I E N N E ============================

constructor TZmienneDane.Create(N : String;W : Double);
begin
  Nazwa:=N;
  Wartosc:=W;
end;

constructor TFunkcjeDane.Create(N : String;P : Integer;D : Pointer;A : Integer);
begin
  Nazwa:=N;
  Priorytet:=P;
  Adres:=D;
  Argumentow:=A;
end;

//====================== O B L I C Z E N I A ===================================

constructor TObliczenia.Create;
begin
    inherited;

    //tworzymy niezbÍdne obiekty
    FZmienne:=TList.Create;
    FListaFunkcji:=TList.Create;
    FStosONP:=TStos.Create;
    FStosWynik:=TStos.Create;

    //zerujemy znacznik b≥Ídu
    FKodError:=0;

    //dodajemy obs≥ugÍ wbudowanych funkcji
    //jako parametry funkcji RejestrujFunkcje podajemy NAZW  funkcji, jej
    //PRIORYTET, liczbÍ ARGUMENT”W i ADRES

    //adres funkcji uzyskamy poprzedzajπc jej nazwÍ symbolem @

    //im wyøszy priorytet, tym szybciej dana operacja jest dokonywana
    //w naszym przypadku:
    // ZMIANA ZNAKU                - priorytet 20
    // R”ZNE FUNKCJE (sin, cos...) - priorytet 15
    // MNOZENIE I DZIELENIE        - priorytet 10
    // DODAWANIE I ODEJMOWANIE     - priorytet 5

    FunkcjeRejestruj('pi',30,0,@BIN_STALAPI);
    FunkcjeRejestruj('e',30,0,@BIN_STALAE);
    FunkcjeRejestruj('zmienznak',20,1,@BIN_ZMIENZNAK);
    FunkcjeRejestruj('+',5,2,@BIN_DODAWANIE);
    FunkcjeRejestruj('-',5,2,@BIN_ODEJMOWANIE);
    FunkcjeRejestruj('*',10,2,@BIN_MNOZENIE);
    FunkcjeRejestruj('/',10,2,@BIN_DZIELENIE);
    FunkcjeRejestruj('abs',15,1,@BIN_ABS);
    FunkcjeRejestruj('!',15,1,@BIN_SILNIA);
    FunkcjeRejestruj('sin',15,1,@BIN_SIN);
    FunkcjeRejestruj('cos',15,1,@BIN_COS);
    FunkcjeRejestruj('tan',15,1,@BIN_TG);
    FunkcjeRejestruj('tg',15,1,@BIN_TG);
    FunkcjeRejestruj('cotan',15,1,@BIN_CTG);
    FunkcjeRejestruj('cot',15,1,@BIN_CTG);
    FunkcjeRejestruj('ctg',15,1,@BIN_CTG);
    FunkcjeRejestruj('sinh',15,1,@BIN_SINH);
    FunkcjeRejestruj('cosh',15,1,@BIN_COSH);
    FunkcjeRejestruj('tanh',15,1,@BIN_TGH);
    FunkcjeRejestruj('tgh',15,1,@BIN_TGH);
    FunkcjeRejestruj('cotanh',15,1,@BIN_CTGH);
    FunkcjeRejestruj('coth',15,1,@BIN_CTGH);
    FunkcjeRejestruj('ctgh',15,1,@BIN_CTGH);
    FunkcjeRejestruj('secant',15,1,@BIN_SECANT);
    FunkcjeRejestruj('sec',15,1,@BIN_SECANT);
    FunkcjeRejestruj('cosecant',15,1,@BIN_COSECANT);
    FunkcjeRejestruj('csc',15,1,@BIN_COSECANT);
    FunkcjeRejestruj('arcsin',15,1,@BIN_ASIN);
    FunkcjeRejestruj('arccos',15,1,@BIN_ACOS);
    FunkcjeRejestruj('arctan',15,1,@BIN_ATG);
    FunkcjeRejestruj('arctg',15,1,@BIN_ATG);
    FunkcjeRejestruj('arccotan',15,1,@BIN_ACTG);
    FunkcjeRejestruj('arcctg',15,1,@BIN_ACTG);
    FunkcjeRejestruj('radtodeg',15,1,@BIN_RADTODEG);
    FunkcjeRejestruj('degtorad',15,1,@BIN_DEGTORAD);
    FunkcjeRejestruj('gradtodeg',15,1,@BIN_GRADTODEG);
    FunkcjeRejestruj('degtograd',15,1,@BIN_DEGTOGRAD);
    FunkcjeRejestruj('radtograd',15,1,@BIN_RADTOGRAD);
    FunkcjeRejestruj('gradtorad',15,1,@BIN_GRADTORAD);
    FunkcjeRejestruj('loge',15,1,@BIN_LOGE);
    FunkcjeRejestruj('ln',15,1,@BIN_LOGE);
    FunkcjeRejestruj('log2',15,1,@BIN_LOG2);
    FunkcjeRejestruj('log10',15,1,@BIN_LOG10);
    FunkcjeRejestruj('log',15,2,@BIN_LOG);
    FunkcjeRejestruj('power',15,2,@BIN_POW);
    FunkcjeRejestruj('^',15,2,@BIN_POW);
    FunkcjeRejestruj('combination',15,2,@BIN_KOMBINACJE);
    FunkcjeRejestruj('sqr',15,1,@BIN_SQR);
    FunkcjeRejestruj('sqrt',15,1,@BIN_SQRT);
end;

destructor TObliczenia.Destroy;
begin
    //zwalniamy pamiÍÊ
    FunkcjeWyczysc;
    ZmienneWyczysc;

    FZmienne.Free;
    FListaFunkcji.Free;

    FStosONP.Free;
    FStosWynik.Free;
    inherited;
end; 
procedure TObliczenia.ZmienneDodaj(N : String; W : Double);
var i : Integer;
begin

 //jak nie podano nazwy - b≥πd
 if (N='') then
 begin
   FKodError:=6;
   Exit;
 end;

 //zamieniamy wielkoúÊ liter w nazwie na ma≥e
 N:=lowercase(N);

 //sprawdzamy, czy w nazwie zmiennej sπ tylko dozwolone znaki (a..z i _)
 for i:=1 to length(N) do
  if not (N[i] in ['a'..'z','_']) then
  begin
     FKodError:=6;
     Exit;
  end;

 //nazwa jest poprawna, wiÍc...

 //szukamy zmiennej w bazie. Moøe juø jest, wtedy tylko jπ uaktualnimy.
 for i:=0 to FZmienne.Count-1 do
   if TZmienneDane(FZmienne.Items[i]).Nazwa=N then
   begin
       TZmienneDane(FZmienne.Items[i]).Wartosc:=W;
       exit;
   end;

 //to nowa zmienna, dodajemy
 FZmienne.Add(TZmienneDane.Create(N,W))
end;

procedure TObliczenia.ZmienneWyczysc;
var i : Integer;
begin
  for i:=0 to FZmienne.Count-1 do TZmienneDane(FZmienne.Items[i]).Free;
  FZmienne.Clear;
end;

procedure TObliczenia.FunkcjeRejestruj(Nazwa : String; Priorytet : Integer; Arg : Integer; Adres : Pointer);
begin
 FListaFunkcji.Add(TFunkcjeDane.Create(Nazwa,Priorytet,Adres,Arg));
end;

procedure TObliczenia.FunkcjeWyczysc;
var i : Integer;
begin
  for i:=0 to FListaFunkcji.Count-1 do
      TFunkcjeDane(FListaFunkcji.Items[i]).Free;
  FListaFunkcji.Clear;
end;

function TObliczenia.FunkcjeID(Nazwa : String) : Integer;
var i : Integer;
begin
    Result:=-1;
    Nazwa:=lowercase(Nazwa);
    for i:=0 to FListaFunkcji.Count-1 do
       if TFunkcjeDane(FListaFunkcji.Items[i]).Nazwa=Nazwa then
       begin
         Result:=i;
         Break;
       end;
end;

function TObliczenia.FunkcjeInfo(Id : Integer) : TFunkcjeDane;
begin
    if (Id>=0) and (Id<FListaFunkcji.Count) then
      Result:=TFunkcjeDane(FListaFunkcji.Items[Id]) else Result:=nil;
end;

//zwrÛcenie kodu b≥Ídu
function TObliczenia.Blad :Integer;
begin
  Result:=FKodError;
end;

//zwrÛcenie opisu b≥Ídu
function TObliczenia.BladOpis :String;
begin
  case FKodError of
  0 : Result:='';
  5 : Result:='Nieprawid≥owy zakres zmiennej.';
  6 : Result:='Nieprawid≥owa nazwa zmiennej.';
  8 : Result:='Nie znaleziono takiej zmiennej.';
  else Result:='Nieznany b≥πd.';
  end;
end;

//zwraca kod ONP dla wyraøenia
function TObliczenia.ONP : String;
begin
    Result:=FONP;
end;

procedure TObliczenia.Wyrazenie(S : String);
begin
    FWyrazenie:=lowercase(trim(S));
    FKodError:=0;
    GenerujONP;
end;

procedure TObliczenia.GenerujONP;
var Start,i      : Integer;
    Token        : String;
    Poprzedni    : String;
    Priorytet,ID : Integer;
begin
    Poprzedni:='';      //poprzedni Token
    FONP:='';           //poczπtkowa treúÊ kodu ONP

    FStosONP.Wyczysc;   //czyúcimy stos ONP


    Start:=1;
    while start<=length(FWyrazenie) do
    begin

      //pobierzmy pojedynczy token (ciπg znakÛw, liczbÍ, operator itp)
      for i:=Start to Length(FWyrazenie) do if not (FWyrazenie[i] in ['a'..'z','0'..'9','.','_']) then Break;
      if i=Start then Inc(i);
      Token:=Copy(FWyrazenie,Start,i-Start);
      Start:=i;

      //uøytkownik moøe uøywaÊ dowolnych nawiasÛw, dla nas one wszystkie to jedno: ( i )
      if (Token='[') or (Token='{') then Token:='(';
      if (Token=']') or (Token='}') then Token:=')';

      //mamy token - operacje na tokenie...


      //jeúli symbolem jest "-" - moøe to oznaczaÊ bπdü to ODJ•∆, bπdü teø
      //ZMIENZNAK. Kiedy wiÍc poprzedni Token by≥ pusty (poczπtek wyraøenia)
      //lub poprzedni Token to "(" - czyli poczπtek wyraøenia w nawiasie,
      //jest to ZMIENZNAK!
      if (Token='-') and
         ( (Poprzedni='') or (Poprzedni='(') ) then Token:='zmienznak';


      //jeúli nasz Token to LICZBA, doklejamy jπ do wyniku ONP
      If CzyLiczba(Token) then FONP:=FONP+Token+' '
      else if Token='(' then
      begin
          //otwieramy nawias
          FStosONP.Dodaj('(',0);  //dodajemy na stos z priorytetem 0 (zarezerwowany)

      end else if Token=')' then
      begin
          //koniec nawiasu

          //zdejmujemy najpierw ze stosu wszystko do czasu napotkania elementu
          //o priorytecie 0, czyli poczπtku nawiasu
          while (FStosONP.Istnieje) and (FStosONP.AktualnyPriorytet<>0) do
              FONP:=FONP+FStosONP.Zdejmij+' ';

          //sam nawias takøe zdejmijmy
          FStosONP.Zdejmij;

      end else if FunkcjeID(Token)=-1 then
      begin
        //to nie øadna funkcja (pewnie zmienna), doklejmy do ONP
        //nie doklejamy jednak znaku przecinka, ktÛry oddziela argumenty
        if Token<>',' then FONP:=FONP+Token+' '
      end else
      begin
          //to funkcja - pobierzmy jej numer
          Id:=FunkcjeID(Token);

          //najpierw zdejmujemy ze stosu wszystko o WYØSZYCH PRIORYTETACH
          Priorytet:=FunkcjeInfo(Id).Priorytet;
          while (FStosONP.Istnieje) and (FStosONP.AktualnyPriorytet>=Priorytet) do
              FONP:=FONP+FStosONP.Zdejmij+' ';

          //i teraz na stos naszπ funkcjÍ
          FStosONP.Dodaj(Token,Priorytet);
      end;

      //zapamiÍtujemy token jako "Poprzedni"
      Poprzedni:=Token;
    end;

    //wszystko co zosta≥o na stosie zdejmujemy
    while (FStosONP.Istnieje) do
              FONP:=FONP+FStosONP.Zdejmij+' ';

end;

function TObliczenia.Wynik : Double;
var Start, i,k,id : Integer;
    Token     : String;
    A,B,C,D   : String;
    Adres     : Pointer;
    W         : Double;
begin
    //jak nie mamy ONP - koniec
    if FONP='' then
    begin
      Result:=0;
      exit;
    end;

    //czyúcimy stos wyniku i dodajemy jako wynik 0
    //priorytet nie jest istotny - wiÍc go nie podajemy
    FStosWynik.Wyczysc;
    FStosWynik.Dodaj('0');

    //zerujemy kod b≥Ídu
    FKodError:=0;

    Start:=1;
    while start<=length(FONP) do
    begin

      //pobierzmy pojedynczy token (ciπg znakÛw, liczbÍ, operator itp)
      for i:=Start to Length(FONP) do if (FONP[i] in [' ']) then Break;
      if i=Start then Inc(i);
      Token:=Copy(FONP,Start,i-Start);
      Start:=i;

      //wytnijmy bia≥e znaki z koÒcÛw tego tokena
      Token:=trim(Token);
      if Token='' then Continue;

      //jeúli to liczba - to na stos
      If CzyLiczba(Token) then FStosWynik.Dodaj(Token)
      else if FunkcjeID(Token)=-1 then
      begin
        //jeúli to nie funkcja, to pewnie zmienna...
         k:=0;
         for i:=0 to FZmienne.Count-1 do
           if TZmienneDane(FZmienne.Items[i]).Nazwa=Token then 
           begin 
             k:=1;
             break;
           end;
         if k=1 then FStosWynik.Dodaj(FTS(TZmienneDane(FZmienne.Items[i]).Wartosc)) else FKodError:=8;
      end else
       begin
          //to funkcja, wiÍc pobierzmy jej numer i adres
          Id:=FunkcjeID(Token);
          Adres:=FunkcjeInfo(Id).Adres;

          //a teraz w zaleønoúci od iloúci argumentÛw...
          case FunkcjeInfo(ID).Argumentow of
           0: begin
               try
                 W:=TFunkcjaObliczen0arg(Adres);
                 FStosWynik.Dodaj(FTS(W),0);
               except
                 FKodError:=5;
               end;
              end;
           1: begin
               try
                 A:=FStosWynik.Zdejmij;
                 W:=TFunkcjaObliczen1arg(Adres)(STF(A));
                 FStosWynik.Dodaj(FTS(W),0);
               except
                 FKodError:=5;
               end;
              end;
           2: begin
               try
                 B:=FStosWynik.Zdejmij;
                 if FStosWynik.Istnieje then A:=FStosWynik.Zdejmij else A:='0';
                 W:=TFunkcjaObliczen2arg(Adres)(STF(A),STF(B));
                 FStosWynik.Dodaj(FTS(W),0);
               except
                 FKodError:=5;
               end;
              end;
           3: begin
               try
                 C:=FStosWynik.Zdejmij;
                 if FStosWynik.Istnieje then B:=FStosWynik.Zdejmij else B:='0';
                 if FStosWynik.Istnieje then A:=FStosWynik.Zdejmij else A:='0';
                 W:=TFunkcjaObliczen3arg(Adres)(STF(A),STF(B),STF(C));
                 FStosWynik.Dodaj(FTS(W),0);
               except
                 FKodError:=5;
               end;
              end;
           4: begin
               try
                 D:=FStosWynik.Zdejmij;
                 if FStosWynik.Istnieje then C:=FStosWynik.Zdejmij else C:='0';
                 if FStosWynik.Istnieje then B:=FStosWynik.Zdejmij else B:='0';
                 if FStosWynik.Istnieje then A:=FStosWynik.Zdejmij else A:='0';

                 W:=TFunkcjaObliczen4arg(Adres)(STF(A),STF(B),STF(C),STF(D));
                 FStosWynik.Dodaj(FTS(W),0);
               except
                 FKodError:=5;
               end;

              end;
          end;

      end;

    end;

      if FKodError=0 then Result:=STF(FStosWynik.Zdejmij)
                  else Result:=0;

end;




end.

