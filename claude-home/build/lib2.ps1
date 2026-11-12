$ErrorActionPreference = 'Stop'
$global:SP   = $PSScriptRoot
$global:IMG  = Join-Path $global:SP 'img'
$global:Warns = New-Object System.Collections.ArrayList

function Rgb([int]$r,[int]$g,[int]$b){ return ($r + 256*$g + 65536*$b) }
# KusoGames brand: logo orange + black outline, cardboard kraft / cream
$global:INK   = Rgb 34 28 24
$global:ORG   = Rgb 244 158 12
$global:LORG  = Rgb 253 228 170
$global:CREAM = Rgb 247 238 218
$global:PAPER = Rgb 255 252 244
$global:ZEBRA = Rgb 251 243 224
$global:KRAFT = Rgb 205 160 105
$global:DKRAFT= Rgb 150 104 58
$global:TAPE  = Rgb 230 200 140
$global:MUTE  = Rgb 112 94 78
$global:RED   = Rgb 214 58 38
$global:GRN   = Rgb 64 150 90
$global:BLU   = Rgb 58 116 196
$global:PUR   = Rgb 134 92 178
$global:WHITE = Rgb 255 255 255
$global:LGRAY = Rgb 236 230 218
$global:FONT  = [string][char]0xB9D1 + [char]0xC740 + ' ' + [char]0xACE0 + [char]0xB515   # malgun gothic

function AddT($s,[string]$txt,$x,$y,$w,$h,$sz=14,$bold=$false,$col=$null,$al=1,$va=1){
  if ($col -eq $null) { $col = $global:INK }
  $sh = $s.Shapes.AddTextbox(1,$x,$y,$w,$h)
  $tf = $sh.TextFrame; $tf.WordWrap = -1; $tf.AutoSize = 0; $sh.Height = $h
  $tf.MarginLeft=3; $tf.MarginRight=3; $tf.MarginTop=2; $tf.MarginBottom=2
  $tf.VerticalAnchor = $va
  $tr = $tf.TextRange; $tr.Text = $txt
  $tr.Font.Name = $global:FONT; $tr.Font.NameFarEast = $global:FONT; $tr.Font.Size = $sz
  $tr.Font.Bold = $(if($bold){-1}else{0}); $tr.Font.Color.RGB = $col
  $tr.ParagraphFormat.Alignment = $al
  if ($tr.BoundHeight -gt ($h+3)) {
    $p = $txt.Substring(0,[Math]::Min(18,$txt.Length)) -replace "[\r\n\v]",' '
    [void]$global:Warns.Add(("OVERFLOW s{0} '{1}' need {2:N0} have {3:N0}" -f $s.SlideIndex,$p,$tr.BoundHeight,$h))
  }
  return $sh
}

function AddBul($s,[string[]]$items,$x,$y,$w,$h,$sz=14,$col=$null,$gap=6){
  if ($col -eq $null) { $col = $global:INK }
  $sh = AddT $s ($items -join "`r") $x $y $w $h $sz $false $col 1 1
  $tr = $sh.TextFrame.TextRange
  $tr.ParagraphFormat.Bullet.Visible = -1
  $tr.ParagraphFormat.Bullet.Character = 9632
  $tr.ParagraphFormat.Bullet.RelativeSize = 0.6
  $tr.ParagraphFormat.Bullet.Font.Color.RGB = $global:ORG
  $tr.ParagraphFormat.LineRuleAfter = 0; $tr.ParagraphFormat.SpaceAfter = $gap
  $sh.TextFrame.Ruler.Levels(1).FirstMargin = 0; $sh.TextFrame.Ruler.Levels(1).LeftMargin = 14
  if ($tr.BoundHeight -gt ($h+3)) {
    [void]$global:Warns.Add(("OVERFLOW(b) s{0} '{1}' need {2:N0} have {3:N0}" -f $s.SlideIndex,($items[0].Substring(0,[Math]::Min(14,$items[0].Length))),$tr.BoundHeight,$h))
  }
  return $sh
}

function Em($sh,[string]$sub,$col,$bold=$true){
  $f = $sh.TextFrame.TextRange.Find($sub)
  if ($f -ne $null) { $f.Font.Color.RGB = $col; $f.Font.Bold = $(if($bold){-1}else{0}) }
}

# sticker card: black outline + hard offset shadow
function Card($s,$x,$y,$w,$h,$fill=$null,$off=4,$rad=10){
  if ($fill -eq $null) { $fill = $global:PAPER }
  $adj = [Math]::Min(0.5, $rad / [Math]::Min($w,$h))
  if ($off -gt 0) {
    $sd = $s.Shapes.AddShape(5,($x+$off),($y+$off),$w,$h); $sd.Adjustments.Item(1) = $adj
    $sd.Fill.Solid(); $sd.Fill.ForeColor.RGB = $global:INK; $sd.Line.Visible = 0; $sd.Shadow.Visible = 0
  }
  $sh = $s.Shapes.AddShape(5,$x,$y,$w,$h); $sh.Adjustments.Item(1) = $adj
  $sh.Fill.Solid(); $sh.Fill.ForeColor.RGB = $fill
  $sh.Line.ForeColor.RGB = $global:INK; $sh.Line.Weight = 2; $sh.Shadow.Visible = 0
  return $sh
}

