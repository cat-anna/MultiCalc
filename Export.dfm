object FExport: TFExport
  Left = 0
  Top = 0
  Caption = 'Eksportuj do pliku Excel'
  ClientHeight = 225
  ClientWidth = 271
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 5
    Width = 25
    Height = 13
    Caption = 'Wz'#243'r'
  end
  object Label2: TLabel
    Left = 8
    Top = 207
    Width = 66
    Height = 13
    Caption = 'Ilo'#347#263' kobinacji'
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 24
    Width = 169
    Height = 177
    Caption = 'Zmienne'
    TabOrder = 0
    object lbVaribles: TListBox
      Left = 10
      Top = 22
      Width = 63
      Height = 143
      ItemHeight = 13
      TabOrder = 0
      OnClick = lbVariblesClick
    end
    object GroupBox2: TGroupBox
      Left = 79
      Top = 22
      Width = 75
      Height = 143
      Caption = 'Zakres'
      TabOrder = 1
      object Label3: TLabel
        Left = 7
        Top = 18
        Width = 14
        Height = 13
        Caption = 'Od'
      end
      object Label4: TLabel
        Left = 7
        Top = 61
        Width = 13
        Height = 13
        Caption = 'Do'
      end
      object Label5: TLabel
        Left = 7
        Top = 101
        Width = 21
        Height = 13
        Caption = 'Krok'
      end
      object lseVarFrom: TFloatSpinEdit
        Tag = 1
        Left = 7
        Top = 33
        Width = 59
        Height = 22
        Increment = 0.100000001490116100
        MaxValue = 0
        MinValue = 0
        TabOrder = 0
        Precision = 3
        OnChange = fseDeltaChange
      end
      object lseVarTo: TFloatSpinEdit
        Tag = 2
        Left = 7
        Top = 75
        Width = 59
        Height = 22
        Increment = 0.100000001490116100
        MaxValue = 0
        MinValue = 0
        TabOrder = 1
        Precision = 3
        OnChange = fseDeltaChange
      end
      object fseDelta: TFloatSpinEdit
        Tag = 2
        Left = 7
        Top = 114
        Width = 59
        Height = 22
        Increment = 0.100000001490116100
        MaxValue = 0
        MinValue = 0
        TabOrder = 2
        Precision = 3
        OnChange = fseDeltaChange
      end
    end
  end
  object Button1: TButton
    Left = 188
    Top = 24
    Width = 75
    Height = 25
    Caption = 'Eksportuj'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 190
    Top = 55
    Width = 75
    Height = 25
    Caption = 'Anuluj'
    ModalResult = 2
    TabOrder = 2
  end
  object SaveDialog: TSaveDialog
    Filter = 'Plik programu Excel|*.xls'
    Left = 88
  end
end
