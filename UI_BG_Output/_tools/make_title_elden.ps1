param(
    [string]$SrcPath,
    [string]$OutDir,
    [string]$Line1 = "ZZAKTUNG",
    [string]$Line2 = "SOULS",
    [string]$Font = "Perpetua Titling MT",
    [int]$W = 2400,
    [int]$H = 720,
    [int]$Seed = 11,
    [int]$Depth = 22,
    [int]$Cracks = 20,
    [string]$Name = "T_UI_Title_Cardboard.png"
)

# Souls-like title lockup (ELDEN RING-style reference) built from thick corrugated cardboard:
# trajan-like capitals, extruded edge showing corrugation flutes, torn-off top liner that exposes the inner flute layer,
# cracks, top-lit vertical gradient, warm outer glow. Output = transparent PNG + dark preview.
$cs = @'
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

public static class EldenTitle
{
    static int SW, SH;
    static int[] SP;

    static float Clamp01(float v) { return v < 0f ? 0f : (v > 1f ? 1f : v); }
    static float Smooth(float e0, float e1, float x) { float t = Clamp01((x - e0) / (e1 - e0)); return t * t * (3f - 2f * t); }
    static float Cl255(float v) { return v < 0f ? 0f : (v > 255f ? 255f : v); }
    static float Lerp(float a, float b, float t) { return a + (b - a) * t; }

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

    static int Mirror(int v, int n)
    {
        int p = 2 * n;
        v = ((v % p) + p) % p;
        return v < n ? v : p - 1 - v;
    }

