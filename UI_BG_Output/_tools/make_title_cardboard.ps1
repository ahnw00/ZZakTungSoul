param(
    [string]$SrcPath,
    [string]$OutDir,
    [string]$Text = "ZZakTungSouls",
    [string]$Font = "Impact",
    [switch]$Bold,
    [int]$W = 1800,
    [int]$H = 480,
    [int]$Seed = 7,
    [int]$Rim = 9,
    [string]$Name = "T_UI_Title_ZZakTungSouls.png"
)

# Cardboard cut-out title: every letter is cut from a different spot of the flat cardboard in the source sheet,
# surrounded by a ragged torn-paper rim, a thin dark cut line, a bevel and a drop shadow. Output is a transparent PNG.
$cs = @'
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

public static class CardboardTitle
{
    static int SW, SH;
    static int[] SP;

    static float Clamp01(float v) { return v < 0f ? 0f : (v > 1f ? 1f : v); }
    static float Smooth(float e0, float e1, float x)
    {
        float t = Clamp01((x - e0) / (e1 - e0));
        return t * t * (3f - 2f * t);
    }

    static void LoadSrc(string path)
    {
        using (Bitmap b = new Bitmap(path))
        using (Bitmap c = new Bitmap(b.Width, b.Height, PixelFormat.Format32bppArgb))
        {
            using (Graphics g = Graphics.FromImage(c)) { g.DrawImage(b, 0, 0, b.Width, b.Height); }
            SW = c.Width; SH = c.Height;
            SP = new int[SW * SH];
            BitmapData d = c.LockBits(new Rectangle(0, 0, SW, SH), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            System.Runtime.InteropServices.Marshal.Copy(d.Scan0, SP, 0, SW * SH);
            c.UnlockBits(d);
        }
    }

    static void SrcRGB(int x, int y, out float r, out float g, out float b)
    {
        if (x < 0) x = 0; if (x >= SW) x = SW - 1;
        if (y < 0) y = 0; if (y >= SH) y = SH - 1;
        int p = SP[y * SW + x];
        r = (p >> 16) & 255; g = (p >> 8) & 255; b = p & 255;
    }

    static float[] BoxBlur(float[] a, int W, int H, int r)
    {
        float[] tmp = new float[W * H];
        float[] o = new float[W * H];
        float inv = 1f / (2 * r + 1);
        for (int y = 0; y < H; y++)
        {
            int row = y * W;
            float sum = 0f;
            for (int x = -r; x <= r; x++) sum += a[row + Math.Min(Math.Max(x, 0), W - 1)];
            for (int x = 0; x < W; x++)
            {
                tmp[row + x] = sum * inv;
                sum += a[row + Math.Min(x + r + 1, W - 1)] - a[row + Math.Max(x - r, 0)];
            }
        }
        for (int x = 0; x < W; x++)
        {
            float sum = 0f;
            for (int y = -r; y <= r; y++) sum += tmp[Math.Min(Math.Max(y, 0), H - 1) * W + x];
            for (int y = 0; y < H; y++)
            {
                o[y * W + x] = sum * inv;
                sum += tmp[Math.Min(y + r + 1, H - 1) * W + x] - tmp[Math.Max(y - r, 0) * W + x];
            }
        }
        return o;
    }
    static float[] Blur(float[] a, int W, int H, int r) { return BoxBlur(BoxBlur(a, W, H, r), W, H, r); }

    static float[] Noise(int W, int H, int cell, int seed)
    {
        Random rr = new Random(seed);
        int gw = W / cell + 3, gh = H / cell + 3;
        float[] grid = new float[gw * gh];
        for (int i = 0; i < grid.Length; i++) grid[i] = (float)rr.NextDouble();
        float[] o = new float[W * H];
        for (int y = 0; y < H; y++)
        {
            float fy = y / (float)cell; int iy = (int)fy; float ty = fy - iy; ty = ty * ty * (3f - 2f * ty);
            for (int x = 0; x < W; x++)
            {
                float fx = x / (float)cell; int ix = (int)fx; float tx = fx - ix; tx = tx * tx * (3f - 2f * tx);
                float a = grid[iy * gw + ix], b = grid[iy * gw + ix + 1];
                float c = grid[(iy + 1) * gw + ix], d = grid[(iy + 1) * gw + ix + 1];
                o[y * W + x] = (a + (b - a) * tx) * (1f - ty) + (c + (d - c) * tx) * ty;
            }
        }
        return o;
    }

    static float[] RenderMask(List<GraphicsPath> paths, float pen, int W, int H)
    {
        float[] m = new float[W * H];
        using (Bitmap bm = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            using (Graphics g = Graphics.FromImage(bm))
            {
                g.Clear(Color.Transparent);
                g.SmoothingMode = SmoothingMode.AntiAlias;
                using (SolidBrush br = new SolidBrush(Color.White))
                using (Pen p = new Pen(Color.White, Math.Max(pen, 0.01f)))
                {
                    p.LineJoin = LineJoin.Round;
                    foreach (GraphicsPath gp in paths)
                    {
                        g.FillPath(br, gp);
                        if (pen > 0f) g.DrawPath(p, gp);
                    }
                }
            }
            BitmapData d = bm.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            int[] px = new int[W * H];
            System.Runtime.InteropServices.Marshal.Copy(d.Scan0, px, 0, W * H);
            bm.UnlockBits(d);
            for (int i = 0; i < m.Length; i++) m[i] = ((px[i] >> 24) & 255) / 255f;
        }
        return m;
    }

    static void Over(float[] cr, float[] cg, float[] cb, float[] ca, int i, float r, float g, float b, float a)
    {
        if (a <= 0.001f) return;
        float da = ca[i];
        float oa = a + da * (1f - a);
        cr[i] = (r * a + cr[i] * da * (1f - a)) / oa;
        cg[i] = (g * a + cg[i] * da * (1f - a)) / oa;
        cb[i] = (b * a + cb[i] * da * (1f - a)) / oa;
        ca[i] = oa;
    }

    static float Cl255(float v) { return v < 0f ? 0f : (v > 255f ? 255f : v); }

    public static string Make(string srcPath, string outPath, string previewPath, string text, string fontName, bool bold, int W, int H, int seed, int rimPx)
    {
        LoadSrc(srcPath);
        Random R = new Random(seed);
        FontFamily ff = new FontFamily(fontName);
        int style = bold ? (int)FontStyle.Bold : (int)FontStyle.Regular;
        StringFormat sf = StringFormat.GenericTypographic;

        List<GraphicsPath> glyphs = new List<GraphicsPath>();
        foreach (char ch in text)
        {
            if (ch == ' ') continue;
            GraphicsPath p = new GraphicsPath();
            p.AddString(ch.ToString(), ff, style, 100f, new PointF(0f, 0f), sf);
            glyphs.Add(p);
        }
        int n = glyphs.Count;
        float gap = 4f;
        float pad = rimPx + 40f;

        float totalW = gap * (n - 1);
        foreach (GraphicsPath p in glyphs) totalW += p.GetBounds().Width;
        float sc = (W - 2f * pad) / totalW;
        Matrix ms = new Matrix(); ms.Scale(sc, sc);
        foreach (GraphicsPath p in glyphs) p.Transform(ms);

        float top = float.MaxValue, bot = float.MinValue;
        foreach (GraphicsPath p in glyphs) { RectangleF bb = p.GetBounds(); top = Math.Min(top, bb.Top); bot = Math.Max(bot, bb.Bottom); }
        float hh = bot - top;
        float maxH = H - 2f * pad;
        if (hh > maxH)
        {
            float s2 = maxH / hh;
            Matrix m2 = new Matrix(); m2.Scale(s2, s2);
            foreach (GraphicsPath p in glyphs) p.Transform(m2);
            top = float.MaxValue; bot = float.MinValue;
            foreach (GraphicsPath p in glyphs) { RectangleF bb = p.GetBounds(); top = Math.Min(top, bb.Top); bot = Math.Max(bot, bb.Bottom); }
            hh = bot - top;
        }
        totalW = gap * (n - 1);
        foreach (GraphicsPath p in glyphs) totalW += p.GetBounds().Width;

        float x0 = (W - totalW) / 2f;
        float offY = (H - hh) / 2f - top;
        for (int k = 0; k < n; k++)
        {
            GraphicsPath p = glyphs[k];
            RectangleF bb = p.GetBounds();
            Matrix mt = new Matrix(); mt.Translate(x0 - bb.Left, offY);
            p.Transform(mt);
            x0 += bb.Width + gap;
            bb = p.GetBounds();
            float ang = (float)((R.NextDouble() - 0.5) * 2.0 * 5.0);
            float dy = (float)((R.NextDouble() - 0.5) * 2.0 * 0.035 * hh);
            Matrix mr = new Matrix();
            mr.RotateAt(ang, new PointF(bb.Left + bb.Width / 2f, bb.Top + bb.Height / 2f));
            p.Transform(mr);
            Matrix md = new Matrix(); md.Translate(0f, dy);
            p.Transform(md);
        }

        int N = W * H;
        float[] cr = new float[N], cg = new float[N], cb = new float[N], ca = new float[N];

        // ---- torn rim (union of all letters, dilated, ragged edge) ----
        float[] rimBase = RenderMask(glyphs, 2f * rimPx, W, H);
        float[] rimBl = Blur(rimBase, W, H, 3);
        float[] n1 = Noise(W, H, 8, seed + 1);
        float[] n2 = Noise(W, H, 3, seed + 2);
        float[] n3 = Noise(W, H, 2, seed + 3);
        float[] alphaRim = new float[N];
        for (int i = 0; i < N; i++)
        {
            float amp = 4f * rimBl[i] * (1f - rimBl[i]);
            float v = rimBl[i] + ((n1[i] - 0.5f) * 0.9f + (n2[i] - 0.5f) * 0.5f) * amp;
            alphaRim[i] = Smooth(0.42f, 0.58f, v);
        }

        // ---- drop shadow ----
        int shX = 5, shY = 8;
        float[] shBase = new float[N];
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int sx = x - shX, sy = y - shY;
                if (sx >= 0 && sx < W && sy >= 0 && sy < H) shBase[y * W + x] = alphaRim[sy * W + sx];
            }
        float[] sh = Blur(shBase, W, H, 5);
        for (int i = 0; i < N; i++) Over(cr, cg, cb, ca, i, 20f, 12f, 6f, sh[i] * 0.55f);

        // ---- rim colour: lighter fibrous cardboard, darker toward the outer edge ----
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * W + x;
                if (alphaRim[i] <= 0.001f) continue;
                float r, g, b;
                SrcRGB(x + 300, y + 180, out r, out g, out b);
                float fib = (n3[i] - 0.5f) * 34f;
                r = r * 0.62f + 238f * 0.38f + fib;
                g = g * 0.62f + 210f * 0.38f + fib;
                b = b * 0.62f + 158f * 0.38f + fib;
                float d = Clamp01((0.86f - rimBl[i]) / 0.46f);
                float dk = 1f - 0.52f * d;
                Over(cr, cg, cb, ca, i, Cl255(r * dk), Cl255(g * dk), Cl255(b * dk), alphaRim[i]);
            }