# label / chip with text (outlined)
function Chip($s,[string]$txt,$x,$y,$w,$h,$fill,$col,$sz=14,$shape=5,$lw=2){
  $sh = $s.Shapes.AddShape($shape,$x,$y,$w,$h)
  if ($shape -eq 5) { $sh.Adjustments.Item(1) = [Math]::Min(0.5, 8 / [Math]::Min($w,$h)) }
  $sh.Fill.Solid(); $sh.Fill.ForeColor.RGB = $fill
  if ($lw -le 0) { $sh.Line.Visible = 0 } else { $sh.Line.ForeColor.RGB = $global:INK; $sh.Line.Weight = [single]$lw }
  $sh.Shadow.Visible = 0
  $tf = $sh.TextFrame; $tf.WordWrap = -1; $tf.AutoSize = 0
  $tf.MarginLeft=4; $tf.MarginRight=4; $tf.MarginTop=1; $tf.MarginBottom=1; $tf.VerticalAnchor = 3
  $tr = $tf.TextRange; $tr.Text = $txt
  $tr.Font.Name = $global:FONT; $tr.Font.NameFarEast = $global:FONT; $tr.Font.Size = $sz
  $tr.Font.Bold = -1; $tr.Font.Color.RGB = $col; $tr.ParagraphFormat.Alignment = 2
  if ($tr.BoundHeight -gt ($h+3) -or $tr.BoundWidth -gt ($w)) {
    $p = $txt.Substring(0,[Math]::Min(14,$txt.Length)) -replace "[\r\n\v]",' '
    [void]$global:Warns.Add(("OVERFLOW(chip) s{0} '{1}' need {2:N0}x{3:N0} have {4:N0}x{5:N0}" -f $s.SlideIndex,$p,$tr.BoundWidth,$tr.BoundHeight,$w,$h))
  }
  return $sh
}

# two-line node: first line bold big, second line small
function Node2($s,[string]$t1,[string]$t2,$x,$y,$w,$h,$fill,$col,$sz1=15,$sz2=11){
  $sh = Chip $s ($t1 + "`r" + $t2) $x $y $w $h $fill $col $sz1
  $tr = $sh.TextFrame.TextRange
  $p2 = $tr.Paragraphs(2); $p2.Font.Size = $sz2; $p2.Font.Bold = 0
  return $sh
}

function Tape($s,$x,$y,$w=54,$h=16,$rot=-6){
  $sh = $s.Shapes.AddShape(1,$x,$y,$w,$h)
  $sh.Fill.Solid(); $sh.Fill.ForeColor.RGB = $global:TAPE; $sh.Fill.Transparency = 0.15
  $sh.Line.Visible = 0; $sh.Shadow.Visible = 0; try { $sh.Rotation = [single]$rot } catch { [void]$global:Warns.Add('ROT failed') }
  return $sh
}

function AddArrow($s,$x1,$y1,$x2,$y2,$col=$null,$wt=3){
  if ($col -eq $null) { $col = $global:INK }
  $ln = $s.Shapes.AddLine($x1,$y1,$x2,$y2)
  try { $ln.Line.ForeColor.RGB = [int]$col } catch {}
  try { $ln.Line.Weight = [int][Math]::Ceiling([double]$wt) } catch { try { $ln.Line.Weight = [single]$wt } catch {} }
  try { $ln.Line.EndArrowheadStyle = 2 } catch {}
  return $ln
}

