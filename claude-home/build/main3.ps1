$root = Split-Path -Parent $MyInvocation.MyCommand.Path
. (Join-Path $root 'lib2.ps1')
. (Join-Path $root 'lib3.ps1')
OpenDeck
try { . (Join-Path $root 'r1.ps1') } catch { Write-Output ('ERROR: ' + $_.Exception.Message); Write-Output $_.ScriptStackTrace; Write-Output $_.InvocationInfo.PositionMessage; try { $pres.Close() } catch {}; $app.Quit(); exit 1 }

for($i=14;$i -ge 1;$i--){ $pres.Slides($i).Delete() }

$outDir = Join-Path $root 'out3'
New-Item -ItemType Directory -Force $outDir | Out-Null
$outPptx = Join-Path $outDir 'deck.pptx'
if (Test-Path $outPptx) { Remove-Item $outPptx -Force }
$pres.SaveAs($outPptx)
$png = Join-Path $outDir 'png'
if (Test-Path $png) { Remove-Item $png -Recurse -Force }
New-Item -ItemType Directory -Force $png | Out-Null
$pres.Export($png,'PNG',1280,720)
"Slides: " + $pres.Slides.Count
"Warnings: " + $global:Warns.Count
$global:Warns | ForEach-Object { $_ }
$pres.Close(); $app.Quit()
