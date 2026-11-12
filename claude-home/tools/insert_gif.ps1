# insert_gif.ps1
# Replaces the dashed placeholder shape "GIF_PLACEHOLDER" (and its label) in a deck with a GIF/picture.
# Usage (PowerShell 5.1, PowerPoint must be installed):
#   powershell -NoProfile -ExecutionPolicy Bypass -File insert_gif.ps1 -Pptx ..\deck\ZZakTungSoul_v3.pptx -Gif C:\path\mocap.gif
# Optional: -Out <new pptx path>   (default: <name>_gif.pptx next to the input)
# The animated GIF stays animated in slideshow mode. The picture is fitted inside the placeholder box.
param(
  [Parameter(Mandatory=$true)][string]$Pptx,
  [Parameter(Mandatory=$true)][string]$Gif,
  [string]$Out = ''
)
$ErrorActionPreference = 'Stop'
$Pptx = (Resolve-Path $Pptx).Path
$Gif  = (Resolve-Path $Gif).Path
if ($Out -eq '') {
  $Out = Join-Path ([IO.Path]::GetDirectoryName($Pptx)) ([IO.Path]::GetFileNameWithoutExtension($Pptx) + '_gif.pptx')
}
$INK = 34 + 256*28 + 65536*24

$app = New-Object -ComObject PowerPoint.Application
$pres = $app.Presentations.Open($Pptx, $false, $false, $false)
try {
  $slide = $null; $card = $null
  foreach ($s in $pres.Slides) {
    foreach ($sh in $s.Shapes) { if ($sh.Name -eq 'GIF_PLACEHOLDER') { $slide = $s; $card = $sh } }
  }
  if ($card -eq $null) { throw 'GIF_PLACEHOLDER shape not found (already replaced?)' }

  $x = $card.Left; $y = $card.Top; $w = $card.Width; $h = $card.Height
  $z = $card.ZOrderPosition
  # remove label, the offset shadow rectangle behind the card, and the card itself
  foreach ($sh in @($slide.Shapes)) { if ($sh.Name -eq 'GIF_PLACEHOLDER_LABEL') { $sh.Delete() } }
  $shadow = $null
  if ($z -gt 1) { $cand = $slide.Shapes($z - 1); if ([Math]::Abs($cand.Width - $w) -lt 1 -and [Math]::Abs($cand.Height - $h) -lt 1) { $shadow = $cand } }
  $card.Delete()
  if ($shadow -ne $null) { $shadow.Delete() }

  # natural size -> fit inside the box
  $pic = $slide.Shapes.AddPicture($Gif, 0, -1, $x, $y, -1, -1)
  $pic.LockAspectRatio = -1
  $scale = [Math]::Min($w / $pic.Width, $h / $pic.Height)
  $pic.Width = $pic.Width * $scale
  $pic.Left = $x + ($w - $pic.Width) / 2
  $pic.Top  = $y + ($h - $pic.Height) / 2
  $pic.Name = 'GIF_MOCAP'
  $pic.Line.Visible = -1; $pic.Line.ForeColor.RGB = $INK; $pic.Line.Weight = [single]3

  # hard shadow like the other framed pictures (same style as FramePic)
  $sd = $slide.Shapes.AddShape(1, $pic.Left + 6, $pic.Top + 6, $pic.Width, $pic.Height)
  $sd.Fill.Solid(); $sd.Fill.ForeColor.RGB = $INK; $sd.Line.Visible = 0; $sd.Shadow.Visible = 0
  $sd.ZOrder(1)          # send to back
  $pic.ZOrder(0)         # bring picture to front

  $pres.SaveAs($Out)
  Write-Output ('Saved: ' + $Out + ' (slide ' + $slide.SlideIndex + ')')
}
finally {
  $pres.Close(); $app.Quit()
}
