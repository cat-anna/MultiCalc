object FFncEdt: TFFncEdt
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Funkcje'
  ClientHeight = 343
  ClientWidth = 343
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object bModify: TButton
    Left = 8
    Top = 311
    Width = 123
    Height = 25
    Caption = 'Dodaj/Modyfikuj'
    TabOrder = 0
    OnClick = bModifyClick
  end
  object lbFncs: TListBox
    Left = 8
    Top = 8
    Width = 121
    Height = 297
    Style = lbOwnerDrawFixed
    ItemHeight = 16
    TabOrder = 1
    OnClick = lbFncsClick
    OnDrawItem = lbFncsDrawItem
  end
  object GroupBox1: TGroupBox
    Left = 135
    Top = 8
    Width = 202
    Height = 297
    Caption = 'Funkcja'
    TabOrder = 2
    object leName: TLabeledEdit
      Left = 8
      Top = 32
      Width = 121
      Height = 21
      EditLabel.Width = 32
      EditLabel.Height = 13
      EditLabel.Caption = 'Nazwa'
      TabOrder = 0
    end
    object GroupBox2: TGroupBox
      Left = 3
      Top = 59
      Width = 190
      Height = 230
      Caption = 'Typ funkcji'
      TabOrder = 1
      object lParCount: TLabel
        Left = 3
        Top = 214
        Width = 96
        Height = 13
        Caption = 'Ilo'#347#263' parametr'#243'w: 0'
      end
      object rbwew: TRadioButton
        Left = 3
        Top = 16
        Width = 113
        Height = 17
        Caption = 'Wewn'#281'trzna'
        Checked = True
        Enabled = False
        TabOrder = 0
        TabStop = True
        OnClick = rbwewClick
      end
      object rbwers: TRadioButton
        Left = 3
        Top = 39
        Width = 113
        Height = 17
        Caption = 'Wersetowa'
        TabOrder = 1
        OnClick = rbwersClick
      end
      object GroupBox3: TGroupBox
        Left = 3
        Top = 53
        Width = 183
        Height = 148
        Caption = 'Funkcja wersetowa'
        TabOrder = 2
        object Label1: TLabel
          Left = 13
          Top = 73
          Width = 156
          Height = 72
          AutoSize = False
          Caption = 
            'Aby wstawi'#263' parametr wpisz "[n]" gdzie n oznacza jego numer (n l' +
            'icz od 0)'#13#10'ilo'#347#263' parametr'#243'w jest rozpoznawana automatycznie'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
        object leData: TLabeledEdit
          Left = 13
          Top = 35
          Width = 161
          Height = 21
          EditLabel.Width = 51
          EditLabel.Height = 13
          EditLabel.Caption = 'Wyra'#380'enie'
          TabOrder = 0
          OnChange = leDataChange
        end
      end
    end
  end
  object bUsun: TButton
    Left = 135
    Top = 311
    Width = 75
    Height = 25
    Caption = 'Usu'#324
    TabOrder = 3
    OnClick = bUsunClick
  end
  object Button2: TButton
    Left = 262
    Top = 311
    Width = 75
    Height = 25
    Caption = 'Zamknij'
    ModalResult = 1
    TabOrder = 4
  end
end
