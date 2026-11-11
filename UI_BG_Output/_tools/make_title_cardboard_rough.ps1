param(
    [string]$SrcPath,
    [string]$OutDir,
    [string]$Text = "ZZakTungSouls",
    [string]$Font = "Impact",
    [switch]$Bold,
    [int]$W = 1800,
    [int]$H = 480,
    [int]$Seed = 7,
    [int]$Rim = 11,
    [int]$Tapes = 8,
    [int]$Patches = 4,
    [double]$Rough = 1.0,
    [string]$Name = "T_UI_Title_ZZakTungSouls_Rough.png"
)

# Rough, patched-together cardboard title: wobbly hand-cut letters, ragged torn rim, thick cut lines, creases, grime,
# cardboard scrap patches and masking / packing tape strips. Output is a transparent PNG (plus a dark-background preview).
$cs = @'
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

public static class RoughTitle
{
    static int SW, SH;
    static int[] SP;
    static float[] NA, NB, N3, NG;

    static float Clamp01(float v) { return v < 0f ? 0f : (v > 1f ? 1f : v); }
    static float Smooth(float e0, float e1, float x)
    {
        float t = Clamp01((x - e0) / (e1 - e0));
        return t * t * (3f - 2f * t);
    }
    static float Cl255(float v) { return v < 0f ? 0f : (v > 255f ? 255f : v); }

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

    static float[] ReadAlpha(Bitmap bm, int W, int H)
    {
        float[] m = new float[W * H];
        BitmapData d = bm.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        int[] px = new int[W * H];
        System.Runtime.InteropServices.Marshal.Copy(d.Scan0, px, 0, W * H);
        bm.UnlockBits(d);
        for (int i = 0; i < m.Length; i++) m[i] = ((px[i] >> 24) & 255) / 255f;
        return m;
    }