    // flat cardboard area of the source sheet only (x 140..1780, y 160..900), mirrored so any offset is valid
    static void SrcRGB(int x, int y, out float r, out float g, out float b)
    {
        int ax = 140, aw = 1640, ay = 160, ah = 740;
        int sx = ax + Mirror(x, aw), sy = ay + Mirror(y, ah);
        int p = SP[sy * SW + sx];
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

    static float[] RenderMask(List<GraphicsPath> paths, int W, int H)
    {
        using (Bitmap bm = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            using (Graphics g = Graphics.FromImage(bm))
            {
                g.Clear(Color.Transparent);
                g.SmoothingMode = SmoothingMode.AntiAlias;
                using (SolidBrush br = new SolidBrush(Color.White))
                    foreach (GraphicsPath gp in paths) g.FillPath(br, gp);
            }
            return ReadAlpha(bm, W, H);
        }
    }

    static float[] RenderLines(List<PointF[]> lines, float width, int W, int H)
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
                    foreach (PointF[] l in lines) if (l.Length > 1) g.DrawLines(p, l);
                }
            }
            return ReadAlpha(bm, W, H);
        }
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

    // one text line -> glyph paths, laid out left to right, scaled so total width == targetW. Returns cap-height in px via out.
    static List<GraphicsPath> Line(string text, string fontName, float targetW, float tracking, out float capH)
    {
        FontFamily ff = new FontFamily(fontName);
        List<GraphicsPath> gl = new List<GraphicsPath>();
        foreach (char ch in text)
        {
            GraphicsPath p = new GraphicsPath();
            p.AddString(ch.ToString(), ff, (int)FontStyle.Bold, 100f, new PointF(0f, 0f), StringFormat.GenericTypographic);
            gl.Add(p);
        }
        float tw = tracking * (gl.Count - 1);
        foreach (GraphicsPath p in gl) tw += p.GetBounds().Width;
        float sc = targetW / tw;
        Matrix ms = new Matrix(); ms.Scale(sc, sc);
        foreach (GraphicsPath p in gl) p.Transform(ms);
        float x = 0f; float top = float.MaxValue, bot = float.MinValue;
        foreach (GraphicsPath p in gl)
        {
            RectangleF bb = p.GetBounds();
            Matrix mt = new Matrix(); mt.Translate(x - bb.Left, 0f); p.Transform(mt);
            x += bb.Width + tracking * sc;
            bb = p.GetBounds(); top = Math.Min(top, bb.Top); bot = Math.Max(bot, bb.Bottom);
        }
        capH = bot - top;
        // normalise so the line's top edge is y = 0
        foreach (GraphicsPath p in gl) { Matrix mn = new Matrix(); mn.Translate(0f, -top); p.Transform(mn); }
        return gl;
    }

    static void Shift(List<GraphicsPath> l, float dx, float dy)
    {
        Matrix m = new Matrix(); m.Translate(dx, dy);
        foreach (GraphicsPath p in l) p.Transform(m);
    }

    static float Right(List<GraphicsPath> l) { float v = float.MinValue; foreach (GraphicsPath p in l) v = Math.Max(v, p.GetBounds().Right); return v; }
    static float Left(List<GraphicsPath> l) { float v = float.MaxValue; foreach (GraphicsPath p in l) v = Math.Min(v, p.GetBounds().Left); return v; }

    static GraphicsPath Bar(float x0, float x1, float yc, float h, bool pointLeft)
    {
        GraphicsPath p = new GraphicsPath();
        float t = h * 0.5f;
        if (pointLeft)
            p.AddPolygon(new PointF[] { new PointF(x0, yc), new PointF(x0 + h * 1.4f, yc - t), new PointF(x1, yc - t), new PointF(x1, yc + t), new PointF(x0 + h * 1.4f, yc + t) });
        else
            p.AddPolygon(new PointF[] { new PointF(x1, yc), new PointF(x1 - h * 1.4f, yc - t), new PointF(x0, yc - t), new PointF(x0, yc + t), new PointF(x1 - h * 1.4f, yc + t) });
        return p;
    }

    public static string Make(string srcPath, string outPath, string previewPath, string line1, string line2, string fontName, int W, int H, int seed, int depth, int crackCount)
    {
        LoadSrc(srcPath);
        Random R = new Random(seed);
        float padX = 120f;

        // ---------- layout ----------
        float cap1, cap2;
        List<GraphicsPath> L1 = Line(line1, fontName, W - 2f * padX, 9f, out cap1);
        // line 2: half-size letters, wide tracking, centred
        float w1 = W - 2f * padX;
        float cap1ref = cap1;
        // scale for line 2 = 0.46 of line 1; build at width proportional to letter count
        float l2w = w1 * 0.40f;
        List<GraphicsPath> L2 = Line(line2, fontName, l2w, 44f, out cap2);
        float gapLines = cap1 * 0.30f;
        float totalH = cap1 + gapLines + cap2;
        float y1 = (H - totalH) / 2f - depth * 0.3f;
        Shift(L1, padX, y1);
        float x2 = (W - l2w) / 2f;
        float y2 = y1 + cap1 + gapLines;
        Shift(L2, x2, y2);

        List<GraphicsPath> all = new List<GraphicsPath>();
        all.AddRange(L1); all.AddRange(L2);
        // pointed cardboard strips left/right of line 2
        float yc2 = y2 + cap2 * 0.5f;
        float barH = cap2 * 0.15f;
        float gapBar = cap2 * 0.70f;
        float lx1 = Left(L2) - gapBar, rx0 = Right(L2) + gapBar;
        all.Add(Bar(padX, lx1, yc2, barH, true));
        all.Add(Bar(rx0, W - padX, yc2, barH, false));

        int N = W * H;
        float topAll = y1, botAll = y2 + cap2;

        // ---------- masks ----------
        float[] F = RenderMask(all, W, H);

        // extrusion (down / slightly right), keeps depth index of the nearest layer
        float[] S = new float[N];
        float[] Sd = new float[N];
        for (int k = depth; k >= 1; k--)
        {
            int dx = (int)Math.Round(k * 0.30f), dy = k;
            for (int y = 0; y < H - dy; y++)
            {
                int row = y * W, row2 = (y + dy) * W;
                for (int x = 0; x < W - dx; x++)
                {
                    float f = F[row + x];
                    if (f <= 0.02f) continue;
                    int j = row2 + x + dx;
                    if (f > 0.3f) { if (f > S[j]) S[j] = f; Sd[j] = k; }   // k runs depth..1, so the nearest layer wins
                }
            }
        }

        float[] nA = Noise(W, H, 40, seed + 1);     // big blotches
        float[] nB = Noise(W, H, 9, seed + 2);      // medium
        float[] nC = Noise(W, H, 3, seed + 3);      // fibres
        float[] nD = Noise(W, H, 90, seed + 4);     // very large (peel zones)

        // ---------- cracks ----------
        List<PointF[]> lines = new List<PointF[]>();
        for (int c = 0; c < crackCount; c++)
        {
            int tries = 0; float px = 0, py = 0;
            while (tries++ < 400)
            {
                px = (float)(R.NextDouble() * W); py = topAll + (float)(R.NextDouble() * (botAll - topAll));
                int ix = (int)px, iy = (int)py;
                if (ix >= 0 && ix < W && iy >= 0 && iy < H && F[iy * W + ix] > 0.99f) break;
            }
            float ang = (float)(R.NextDouble() * Math.PI * 2.0);
            int segs = 6 + R.Next(0, 10);
            List<PointF> pts = new List<PointF>(); pts.Add(new PointF(px, py));
            for (int s = 0; s < segs; s++)
            {
                ang += (float)((R.NextDouble() - 0.5) * 1.1);
                float len = 8f + (float)R.NextDouble() * 20f;
                px += (float)Math.Cos(ang) * len; py += (float)Math.Sin(ang) * len;
                pts.Add(new PointF(px, py));
                if (R.NextDouble() < 0.22 && s > 1)
                {
                    List<PointF> br = new List<PointF>(); br.Add(new PointF(px, py));
                    float a2 = ang + (R.NextDouble() < 0.5 ? -1f : 1f) * (0.5f + (float)R.NextDouble() * 0.7f);
                    float bx = px, by = py;
                    int bs = 2 + R.Next(0, 4);
                    for (int q = 0; q < bs; q++)
                    {
                        a2 += (float)((R.NextDouble() - 0.5) * 0.9);
                        float l2 = 6f + (float)R.NextDouble() * 14f;
                        bx += (float)Math.Cos(a2) * l2; by += (float)Math.Sin(a2) * l2;
                        br.Add(new PointF(bx, by));
                    }
                    lines.Add(br.ToArray());
                }
            }
            lines.Add(pts.ToArray());
        }
        float[] crack = RenderLines(lines, 2.0f, W, H);

        // ---------- face / rim structure ----------
        float[] fb2 = Blur(F, W, H, 5);
        float[] bf = Blur(F, W, H, 3);
        float[] inner = new float[N];      // 1 = intact top liner, 0 = torn away (exposed flute layer)
        for (int i = 0; i < N; i++)
        {
            if (F[i] <= 0.001f) continue;
            // only the outer band can peel: weight the noise by how close to the edge we are (fb2 < 1 near the edge)
            float edgeBand = Clamp01((0.98f - fb2[i]) / 0.5f);
            float v = fb2[i] + ((nB[i] - 0.5f) * 0.5f + (nC[i] - 0.5f) * 0.22f) * (0.35f + 0.65f * edgeBand);
            v -= Smooth(0.62f, 0.82f, nD[i]) * 0.22f * edgeBand;   // occasional larger peeled zones, edge only
            inner[i] = Smooth(0.58f, 0.66f, v);
        }

        float[] cr = new float[N], cg = new float[N], cb = new float[N], ca = new float[N];

        // ---------- glow + shadow ----------
        float[] full = new float[N];
        for (int i = 0; i < N; i++) full[i] = Math.Max(F[i], S[i]);
        float[] glow = Blur(full, W, H, 26);
        float[] glow2 = Blur(full, W, H, 9);
        for (int i = 0; i < N; i++)
        {
            Over(cr, cg, cb, ca, i, 235f, 140f, 55f, Clamp01(glow[i] * 0.26f));
            Over(cr, cg, cb, ca, i, 245f, 170f, 85f, Clamp01(glow2[i] * 0.08f));
        }
        float[] shBase = new float[N];
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int sx = x - 5, sy = y - (depth + 8);
                if (sx >= 0 && sy >= 0) shBase[y * W + x] = full[sy * W + sx];
            }
        float[] sh = Blur(shBase, W, H, 8);
        for (int i = 0; i < N; i++) Over(cr, cg, cb, ca, i, 12f, 6f, 2f, sh[i] * 0.55f);

        // ---------- extruded side: cut edge of corrugated board ----------
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * W + x;
                float a = S[i] * (1f - F[i]);
                if (a <= 0.01f) continue;
                // cut edge of corrugated board: flat liner on top, flat liner at the bottom, sine-wave flute paper in between
                float kf = Sd[i] / (float)depth;                      // 0 just under the face .. 1 bottom
                float yw = 0.5f + 0.24f * (float)Math.Sin(x * 0.30f + nA[i] * 1.0f);
                float onWave = 1f - Smooth(0.06f, 0.12f, Math.Abs(kf - yw));
                float onTop = 1f - Smooth(0.08f, 0.14f, kf);
                float onBot = Smooth(0.86f, 0.92f, kf);
                float paper = Clamp01(Math.Max(onWave, Math.Max(onTop, onBot)));
                float fib = (nC[i] - 0.5f) * 30f;
                // paper colour (lit from above: the wave crest catches light, the liners are in shadow)
                float pr = 188f + fib, pg = 132f + fib, pb = 74f + fib;
                // gap colour: dark hollow between liner and flute, slightly lighter near the bottom (bounce light)
                float gr = 98f + 30f * kf, gg = 60f + 18f * kf, gb = 30f + 10f * kf;
                float sr = Lerp(gr, pr, paper), sg = Lerp(gg, pg, paper), sb = Lerp(gb, pb, paper);
                float dk = 0.86f - 0.30f * kf;
                Over(cr, cg, cb, ca, i, Cl255(sr * dk), Cl255(sg * dk), Cl255(sb * dk), a);
            }

        // ---------- face ----------
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * W + x;
                if (F[i] <= 0.001f) continue;
                float r, g, b;
                // exposed flute layer (lighter, striped)
                float stp = 0.5f + 0.5f * (float)Math.Sin(x * 0.50f + nB[i] * 2.5f);   // top view of flute ridges
                float er = Lerp(176f, 232f, stp), eg = Lerp(126f, 186f, stp), eb = Lerp(70f, 118f, stp);
                float efib = (nC[i] - 0.5f) * 40f;
                er += efib; eg += efib; eb += efib;
                // top liner: sheet texture, contrast boosted
                SrcRGB(x + 260, y + 220, out r, out g, out b);
                // pull toward a duller kraft brown (less saturated orange) and boost contrast
                r = 176f + (r - 196f) * 2.1f; g = 124f + (g - 138f) * 2.1f; b = 78f + (b - 84f) * 2.1f;
                float blot = 0.86f + 0.26f * nA[i];
                float fine = (nC[i] - 0.5f) * 22f;
                float ridge = 1f + 0.05f * (float)Math.Sin(y * 0.85f + nA[i] * 6f);   // faint corrugation ridges under the liner
                r = (r * blot + fine) * ridge; g = (g * blot + fine) * ridge; b = (b * blot + fine) * ridge;

                // cracks (dark line + light lip)
                float c = crack[i];
                float c2 = (x > 0 && y > 0) ? crack[(y - 1) * W + (x - 1)] : 0f;
                r *= (1f - 0.60f * c); g *= (1f - 0.66f * c); b *= (1f - 0.72f * c);
                float lip = c2 * (1f - c) * 0.3f;
                r += 70f * lip; g += 56f * lip; b += 36f * lip;

                // mix by torn-away mask
                float t = inner[i];
                float fr = Lerp(er, r, t), fg = Lerp(eg, g, t), fbl = Lerp(eb, b, t);

                // bevel from the blurred silhouette (light from top-left)
                int xl = Math.Max(x - 2, 0), xr = Math.Min(x + 2, W - 1);
                int yu = Math.Max(y - 2, 0), yd = Math.Min(y + 2, H - 1);
                float nx = bf[y * W + xr] - bf[y * W + xl];
                float ny = bf[yd * W + x] - bf[yu * W + x];
                float shade = 1f + 1.1f * (nx + ny);
                shade = shade < 0.62f ? 0.62f : (shade > 1.45f ? 1.45f : shade);
                // vertical light: bright on top, dark at the bottom (reference look)
                float vt = Clamp01((y - topAll) / (botAll - topAll));
                float vl = Lerp(1.28f, 0.58f, vt);
                // outline where liner ends / body edge
                float edge = 4f * t * (1f - t);
                float ol = 1f - 0.45f * edge;
                float rimDark = 1f - 0.35f * Smooth(0.0f, 0.5f, 1f - bf[i]);
                float m = shade * vl * ol * rimDark;
                // grime in the lower half
                float grime = 1f - 0.28f * Smooth(0.55f, 0.9f, nA[i]) * (0.4f + 0.6f * vt);
                m *= grime;
                float spec = 52f * Math.Max(0f, nx + ny - 0.20f) * t;        // bright rim on the lit (top-left) edges
                Over(cr, cg, cb, ca, i, Cl255(fr * m + spec), Cl255(fg * m + spec * 0.85f), Cl255(fbl * m + spec * 0.6f), F[i]);
            }

        // ---------- packing tape ----------
        List<RectangleF> b1 = new List<RectangleF>(), b2 = new List<RectangleF>();
        foreach (GraphicsPath p in L1) b1.Add(p.GetBounds());
        foreach (GraphicsPath p in L2) b2.Add(p.GetBounds());
        List<float[]> specs = new List<float[]>();   // cx, cy, len, wid, angleDeg
        int[] br1 = new int[] { 0, 3, 6 };           // tape bridging two neighbouring letters of line 1
        foreach (int bi in br1)
        {
            float tcx = (b1[bi].Right + b1[bi + 1].Left) * 0.5f;
            float tcy = y1 + cap1 * (0.28f + 0.42f * (float)R.NextDouble());
            float tang = (R.NextDouble() < 0.5 ? -1f : 1f) * (12f + (float)R.NextDouble() * 22f);
            specs.Add(new float[] { tcx, tcy, cap1 * 0.62f, cap1 * 0.15f, tang });
        }
        int[] br2 = new int[] { 1, 3 };              // line 2
        foreach (int bi in br2)
        {
            float tcx = (b2[bi].Right + b2[bi + 1].Left) * 0.5f;
            float tcy = y2 + cap2 * (0.30f + 0.40f * (float)R.NextDouble());
            float tang = (R.NextDouble() < 0.5 ? -1f : 1f) * (14f + (float)R.NextDouble() * 20f);
            specs.Add(new float[] { tcx, tcy, cap2 * 0.78f, cap2 * 0.20f, tang });
        }
        // corner tapes (hold the corners of the first / last letter)
        specs.Add(new float[] { b1[0].Left + cap1 * 0.06f, b1[0].Top + cap1 * 0.10f, cap1 * 0.46f, cap1 * 0.14f, -42f });
        specs.Add(new float[] { b1[b1.Count - 1].Right - cap1 * 0.05f, b1[b1.Count - 1].Bottom - cap1 * 0.08f, cap1 * 0.46f, cap1 * 0.14f, -40f });

        float[] tapeAll = new float[N];
        List<float[]> tapeMasks = new List<float[]>();
        foreach (float[] sp in specs)
        {
            List<GraphicsPath> one = new List<GraphicsPath>();
            one.Add(TapePath(sp[0], sp[1], sp[2], sp[3], sp[4], R));
            float[] tm = RenderMask(one, W, H);
            tapeMasks.Add(tm);
            for (int i = 0; i < N; i++) if (tm[i] > tapeAll[i]) tapeAll[i] = tm[i];
        }
        // soft shadow under the tape
        float[] tShBase = new float[N];
        for (int y = 0; y < H - 4; y++)
            for (int x = 0; x < W - 2; x++) tShBase[(y + 4) * W + x + 2] = tapeAll[y * W + x];
        float[] tSh = Blur(tShBase, W, H, 3);
        for (int i = 0; i < N; i++) Over(cr, cg, cb, ca, i, 20f, 10f, 3f, tSh[i] * 0.32f);

        for (int k = 0; k < specs.Count; k++)
        {
            float[] sp = specs[k]; float[] tm = tapeMasks[k];
            double ar = sp[4] * Math.PI / 180.0;
            float cosA = (float)Math.Cos(ar), sinA = (float)Math.Sin(ar);
            for (int y = 0; y < H; y++)
                for (int x = 0; x < W; x++)
                {
                    int i = y * W + x;
                    float m = tm[i];
                    if (m <= 0.01f) continue;
                    float ddx = x - sp[0], ddy = y - sp[1];
                    float u = ddx * cosA + ddy * sinA;
                    float vn = (-ddx * sinA + ddy * cosA) / sp[3];          // -0.5 .. 0.5 across the tape
                    // translucent amber tape body
                    Over(cr, cg, cb, ca, i, 226f, 170f, 84f, m * 0.50f);
                    // glossy streaks running along the tape + wrinkles
                    float band1 = (float)Math.Exp(-Math.Pow((vn + 0.26f) / 0.07f, 2.0));
                    float band2 = (float)Math.Exp(-Math.Pow((vn - 0.30f) / 0.04f, 2.0));
                    float wr = Clamp01((nB[i] - 0.42f) * 3f) * (0.5f + 0.5f * (float)Math.Sin(u * 0.10f + vn * 7f));
                    float sheen = 0.30f * band1 + 0.16f * band2 + 0.20f * wr;
                    Over(cr, cg, cb, ca, i, 255f, 238f, 196f, m * Clamp01(sheen));
                    // darker long edges + a faint dark crease
                    float e = Smooth(0.40f, 0.50f, Math.Abs(vn));
                    Over(cr, cg, cb, ca, i, 96f, 58f, 22f, m * 0.40f * e);
                    float crease = (float)Math.Exp(-Math.Pow((vn * 1.0f + 0.05f * (float)Math.Sin(u * 0.05f)) / 0.012f, 2.0)) * 0.10f;
                    Over(cr, cg, cb, ca, i, 120f, 80f, 30f, m * crease);
                }
        }

        // ---------- save ----------
        int[] outPx = new int[N];
        int[] prePx = new int[N];
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * W + x;
                int a = (int)(Clamp01(ca[i]) * 255f + 0.5f);
                int r = (int)(Cl255(cr[i]) + 0.5f), g = (int)(Cl255(cg[i]) + 0.5f), b = (int)(Cl255(cb[i]) + 0.5f);
                outPx[i] = (a << 24) | (r << 16) | (g << 8) | b;
                float al = Clamp01(ca[i]);
                float dx = (x - W * 0.5f) / (W * 0.5f), dy = (y - H * 0.5f) / (H * 0.9f);
                float rad = Clamp01(1f - (float)Math.Sqrt(dx * dx * 0.7f + dy * dy));
                float bgR = 6f + 52f * rad * rad, bgG = 4f + 28f * rad * rad, bgB = 3f + 9f * rad * rad;
                int pr = (int)(Cl255(cr[i] * al + bgR * (1f - al)) + 0.5f);
                int pg = (int)(Cl255(cg[i] * al + bgG * (1f - al)) + 0.5f);
                int pb = (int)(Cl255(cb[i] * al + bgB * (1f - al)) + 0.5f);
                prePx[i] = (255 << 24) | (pr << 16) | (pg << 8) | pb;
            }
        SaveArgb(outPx, W, H, outPath);
        SaveArgb(prePx, W, H, previewPath);
        return "font=" + fontName + " size=" + W + "x" + H + " cap1=" + cap1 + " cap2=" + cap2;
    }

    // strip of tape: straight long edges, torn zig-zag short ends, rotated about its centre
    static GraphicsPath TapePath(float cx, float cy, float len, float wid, float angDeg, Random R)
    {
        double a = angDeg * Math.PI / 180.0;
        float ca = (float)Math.Cos(a), sa = (float)Math.Sin(a);
        float hl = len * 0.5f, hw = wid * 0.5f;
        int n = 9;
        List<PointF> loc = new List<PointF>();
        for (int t = 0; t <= n; t++)       // left end, bottom -> top
        {
            float y = hw - wid * t / n;
            float j = (t == 0 || t == n) ? 0f : (float)R.NextDouble() * wid * 0.16f;
            loc.Add(new PointF(-hl + j, y));
        }
        for (int t = 0; t <= n; t++)       // right end, top -> bottom
        {
            float y = -hw + wid * t / n;
            float j = (t == 0 || t == n) ? 0f : (float)R.NextDouble() * wid * 0.16f;
            loc.Add(new PointF(hl - j, y));
        }
        PointF[] pts = new PointF[loc.Count];
        for (int i = 0; i < pts.Length; i++)
            pts[i] = new PointF(cx + loc[i].X * ca - loc[i].Y * sa, cy + loc[i].X * sa + loc[i].Y * ca);
        GraphicsPath gp = new GraphicsPath();
        gp.AddPolygon(pts);
        return gp;
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
$msg = [EldenTitle]::Make($SrcPath, $outPath, $previewPath, $Line1, $Line2, $Font, $W, $H, $Seed, $Depth, $Cracks)
$msg
"saved: $outPath"
"preview: $previewPath"
