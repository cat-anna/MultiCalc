object FChartPropertis: TFChartPropertis
  Left = 0
  Top = 0
  Caption = 'W'#322'a'#347'ciwo'#347'ci wykresu'
  ClientHeight = 264
  ClientWidth = 185
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox2: TGroupBox
    Left = 8
    Top = 0
    Width = 169
    Height = 225
    Caption = 'Opcje wykresu'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 0
    object Label6: TLabel
      Left = 12
      Top = 55
      Width = 91
      Height = 13
      Caption = 'O'#347' odci'#281'tych (OX):'
    end
    object LabelOY: TLabel
      Left = 12
      Top = 78
      Width = 89
      Height = 13
      Caption = 'O'#347' rz'#281'dnych (OY):'
    end
    object lParamName: TLabel
      Left = 12
      Top = 165
      Width = 91
      Height = 13
      Caption = 'Zmienny parametr:'
    end
    object lTimeStep: TLabel
      Left = 34
      Top = 193
      Width = 69
      Height = 13
      Caption = 'Krok czasowy:'
      WordWrap = True
    end
    object lParam: TLabel
      Left = 109
      Top = 165
      Width = 32
      Height = 13
      Caption = 'lParam'
    end
    object rgChartType: TRadioGroup
      Left = 3
      Top = 16
      Width = 163
      Height = 33
      Caption = 'Typ'
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        '2D'
        '3D')
      TabOrder = 0
      OnClick = rgChartTypeClick
    end
    object cbOX: TComboBox
      Left = 107
      Top = 51
      Width = 57
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 1
    end
    object cbOY: TComboBox
      Left = 107
      Top = 74
      Width = 57
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
    end
    object cbAnimateChart: TCheckBox
      Left = 14
      Top = 142
      Width = 149
      Height = 17
      Caption = 'Animacja wykresu w czasie'
      TabOrder = 3
      OnClick = cbAnimateChartClick
    end
    object cbShowNet: TCheckBox
      Left = 14
      Top = 121
      Width = 97
      Height = 17
      Caption = 'Poka'#380' siatk'#281
      TabOrder = 4
    end
    object fseTimeStep: TCSpinEdit
      Left = 109
      Top = 188
      Width = 46
      Height = 22
      TabOrder = 5
    end
    object pLineColor: TPanel
      Left = 16
      Top = 96
      Width = 137
      Height = 17
      Caption = 'kolor lini'
      TabOrder = 6
      OnClick = pLineColorClick
    end
  end
  object Button1: TButton
    Left = 20
    Top = 231
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 101
    Top = 231
    Width = 75
    Height = 25
    Caption = 'Anuluj'
    ModalResult = 2
    TabOrder = 2
  end
  object ColorDialog1: TColorDialog
    Top = 208
  end
end