        // ---- letters: dark cut line + face cut from a different spot of the flat cardboard + bevel ----
        for (int k = 0; k < n; k++)
        {
            List<GraphicsPath> one = new List<GraphicsPath>(); one.Add(glyphs[k]);
            float[] m = RenderMask(one, 0f, W, H);
            float[] mo = RenderMask(one, 3.5f, W, H);
            float[] bf = Blur(m, W, H, 3);
            RectangleF bb = glyphs[k].GetBounds();
            int ox = 170 + R.Next(0, 900);
            int oy = 180 + R.Next(0, 330);
            float tint = 0.94f + 0.12f * (float)R.NextDouble();
            int xa = Math.Max(0, (int)bb.Left - 8), xb = Math.Min(W - 1, (int)bb.Right + 8);
            int ya = Math.Max(0, (int)bb.Top - 8), yb = Math.Min(H - 1, (int)bb.Bottom + 8);
            for (int y = ya; y <= yb; y++)
                for (int x = xa; x <= xb; x++)
                {
                    int i = y * W + x;
                    if (mo[i] > 0.001f) Over(cr, cg, cb, ca, i, 70f, 42f, 18f, mo[i] * 0.9f);
                    if (m[i] <= 0.001f) continue;
                    float r, g, b;
                    SrcRGB(x - (int)bb.Left + ox, y - (int)bb.Top + oy, out r, out g, out b);
                    int xl = Math.Max(x - 2, 0), xr = Math.Min(x + 2, W - 1);
                    int yu = Math.Max(y - 2, 0), yd = Math.Min(y + 2, H - 1);
                    float nx = bf[y * W + xr] - bf[y * W + xl];
                    float ny = bf[yd * W + x] - bf[yu * W + x];
                    float shade = 1f + 0.55f * (nx + ny);
                    shade = shade < 0.72f ? 0.72f : (shade > 1.28f ? 1.28f : shade);
                    shade *= (0.9f + 0.1f * Smooth(0.5f, 1f, bf[i])) * tint;
                    Over(cr, cg, cb, ca, i, Cl255(r * shade), Cl255(g * shade), Cl255(b * shade), m[i]);
                }
        }