    static float[] RenderMask(List<GraphicsPath> paths, float pen, int W, int H)
    {
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
            return ReadAlpha(bm, W, H);
        }
    }

    static float[] RenderStrokes(List<PointF[]> lines, float width, int W, int H)
    {
        using (Bitmap bm = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            using (Graphics g = Graphics.FromImage(bm))
            {
                g.Clear(Color.Transparent);
                g.SmoothingMode = SmoothingMode.AntiAlias;
                using (Pen p = new Pen(Color.White, width))
                {
                    p.LineJoin = LineJoin.Round; p.StartCap = LineCap.Round; p.EndCap = LineCap.Round;
                    foreach (PointF[] l in lines) g.DrawLines(p, l);
                }
            }
            return ReadAlpha(bm, W, H);
        }
    }

    // wobble a soft mask's edge with noise: hand-cut, uneven outline
    static float[] Wobble(float[] m, int W, int H, float amp)
    {
        float[] bl = Blur(m, W, H, 2);
        float[] o = new float[W * H];
        for (int i = 0; i < o.Length; i++)
        {
            float a = 4f * bl[i] * (1f - bl[i]);
            float v = bl[i] + ((NA[i] - 0.5f) * 0.9f + (NB[i] - 0.5f) * 0.5f) * amp * a;
            o[i] = Smooth(0.45f, 0.55f, v);
        }
        return o;
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

    static void ShadowOver(float[] cr, float[] cg, float[] cb, float[] ca, float[] mask, int W, int H, int dx, int dy, int br, float alpha)
    {
        float[] sm = new float[W * H];
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int sx = x - dx, sy = y - dy;
                if (sx >= 0 && sx < W && sy >= 0 && sy < H) sm[y * W + x] = mask[sy * W + sx];
            }
        float[] bl = Blur(sm, W, H, br);
        for (int i = 0; i < bl.Length; i++) Over(cr, cg, cb, ca, i, 20f, 12f, 6f, bl[i] * alpha);
    }

    static GraphicsPath RectPath(float cx, float cy, float w, float h, float ang)
    {
        PointF[] pts = new PointF[] { new PointF(-w / 2f, -h / 2f), new PointF(w / 2f, -h / 2f), new PointF(w / 2f, h / 2f), new PointF(-w / 2f, h / 2f) };
        GraphicsPath gp = new GraphicsPath();
        gp.AddPolygon(pts);
        Matrix m = new Matrix();
        m.Rotate(ang);
        m.Translate(cx, cy, MatrixOrder.Append);
        gp.Transform(m);
        return gp;
    }

    static GraphicsPath TapePath(float cx, float cy, float L, float Wd, float ang, Random R)
    {
        List<PointF> pts = new List<PointF>();
        float hl = L / 2f, hw = Wd / 2f;
        int steps = Math.Max(3, (int)(Wd / 4f));
        for (int i = 0; i <= steps; i++)
        {
            float y = -hw + Wd * i / steps;
            float jx = (i % 2 == 0) ? 0f : (float)(2.0 + R.NextDouble() * 3.0);
            pts.Add(new PointF(hl + jx, y));
        }
        for (int i = steps; i >= 0; i--)
        {
            float y = -hw + Wd * i / steps;
            float jx = (i % 2 == 0) ? 0f : (float)(2.0 + R.NextDouble() * 3.0);
            pts.Add(new PointF(-hl - jx, y));
        }
        GraphicsPath gp = new GraphicsPath();
        gp.AddPolygon(pts.ToArray());
        Matrix m = new Matrix();
        m.Rotate(ang);
        m.Translate(cx, cy, MatrixOrder.Append);
        gp.Transform(m);
        return gp;
    }

    // one cardboard piece (letter or scrap): wobbly cut, thick dark cut line, face from a random spot of the sheet, bevel, grime, creases
    static void DrawPiece(float[] cr, float[] cg, float[] cb, float[] ca, GraphicsPath gp, int W, int H, float wob, float outlineW, Random R, bool shadow, bool creases, float tintLo, float tintHi)
    {
        List<GraphicsPath> one = new List<GraphicsPath>(); one.Add(gp);
        float[] m = Wobble(RenderMask(one, 0f, W, H), W, H, wob);
        float[] mo = Wobble(RenderMask(one, outlineW, W, H), W, H, wob);
        if (shadow) ShadowOver(cr, cg, cb, ca, mo, W, H, 4, 6, 3, 0.45f);
        float[] bf = Blur(m, W, H, 3);
        RectangleF bb = gp.GetBounds();

        float[] cm = null;
        if (creases)
        {
            List<PointF[]> lines = new List<PointF[]>();
            int cnt = 1 + R.Next(0, 2);
            for (int c = 0; c < cnt; c++)
            {
                PointF[] pl = new PointF[4];
                float px = bb.Left + (float)R.NextDouble() * bb.Width;
                float py = bb.Top + (float)R.NextDouble() * bb.Height * 0.5f;
                pl[0] = new PointF(px, py);
                for (int q = 1; q < 4; q++)
                {
                    px += ((float)R.NextDouble() - 0.5f) * bb.Width * 0.7f;
                    py += ((float)R.NextDouble() - 0.15f) * bb.Height * 0.35f;
                    pl[q] = new PointF(px, py);
                }
                lines.Add(pl);
            }
            cm = Blur(RenderStrokes(lines, 1.8f, W, H), W, H, 1);
        }

        int ox = 170 + R.Next(0, 900);
        int oy = 180 + R.Next(0, 330);
        float tint = tintLo + (tintHi - tintLo) * (float)R.NextDouble();
        int mg = (int)(outlineW + 14);
        int xa = Math.Max(0, (int)bb.Left - mg), xb = Math.Min(W - 1, (int)bb.Right + mg);
        int ya = Math.Max(0, (int)bb.Top - mg), yb = Math.Min(H - 1, (int)bb.Bottom + mg);
        for (int y = ya; y <= yb; y++)
            for (int x = xa; x <= xb; x++)
            {
                int i = y * W + x;
                if (mo[i] > 0.001f) Over(cr, cg, cb, ca, i, 58f, 34f, 14f, mo[i] * 0.95f);
                if (m[i] <= 0.001f) continue;
                float r, g, b;
                SrcRGB(x - (int)bb.Left + ox, y - (int)bb.Top + oy, out r, out g, out b);
                int xl = Math.Max(x - 2, 0), xr = Math.Min(x + 2, W - 1);
                int yu = Math.Max(y - 2, 0), yd = Math.Min(y + 2, H - 1);
                float nx = bf[y * W + xr] - bf[y * W + xl];
                float ny = bf[yd * W + x] - bf[yu * W + x];
                float shade = 1f + 0.5f * (nx + ny);
                shade = shade < 0.7f ? 0.7f : (shade > 1.25f ? 1.25f : shade);
                shade *= (0.88f + 0.12f * Smooth(0.5f, 1f, bf[i])) * tint;
                shade *= 0.82f + 0.26f * NG[i];
                if (cm != null) shade *= 1f - 0.45f * cm[i];
                Over(cr, cg, cb, ca, i, Cl255(r * shade * 0.96f), Cl255(g * shade * 0.94f), Cl255(b * shade * 0.91f), m[i]);
            }
    }

    static void DrawTape(float[] cr, float[] cg, float[] cb, float[] ca, GraphicsPath gp, int W, int H, float tr, float tg, float tb, float alpha)
    {
        List<GraphicsPath> one = new List<GraphicsPath>(); one.Add(gp);
        float[] m = RenderMask(one, 0f, W, H);
        ShadowOver(cr, cg, cb, ca, m, W, H, 2, 3, 2, 0.4f);
        float[] bl = Blur(m, W, H, 1);
        RectangleF bb = gp.GetBounds();
        int xa = Math.Max(0, (int)bb.Left - 6), xb = Math.Min(W - 1, (int)bb.Right + 6);
        int ya = Math.Max(0, (int)bb.Top - 6), yb = Math.Min(H - 1, (int)bb.Bottom + 6);
        for (int y = ya; y <= yb; y++)
            for (int x = xa; x <= xb; x++)
            {
                int i = y * W + x;
                if (m[i] <= 0.001f) continue;
                float edge = Smooth(0.5f, 1f, bl[i]);
                float tone = 0.86f + 0.14f * edge;
                float fib = (N3[i] - 0.5f) * 24f;
                float a = alpha * (0.88f + (NB[i] - 0.5f) * 0.4f);
                Over(cr, cg, cb, ca, i, Cl255(tr * tone + fib), Cl255(tg * tone + fib), Cl255(tb * tone + fib), Clamp01(a) * m[i]);
            }
    }

    public static string Make(string srcPath, string outPath, string previewPath, string text, string fontName, bool bold, int W, int H, int seed, int rimPx, int tapes, int patches, float rough)
    {
        LoadSrc(srcPath);
        Random R = new Random(seed);
        NA = Noise(W, H, 10, seed + 21);
        NB = Noise(W, H, 4, seed + 22);
        N3 = Noise(W, H, 2, seed + 23);
        NG = Noise(W, H, 30, seed + 24);

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
        float gap = -6f * rough;
        float pad = rimPx + 46f;

        // uneven letter heights
        for (int k = 0; k < n; k++)
        {
            float sy = 1f + ((float)R.NextDouble() - 0.5f) * 2f * 0.09f * rough;
            Matrix mv = new Matrix(); mv.Scale(1f, sy);
            glyphs[k].Transform(mv);
        }

        float totalW = gap * (n - 1);
        foreach (GraphicsPath p in glyphs) totalW += p.GetBounds().Width;
        float sc = (W - 2f * pad) / totalW;
        Matrix ms = new Matrix(); ms.Scale(sc, sc);
        foreach (GraphicsPath p in glyphs) p.Transform(ms);
        gap *= sc;

        float top = float.MaxValue, bot = float.MinValue;
        foreach (GraphicsPath p in glyphs) { RectangleF bb = p.GetBounds(); top = Math.Min(top, bb.Top); bot = Math.Max(bot, bb.Bottom); }
        float hh = bot - top;
        float maxH = H - 2f * pad;
        if (hh > maxH)
        {
            float s2 = maxH / hh;
            Matrix m2 = new Matrix(); m2.Scale(s2, s2);
            foreach (GraphicsPath p in glyphs) p.Transform(m2);
            gap *= s2;
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
            float ang = (float)((R.NextDouble() - 0.5) * 2.0 * 7.5 * rough);
            float dy = (float)((R.NextDouble() - 0.5) * 2.0 * 0.06 * rough * hh);
            Matrix mr = new Matrix();
            mr.RotateAt(ang, new PointF(bb.Left + bb.Width / 2f, bb.Top + bb.Height / 2f));
            p.Transform(mr);
            Matrix md = new Matrix(); md.Translate(0f, dy);
            p.Transform(md);
        }

        int N = W * H;
        float[] cr = new float[N], cg = new float[N], cb = new float[N], ca = new float[N];

        // ---- torn rim (union, dilated, very ragged) ----
        float[] rimBase = RenderMask(glyphs, 2f * rimPx, W, H);
        float[] rimBl = Blur(rimBase, W, H, 3);
        float[] n1 = Noise(W, H, 7, seed + 1);
        float[] alphaRim = new float[N];
        for (int i = 0; i < N; i++)
        {
            float amp = 4f * rimBl[i] * (1f - rimBl[i]);
            float v = rimBl[i] + ((n1[i] - 0.5f) * 1.3f + (NB[i] - 0.5f) * 0.7f) * amp;
            alphaRim[i] = Smooth(0.42f, 0.58f, v);
        }
        ShadowOver(cr, cg, cb, ca, alphaRim, W, H, 5, 9, 5, 0.6f);
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * W + x;
                if (alphaRim[i] <= 0.001f) continue;
                float r, g, b;
                SrcRGB(x + 300, y + 180, out r, out g, out b);
                float fib = (N3[i] - 0.5f) * 40f;
                r = r * 0.6f + 226f * 0.4f + fib;
                g = g * 0.6f + 198f * 0.4f + fib;
                b = b * 0.6f + 146f * 0.4f + fib;
                float dirt = 0.8f + 0.28f * NG[i];
                float d = Clamp01((0.86f - rimBl[i]) / 0.46f);
                float dk = (1f - 0.55f * d) * dirt;
                Over(cr, cg, cb, ca, i, Cl255(r * dk), Cl255(g * dk), Cl255(b * dk), alphaRim[i]);
            }

        // ---- letters ----
        List<RectangleF> lb = new List<RectangleF>();
        for (int k = 0; k < n; k++) lb.Add(glyphs[k].GetBounds());
        for (int k = 0; k < n; k++)
            DrawPiece(cr, cg, cb, ca, glyphs[k], W, H, 1.15f * rough, 5.5f, R, false, true, 0.84f, 1.12f);

        // ---- cardboard scrap patches glued on ----
        for (int q = 0; q < patches; q++)
        {
            RectangleF bb = lb[R.Next(0, n)];
            float cx = bb.Left + (0.2f + 0.6f * (float)R.NextDouble()) * bb.Width;
            float cy = bb.Top + (0.25f + 0.65f * (float)R.NextDouble()) * bb.Height;
            float pw = 55f + (float)R.NextDouble() * 50f;
            float ph = 38f + (float)R.NextDouble() * 34f;
            float ang = ((float)R.NextDouble() - 0.5f) * 50f;
            using (GraphicsPath gp = RectPath(cx, cy, pw, ph, ang))
                DrawPiece(cr, cg, cb, ca, gp, W, H, 1.7f, 4.5f, R, true, false, 0.78f, 1.2f);
        }

        // ---- tape ----
        for (int q = 0; q < tapes; q++)
        {
            int kind = q % 3;
            float cx, cy, L, Wd, ang;
            if (kind == 0)
            {
                RectangleF bb = lb[R.Next(0, n)];
                bool left = R.Next(0, 2) == 0, upper = R.Next(0, 2) == 0;
                cx = left ? bb.Left + 6f : bb.Right - 6f;
                cy = upper ? bb.Top + 10f : bb.Bottom - 10f;
                ang = (left == upper ? -40f : 40f) + ((float)R.NextDouble() - 0.5f) * 16f;
                L = 70f + (float)R.NextDouble() * 30f; Wd = 24f + (float)R.NextDouble() * 6f;
            }
            else if (kind == 1)
            {
                int k = R.Next(0, n - 1);
                cx = (lb[k].Right + lb[k + 1].Left) / 2f;
                cy = H / 2f + ((float)R.NextDouble() - 0.5f) * 0.5f * hh;
                ang = (R.Next(0, 2) == 0 ? 1f : -1f) * (4f + (float)R.NextDouble() * 12f);
                L = 150f + (float)R.NextDouble() * 55f; Wd = 26f + (float)R.NextDouble() * 6f;
            }
            else
            {
                RectangleF bb = lb[R.Next(0, n)];
                cx = bb.Left + bb.Width / 2f + ((float)R.NextDouble() - 0.5f) * bb.Width * 0.4f;
                cy = bb.Top + bb.Height / 2f + ((float)R.NextDouble() - 0.5f) * bb.Height * 0.3f;
                ang = 90f + ((float)R.NextDouble() - 0.5f) * 30f;
                L = 120f + (float)R.NextDouble() * 40f; Wd = 24f + (float)R.NextDouble() * 6f;
            }
            bool packing = (q % 4 == 3);
            using (GraphicsPath gp = TapePath(cx, cy, L, Wd, ang, R))
            {
                if (packing) DrawTape(cr, cg, cb, ca, gp, W, H, 176f, 126f, 70f, 0.66f);
                else DrawTape(cr, cg, cb, ca, gp, W, H, 238f, 218f, 172f, 0.72f);
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
        return "font=" + fontName + " letters=" + n + " tapes=" + tapes + " patches=" + patches + " size=" + W + "x" + H;
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
$msg = [RoughTitle]::Make($SrcPath, $outPath, $previewPath, $Text, $Font, [bool]$Bold, $W, $H, $Seed, $Rim, $Tapes, $Patches, [single]$Rough)
$msg
"saved: $outPath"
"preview: $previewPath"