function AddTbl($s,$data,$x,$y,$colW,$rowH,$sz=14,$aligns=$null,$hdrH=32,$boldFirst=$true){
  $rows = $data.Count; $cols = $colW.Count
  $tw = 0; foreach($cw in $colW){ $tw += $cw }
  $sh = $s.Shapes.AddTable($rows,$cols,$x,$y,$tw,($rowH*$rows))
  $t = $sh.Table
  try { $t.ApplyStyle('{2D5ABB26-0587-4C30-8999-92F81FD0307C}',$false) } catch {}
  for($c=1;$c -le $cols;$c++){ $t.Columns($c).Width = $colW[$c-1] }
  for($r=1;$r -le $rows;$r++){
    for($c=1;$c -le $cols;$c++){
      $cell = $t.Cell($r,$c); $tf = $cell.Shape.TextFrame; $tr = $tf.TextRange
      $tr.Text = [string]$data[$r-1][$c-1]
      $tr.Font.Name = $global:FONT; $tr.Font.NameFarEast = $global:FONT; $tr.Font.Size = $sz
      $tf.MarginLeft = 8; $tf.MarginRight = 8; $tf.MarginTop = 3; $tf.MarginBottom = 3; $tf.VerticalAnchor = 3
      $al = 1; if ($aligns -ne $null) { $al = $aligns[$c-1] }
      $tr.ParagraphFormat.Alignment = $al
      $cell.Shape.Fill.Solid()
      if ($r -eq 1) {
        $cell.Shape.Fill.ForeColor.RGB = $global:ORG
        $tr.Font.Bold = -1; $tr.Font.Color.RGB = $global:INK; $tr.ParagraphFormat.Alignment = 2
      } else {
        if ($r % 2 -eq 1) { $cell.Shape.Fill.ForeColor.RGB = $global:ZEBRA } else { $cell.Shape.Fill.ForeColor.RGB = $global:PAPER }
        $tr.Font.Color.RGB = $global:INK
        $tr.Font.Bold = $(if($c -eq 1 -and $boldFirst){-1}else{0})
        $txt = [string]$data[$r-1][$c-1]
        if ($txt -eq [string][char]0x2713) { $tr.Font.Color.RGB = $global:GRN; $tr.Font.Bold = -1; $tr.Font.Size = $sz+4 }
        elseif ($txt -eq [string][char]0x25B3) { $tr.Font.Color.RGB = (Rgb 214 120 0); $tr.Font.Bold = -1; $tr.Font.Size = $sz+4 }
        elseif ($txt -eq [string][char]0x2717) { $tr.Font.Color.RGB = $global:RED; $tr.Font.Bold = -1; $tr.Font.Size = $sz+4 }
      }
      foreach($bi in 1,2,3,4){ $b = $cell.Borders($bi); $b.Visible = -1; $b.ForeColor.RGB = $global:INK; $b.Weight = 1.25 }
    }
    $t.Rows($r).Height = $(if($r -eq 1){$hdrH}else{$rowH})
  }
  if (($sh.Top + $sh.Height) -gt 504) {
    [void]$global:Warns.Add(("TABLE-LOW s{0}: bottom={1:N0}" -f $s.SlideIndex,($sh.Top+$sh.Height)))
  }
  return $sh
}

function AddPic($s,[string]$file,$x,$y,$w=-1,$h=-1){
  return $s.Shapes.AddPicture((Join-Path $global:IMG $file),0,-1,$x,$y,$w,$h)
}

function SetNotes($s,[string[]]$lines){
  $s.NotesPage.Shapes.Placeholders(2).TextFrame.TextRange.Text = ($lines -join "`r")
}

# blank slide cloned from template slide 12; only the footer logo group (Group 4) is kept
function BlankSlide($bg){
  $dup = $global:pres.Slides(12).Duplicate()
  $s = $dup.Item(1); $s.MoveTo($global:pres.Slides.Count)
  for($i=$s.Shapes.Count;$i -ge 1;$i--){ if($s.Shapes($i).Name -ne 'Group 4'){ $s.Shapes($i).Delete() } }
  $s.FollowMasterBackground = 0
  $s.Background.Fill.Solid(); $s.Background.Fill.ForeColor.RGB = $bg
  return $s
}

function CorrBand($s,$x,$y,$w,$h){
  $b = $s.Shapes.AddShape(1,$x,$y,$w,$h)
  $b.Fill.Patterned(20); $b.Fill.ForeColor.RGB = $global:DKRAFT; $b.Fill.BackColor.RGB = $global:KRAFT
  $b.Line.Visible = 0; $b.Shadow.Visible = 0
  return $b
}

function NewSlide2($num,[string]$section,[string]$title,[string]$msg,$nsz=28){
  $s = BlankSlide $global:CREAM
  $null = CorrBand $s 0 0 960 12
  $ln0 = $s.Shapes.AddLine(0,12,960,12); $ln0.Line.ForeColor.RGB = $global:INK; $ln0.Line.Weight = 2
  # number stamp
  $sd = $s.Shapes.AddShape(5,39,33,58,58); $sd.Adjustments.Item(1) = 0.18; $sd.Fill.Solid(); $sd.Fill.ForeColor.RGB = $global:INK; $sd.Line.Visible = 0
  $st = Chip $s $num 36 30 58 58 $global:ORG $global:INK $nsz 5 2.5
  $null = Tape $s 22 24 34 13 -35
  $null = AddT $s $section 112 24 640 20 12 $true $global:DKRAFT 1 3
  $null = AddT $s $title 112 42 800 40 28 $true $global:INK 1 3
  $mt = AddT $s $msg 112 82 800 24 15 $false $global:MUTE 1 3
  $dl = $s.Shapes.AddLine(36,116,924,116); $dl.Line.ForeColor.RGB = $global:DKRAFT; $dl.Line.Weight = 1.5; $dl.Line.DashStyle = 4
  $null = AddT $s 'KusoGames  ·  ZZakTungSoul' 36 514 300 16 10 $false $global:MUTE 1 3
  return $s
}

function OpenDeck {
  $work = $global:SP
  $global:app = New-Object -ComObject PowerPoint.Application
  $global:pres = $global:app.Presentations.Open((Join-Path $work 'template.pptx'), $false, $false, $false)
}