        // ---- save ----
        int[] outPx = new int[N];
        int[] prePx = new int[N];
        for (int i = 0; i < N; i++)
        {
            int a = (int)(Clamp01(ca[i]) * 255f + 0.5f);
            int r = (int)(Cl255(cr[i]) + 0.5f), g = (int)(Cl255(cg[i]) + 0.5f), b = (int)(Cl255(cb[i]) + 0.5f);
            outPx[i] = (a << 24) | (r << 16) | (g << 8) | b;
            float al = Clamp01(ca[i]);
            int pr = (int)(Cl255(cr[i] * al + 36f * (1f - al)) + 0.5f);
            int pg = (int)(Cl255(cg[i] * al + 34f * (1f - al)) + 0.5f);
            int pb = (int)(Cl255(cb[i] * al + 40f * (1f - al)) + 0.5f);
            prePx[i] = (255 << 24) | (pr << 16) | (pg << 8) | pb;
        }
        SaveArgb(outPx, W, H, outPath);
        SaveArgb(prePx, W, H, previewPath);
        return "font=" + fontName + " letters=" + n + " size=" + W + "x" + H;
    }

    static void SaveArgb(int[] px, int W, int H, string path)
    {
        using (Bitmap bm = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            BitmapData d = bm.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            System.Runtime.InteropServices.Marshal.Copy(px, 0, d.Scan0, W * H);
            bm.UnlockBits(d);
            bm.Save(path, ImageFormat.Png);
        }
    }
}
'@

Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition $cs -Language CSharp

if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Path $OutDir | Out-Null }
$outPath = Join-Path $OutDir $Name
$previewPath = Join-Path $OutDir ("_Preview_" + $Name)
$msg = [CardboardTitle]::Make($SrcPath, $outPath, $previewPath, $Text, $Font, [bool]$Bold, $W, $H, $Seed, $Rim)
$msg
"saved: $outPath"
"preview: $previewPath"
