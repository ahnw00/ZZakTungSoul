# v3 helpers: sparse hero layouts
function Base($bg){
  $s = BlankSlide $bg
  $null = CorrBand $s 0 0 960 12
  $ln0 = $s.Shapes.AddLine(0,12,960,12); $ln0.Line.ForeColor.RGB = $global:INK; $ln0.Line.Weight = 2
  $null = AddT $s 'KusoGames  ·  ZZakTungSoul' 36 514 300 16 10 $false $global:MUTE 1 3
  return $s
}

function Head($s,[string]$num,[string]$section,[string]$headline,$hsz=36,$hw=880){
  $null = Chip $s $num 40 28 50 32 $global:ORG $global:INK 16 5 2
  $null = AddT $s $section 100 28 780 32 14 $true $global:DKRAFT 1 3
  $h = AddT $s $headline 40 76 $hw ([int][Math]::Ceiling($hsz*2.7)) $hsz $true $global:INK 1 1
  $h.TextFrame.TextRange.ParagraphFormat.SpaceWithin = 1.05
  return $h
}

function Dot($s,$cx,$cy,$d,$fill,$lw=3){
  $o = $s.Shapes.AddShape(9,($cx-$d/2),($cy-$d/2),$d,$d)
  $o.Fill.Solid(); $o.Fill.ForeColor.RGB = $fill
  $o.Line.ForeColor.RGB = $global:INK; $o.Line.Weight = [single]$lw; $o.Shadow.Visible = 0
  return $o
}

function Thick($s,$x1,$y1,$x2,$y2,$col,$wt,$dash=1){
  $l = $s.Shapes.AddLine($x1,$y1,$x2,$y2)
  $l.Line.ForeColor.RGB = $col; $l.Line.Weight = [single]$wt; $l.Line.DashStyle = $dash
  return $l
}

function FramePic($s,[string]$file,$x,$y,$w,$h){
  $sd = $s.Shapes.AddShape(1,($x+6),($y+6),$w,$h); $sd.Fill.Solid(); $sd.Fill.ForeColor.RGB = $global:INK; $sd.Line.Visible = 0; $sd.Shadow.Visible = 0
  $p = AddPic $s $file $x $y $w $h
  $p.Line.Visible = -1; $p.Line.ForeColor.RGB = $global:INK; $p.Line.Weight = 3
  return $p
}

# three big nodes in a row with arrows (selected-system slides)
function Chain3($s,$items,$y=270,$h=124){
  for($i=0;$i -lt 3;$i++){
    $x = 50 + $i*310
    $f = $(if($i -eq 1){$global:LORG}else{$global:ORG})
    $null = Node2 $s $items[$i][0] $items[$i][1] $x $y 260 $h $f $global:INK 26 15
    if ($i -lt 2) { $null = AddArrow $s ($x+264) ($y+$h/2) ($x+306) ($y+$h/2) $global:INK 4 }
  }
}

function Bottom($s,[string]$txt,$y=430){
  $t = AddT $s $txt 40 $y 880 40 20 $true $global:INK 2 3
  return $t
}
