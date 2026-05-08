// web_ui.cpp — Otomatik olusturuldu
#include "web_ui.h"
#include <Arduino.h>
#include <pgmspace.h>

const char WEB_HTML[] PROGMEM = R"HTMLEOF(
<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>TA2CAY RADAR SİSTEMİ</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Orbitron:wght@400;700;900&display=swap');
:root {
  --b: #00f3ff; --bd: #00b4c1; --r: #ff2244; --a: #ffaa00; --g: #00ff88;
  --bg: #000a0a; --panel: rgba(0,243,255,0.06); --panel-border: rgba(0,243,255,0.25);
}
* { margin:0; padding:0; box-sizing:border-box; }

body {
  background: var(--bg); color: var(--b); font-family: 'Share Tech Mono', monospace;
  height: 100vh; overflow: hidden; display: flex; flex-direction: column;
  background-image:
    radial-gradient(ellipse at 70% 50%, rgba(0,243,255,0.04) 0%, transparent 60%),
    radial-gradient(ellipse at 20% 80%, rgba(0,180,80,0.03) 0%, transparent 50%);
}

/* Scanline overlay */
body::after {
  content:''; position:fixed; inset:0;
  background: repeating-linear-gradient(0deg, transparent, transparent 2px, rgba(0,243,255,0.015) 2px, rgba(0,243,255,0.015) 4px);
  pointer-events:none; z-index:9999;
}

/* ───── HEADER ───── */
header {
  flex-shrink: 0;
  height: 70px; display: flex; align-items: center; justify-content: space-between; padding: 0 25px;
  border-bottom: 2px solid var(--panel-border);
  background: rgba(0,10,12,0.9);
  backdrop-filter: blur(10px);
  position: relative; z-index: 100;
}
header::after {
  content:''; position:absolute; bottom:-1px; left:0; right:0; height:1px;
  background: linear-gradient(90deg, transparent, var(--b), transparent);
  opacity: 0.5;
}
.logo-area { display:flex; align-items:center; gap:15px; }
.title-box h1 { font-family:'Orbitron',sans-serif; font-size:24px; letter-spacing:4px; text-shadow:0 0 20px var(--b), 0 0 40px rgba(0,243,255,0.3); }
.title-box p  { font-size:11px; color:var(--bd); letter-spacing:6px; margin-top:2px; }

.header-right { display:flex; align-items:center; gap:30px; text-align:right; }
.sys-status { display:flex; flex-direction:column; align-items:flex-end; gap:3px; }
#wst { font-size:12px; font-weight:bold; letter-spacing:2px; }
#clk { font-size:26px; color:var(--a); font-family:'Orbitron'; text-shadow:0 0 15px var(--a); }

/* ───── MAIN LAYOUT ───── */
main {
  flex:1; display:grid;
  grid-template-columns: 280px 1fr 280px;
  grid-template-rows: 1fr;
  gap:12px; padding:12px;
  overflow:hidden; min-height:0;
}

/* ───── SIDE PANELS ───── */
.side-panel { display:flex; flex-direction:column; gap:10px; overflow:hidden; }

.widget {
  background: var(--panel); border:1px solid var(--panel-border);
  padding:14px; position:relative; overflow:hidden;
  box-shadow: 0 0 20px rgba(0,243,255,0.04), inset 0 0 20px rgba(0,243,255,0.02);
}
.widget::before { content:''; position:absolute; top:0; left:0; width:12px; height:12px; border-top:2px solid var(--b); border-left:2px solid var(--b); }
.widget::after  { content:''; position:absolute; bottom:0; right:0; width:12px; height:12px; border-bottom:2px solid var(--b); border-right:2px solid var(--b); }

/* Subtle inner glow on widgets */
.widget-glow::before { box-shadow:none; }

.widget-title {
  font-size:11px; color:var(--bd); letter-spacing:3px; text-transform:uppercase;
  border-bottom:1px solid rgba(0,243,255,0.2); margin-bottom:12px; padding-bottom:6px;
  font-weight:bold;
}

/* ───── STATUS ALERT ───── */
.status-alert {
  padding:14px; text-align:center; border:2px solid;
  font-family:'Orbitron',sans-serif; font-weight:900; font-size:18px;
  letter-spacing:2px; position:relative; overflow:hidden;
}
.status-alert::before {
  content:''; position:absolute; top:-50%; left:-50%; width:200%; height:200%;
  background: conic-gradient(transparent 0deg, rgba(0,243,255,0.05) 90deg, transparent 180deg);
  animation: rotate 4s linear infinite;
}
@keyframes rotate { to { transform: rotate(360deg); } }
.alert-ok { border-color:var(--b); color:var(--b); background:rgba(0,243,255,0.05); }
.alert-warning { border-color:var(--r); color:var(--r); background:rgba(255,34,68,0.1); animation:blink 0.4s infinite; }
@keyframes blink { 50% { opacity:0.4; } }

/* ───── DATA ROWS ───── */
.data-row { display:flex; justify-content:space-between; align-items:center; margin-bottom:8px; }
.data-label { font-size:11px; color:var(--bd); text-transform:uppercase; letter-spacing:2px; }
.data-value-lg { font-family:'Orbitron',sans-serif; font-size:48px; color:var(--b); font-weight:900; text-shadow:0 0 25px var(--b); line-height:1; }
.data-value-md { font-family:'Orbitron',sans-serif; font-size:32px; color:var(--b); font-weight:700; text-shadow:0 0 15px var(--b); line-height:1; }
.data-value-sm { font-family:'Orbitron',sans-serif; font-size:22px; color:var(--b); font-weight:700; text-shadow:0 0 10px var(--b); line-height:1; }
.data-unit-lg { font-size:20px; color:var(--bd); margin-left:5px; }
.data-unit-md { font-size:14px; color:var(--bd); margin-left:4px; }

/* ───── PROGRESS BAR ───── */
.progress-bar-bg {
  height:10px; background:rgba(0,243,255,0.08); border-radius:5px;
  overflow:hidden; border:1px solid rgba(0,243,255,0.15);
}
.progress-bar-fill {
  height:100%; border-radius:5px; transition:width 0.3s ease;
  box-shadow:0 0 10px currentColor;
}
.fill-cyan { background:var(--b); color:var(--b); }
.fill-red  { background:var(--r); color:var(--r); }

/* ───── GAUGE RING (Sıcaklık / Nem) ───── */
.gauge-grid { display:grid; grid-template-columns:1fr 1fr; gap:10px; }
.gauge-item { text-align:center; padding:10px 5px; }
.gauge-label { font-size:10px; color:var(--bd); letter-spacing:2px; margin-bottom:8px; }
.gauge-canvas-wrap { position:relative; display:inline-block; }
.gauge-canvas-wrap canvas { display:block; }
.gauge-val {
  position:absolute; top:50%; left:50%; transform:translate(-50%,-50%);
  font-family:'Orbitron',sans-serif; font-weight:900; text-align:center;
}
.gauge-val .num { font-size:22px; line-height:1; }
.gauge-val .unit { font-size:10px; color:var(--bd); }

/* ───── AUDIO BUTTON ───── */
#btn-audio {
  width:100%; padding:12px; background:transparent; border:1px solid var(--b);
  color:var(--b); cursor:pointer; font-family:'Share Tech Mono'; font-size:13px;
  letter-spacing:2px; transition:all 0.3s; margin-top:8px;
  text-transform:uppercase;
}
#btn-audio:hover { background:rgba(0,243,255,0.1); box-shadow:0 0 15px rgba(0,243,255,0.3); }
#btn-audio.active { background:var(--b); color:var(--bg); box-shadow:0 0 20px var(--b); }

/* ───── STAT GRID ───── */
.stat-grid { display:grid; grid-template-columns:1fr 1fr; gap:8px; }
.stat-item {
  background:rgba(0,243,255,0.04); border:1px solid rgba(0,243,255,0.15);
  padding:10px 8px; text-align:center;
}
.stat-item .slabel { font-size:9px; color:var(--bd); letter-spacing:2px; display:block; margin-bottom:4px; }
.stat-item .sval { font-family:'Orbitron',sans-serif; font-size:20px; color:var(--b); font-weight:700; text-shadow:0 0 10px var(--b); }

/* ───── RADAR CENTER ───── */
.radar-container {
  position:relative; overflow:hidden;
  background: radial-gradient(ellipse at 50% 100%, rgba(0,243,255,0.06) 0%, transparent 70%);
  border:1px solid var(--panel-border);
  display:flex; align-items:stretch; justify-content:center;
}
#rc { display:block; width:100%; height:100%; }

/* ───── FOOTER ───── */
footer {
  flex-shrink:0; height:36px; border-top:1px solid var(--panel-border);
  display:flex; align-items:center; justify-content:space-between;
  padding:0 20px; font-size:10px; color:var(--bd); letter-spacing:2px;
  background:rgba(0,10,12,0.9);
}
footer span { color:var(--b); }

/* ───── SCROLLBAR HIDE ───── */
::-webkit-scrollbar { display:none; }
</style>
</head>
<body>

<header>
  <div class="logo-area">
    <div class="title-box">
      <h1>TA2CAY RADAR</h1>
      <p>RADAR PROJESİ</p>
    </div>
  </div>
  <div class="header-right">
    <div class="sys-status">
      <div id="wst" style="color:var(--a)">SİSTEM HAZIRLANIYOR...</div>
      <div id="clk">00:00:00</div>
    </div>
  </div>
</header>

<main>
  <!-- SOL PANEL -->
  <div class="side-panel">

    <!-- Durum -->
    <div class="widget">
      <div id="sb" class="status-alert alert-ok">TARA-BEKLE</div>
      <button id="btn-audio">🔇 SESİ AKTİFLEŞTİR</button>
    </div>

    <!-- Mesafe -->
    <div class="widget">
      <div class="widget-title">📡 MESAFE TELEMETRİSİ</div>
      <div style="text-align:center; padding:8px 0 12px;">
        <div class="data-label" style="margin-bottom:6px;">ANLIK MENZİL</div>
        <div>
          <span class="data-value-lg" id="dv">---</span>
          <span class="data-unit-lg">cm</span>
        </div>
      </div>
      <div class="progress-bar-bg" style="margin-bottom:14px;">
        <div id="db" class="progress-bar-fill fill-cyan" style="width:0%;"></div>
      </div>
      <div class="data-row">
        <span class="data-label">EN YAKIN ENGEL</span>
        <div>
          <span class="data-value-sm" id="cl" style="color:var(--r);">---</span>
          <span class="data-unit-md" style="color:var(--r);">cm</span>
        </div>
      </div>
    </div>

    <!-- Tarama Durumu -->
    <div class="widget">
      <div class="widget-title">🎯 TARAMA DURUMU</div>
      <div style="text-align:center; padding:6px 0 10px;">
        <div class="data-label" style="margin-bottom:4px;">AKTİF AÇI</div>
        <div>
          <span class="data-value-md" id="av">0</span>
          <span class="data-unit-md">°</span>
        </div>
      </div>
      <div class="data-row" style="margin-bottom:4px;">
        <span class="data-label">YÖN</span>
        <span class="data-value-sm" id="dir" style="font-size:16px;">→</span>
      </div>
      <!-- Açı çubuğu -->
      <div style="position:relative; height:8px; background:rgba(0,243,255,0.08); border-radius:4px; margin-top:8px; overflow:hidden;">
        <div id="anglebar" style="position:absolute; top:0; height:100%; width:4px; background:var(--b); border-radius:2px; box-shadow:0 0 8px var(--b); transition:left 0.15s ease;"></div>
      </div>
      <div style="display:flex; justify-content:space-between; margin-top:3px; font-size:9px; color:var(--bd);">
        <span>0°</span><span>90°</span><span>180°</span>
      </div>
    </div>

  </div>

  <!-- ORTA: RADAR -->
  <div class="radar-container">
    <canvas id="rc"></canvas>
  </div>

  <!-- SAĞ PANEL -->
  <div class="side-panel">

    <!-- Sıcaklık & Nem Gauge'ları -->
    <div class="widget">
      <div class="widget-title">🌡 ORTAM VERİLERİ</div>
      <div class="gauge-grid">
        <!-- Sıcaklık -->
        <div class="gauge-item">
          <div class="gauge-label">SICAKLIK</div>
          <div class="gauge-canvas-wrap">
            <canvas id="tempGauge" width="110" height="70"></canvas>
            <div class="gauge-val">
              <div class="num" id="tv" style="color:var(--a); text-shadow:0 0 12px var(--a);">--</div>
              <div class="unit">°C</div>
            </div>
          </div>
        </div>
        <!-- Nem -->
        <div class="gauge-item">
          <div class="gauge-label">NEM ORANI</div>
          <div class="gauge-canvas-wrap">
            <canvas id="humGauge" width="110" height="70"></canvas>
            <div class="gauge-val">
              <div class="num" id="hv" style="color:var(--b); text-shadow:0 0 12px var(--b);">--</div>
              <div class="unit">%</div>
            </div>
          </div>
        </div>
      </div>
      <!-- Sıcaklık bar -->
      <div style="margin-top:12px;">
        <div style="display:flex; justify-content:space-between; font-size:9px; color:var(--bd); margin-bottom:4px;">
          <span>0°C</span><span>SICAKLIK</span><span>50°C</span>
        </div>
        <div class="progress-bar-bg">
          <div id="tempbar" class="progress-bar-fill" style="width:0%; background:linear-gradient(90deg, #00f3ff, #ffaa00, #ff2244); color:var(--a);"></div>
        </div>
      </div>
      <!-- Nem bar -->
      <div style="margin-top:8px;">
        <div style="display:flex; justify-content:space-between; font-size:9px; color:var(--bd); margin-bottom:4px;">
          <span>0%</span><span>NEM</span><span>100%</span>
        </div>
        <div class="progress-bar-bg">
          <div id="humbar" class="progress-bar-fill fill-cyan" style="width:0%;"></div>
        </div>
      </div>
    </div>

    <!-- İstatistikler -->
    <div class="widget">
      <div class="widget-title">📊 SİSTEM İSTATİSTİKLERİ</div>
      <div class="stat-grid">
        <div class="stat-item">
          <span class="slabel">TARAMA SAYISI</span>
          <span class="sval" id="sv">0</span>
        </div>
        <div class="stat-item">
          <span class="slabel">TESPİT SAYISI</span>
          <span class="sval" id="dt" style="color:var(--a); text-shadow:0 0 10px var(--a);">0</span>
        </div>
        <div class="stat-item" style="grid-column:1/-1;">
          <span class="slabel">SİSTEM IP ADRESİ</span>
          <span class="sval" id="ip" style="font-size:14px; color:var(--g); text-shadow:0 0 8px var(--g);">192.168.4.1</span>
        </div>
      </div>
    </div>

    <!-- Bağlantı durumu -->
    <div class="widget">
      <div class="widget-title">🔗 BAĞLANTI</div>
      <div style="display:flex; flex-direction:column; gap:8px;">
        <div class="data-row">
          <span class="data-label">SSID</span>
          <span style="color:var(--g); font-size:12px;">ta2cay</span>
        </div>
        <div class="data-row">
          <span class="data-label">PROTOKOL</span>
          <span style="color:var(--g); font-size:12px;">WebSocket</span>
        </div>
        <div class="data-row">
          <span class="data-label">PORT</span>
          <span style="color:var(--b); font-size:12px;">:81</span>
        </div>
        <div class="data-row">
          <span class="data-label">DURUM</span>
          <span id="conbar" style="color:var(--a); font-size:12px; font-weight:bold;">BEKLENIYOR</span>
        </div>
      </div>
    </div>

  </div>
</main>

<footer>
  <div>ESP32 CORE | <span>7/24 SİSTEM AKTİF</span></div>
</footer>

<script>
// ──────────────────────────────────────────
// CANVAS SETUP
// ──────────────────────────────────────────
const canvas = document.getElementById('rc');
const ctx = canvas.getContext('2d');
let audioEnabled = false, lastSpeak = 0;
let sweepAngle = 0, sweepDir = 1, points = [];
let ws;

// Gauge canvases
const tempGaugeCanvas = document.getElementById('tempGauge');
const humGaugeCanvas  = document.getElementById('humGauge');
const tgCtx = tempGaugeCanvas.getContext('2d');
const hgCtx = humGaugeCanvas.getContext('2d');

let currentTemp = 0, currentHum = 0;

// ──────────────────────────────────────────
// RESIZE
// ──────────────────────────────────────────
function resize() {
  const container = document.querySelector('.radar-container');
  canvas.width  = container.clientWidth;
  canvas.height = container.clientHeight;
}
window.addEventListener('resize', resize);
resize();

// ──────────────────────────────────────────
// AUDIO
// ──────────────────────────────────────────
const audioCtx = new (window.AudioContext || window.webkitAudioContext)();

function playPing(freq=800, dur=0.1, vol=0.2) {
  if (!audioEnabled || audioCtx.state === 'suspended') return;
  const osc=audioCtx.createOscillator(), gain=audioCtx.createGain();
  osc.type='sine'; osc.frequency.setValueAtTime(freq, audioCtx.currentTime);
  gain.gain.setValueAtTime(vol, audioCtx.currentTime);
  gain.gain.exponentialRampToValueAtTime(0.01, audioCtx.currentTime+dur);
  osc.connect(gain); gain.connect(audioCtx.destination);
  osc.start(); osc.stop(audioCtx.currentTime+dur);
}

function speak(text) {
  if (!audioEnabled) return;
  if (Date.now()-lastSpeak < 5000) return;
  const msg=new SpeechSynthesisUtterance(text);
  msg.lang='tr-TR'; msg.rate=0.9;
  window.speechSynthesis.speak(msg);
  lastSpeak=Date.now();
}

document.getElementById('btn-audio').onclick = function() {
  audioEnabled = !audioEnabled;
  this.textContent = audioEnabled ? '🔊 SES AKTİF' : '🔇 SESİ AKTİFLEŞTİR';
  this.className = audioEnabled ? 'active' : '';
  if (audioEnabled) { audioCtx.resume(); speak('Sistem aktif, sesli geri bildirim devrede.'); }
};

// ──────────────────────────────────────────
// WEBSOCKET
// ──────────────────────────────────────────
function connectWS() {
  ws = new WebSocket('ws://'+location.hostname+':81/');
  ws.onopen = () => {
    document.getElementById('wst').textContent='SİSTEM ÇALIŞIYOR';
    document.getElementById('wst').style.color='var(--b)';
    document.getElementById('conbar').textContent='BAĞLI ✓';
    document.getElementById('conbar').style.color='var(--g)';
  };
  ws.onclose = () => {
    document.getElementById('wst').textContent='BAĞLANTI KESİLDİ';
    document.getElementById('wst').style.color='var(--r)';
    document.getElementById('conbar').textContent='KESİLDİ ✕';
    document.getElementById('conbar').style.color='var(--r)';
    setTimeout(connectWS, 2000);
  };
  ws.onmessage = (e) => {
    const d = JSON.parse(e.data);
    sweepAngle = d.angle;
    sweepDir   = d.dir;
    const dist = d.distance;

    // Mesafe
    document.getElementById('dv').textContent = dist>0 ? dist : '---';
    document.getElementById('db').style.width = dist>0 ? Math.min(100,(dist/200)*100)+'%' : '0%';

    // Açı göstergesi
    document.getElementById('av').textContent = d.angle;
    const pct = d.angle / 180;
    document.getElementById('anglebar').style.left = `calc(${pct*100}% - 2px)`;
    document.getElementById('dir').textContent = d.dir>0 ? '→ SAĞ' : '← SOL';

    // Sayaçlar
    document.getElementById('sv').textContent  = d.scans;
    document.getElementById('dt').textContent  = d.detections;

    // En yakın
    if (d.closest>0) document.getElementById('cl').textContent = d.closest;

    // Sıcaklık & Nem
    if (d.temp>0) {
      currentTemp = d.temp;
      document.getElementById('tv').textContent = d.temp;
      const tp = Math.min(100, (d.temp/50)*100);
      document.getElementById('tempbar').style.width = tp+'%';
    }
    if (d.hum>0) {
      currentHum = d.hum;
      document.getElementById('hv').textContent = d.hum;
      document.getElementById('humbar').style.width = Math.min(100, d.hum)+'%';
    }

    // Durum
    const sb=document.getElementById('sb');
    if (dist>0 && dist<50) {
      sb.className='status-alert alert-warning';
      sb.textContent='⚠ ENGEL: '+dist+'cm';
      playPing(1200,0.15,0.4);
      if (dist<30) speak('Dikkat, engel çok yakın!');
    } else {
      sb.className='status-alert alert-ok';
      sb.textContent='✓ ALAN TEMİZ';
    }

    if (dist>0 && dist<200) {
      points.push({a:d.angle, d:dist, t:Date.now()});
      playPing(600,0.05,0.1);
    }
  };
}
connectWS();

// ──────────────────────────────────────────
// CLOCK
// ──────────────────────────────────────────
setInterval(()=>{
  document.getElementById('clk').textContent=new Date().toLocaleTimeString('tr-TR');
}, 1000);

// ──────────────────────────────────────────
// GAUGE DRAWING (Arc gauge)
// ──────────────────────────────────────────
function drawArcGauge(ctx, canvas, value, max, color, bgColor) {
  const w=canvas.width, h=canvas.height;
  ctx.clearRect(0,0,w,h);
  const cx=w/2, cy=h-8, r=Math.min(w,h*2)/2 - 8;
  const startAngle=Math.PI, endAngle=0;
  const pct = Math.min(1, Math.max(0, value/max));

  // BG arc
  ctx.beginPath(); ctx.arc(cx,cy,r,startAngle,endAngle);
  ctx.strokeStyle=bgColor; ctx.lineWidth=10; ctx.lineCap='round'; ctx.stroke();

  // Value arc
  if (pct>0) {
    const valEnd = startAngle + pct*Math.PI;
    ctx.beginPath(); ctx.arc(cx,cy,r,startAngle,valEnd);
    ctx.strokeStyle=color; ctx.lineWidth=10; ctx.lineCap='round';
    ctx.shadowBlur=12; ctx.shadowColor=color;
    ctx.stroke(); ctx.shadowBlur=0;
  }

  // Tick marks
  for (let i=0; i<=10; i++) {
    const a = Math.PI + i*(Math.PI/10);
    const r1=r-8, r2=r+2;
    const x1=cx+r1*Math.cos(a), y1=cy+r1*Math.sin(a);
    const x2=cx+r2*Math.cos(a), y2=cy+r2*Math.sin(a);
    ctx.beginPath(); ctx.moveTo(x1,y1); ctx.lineTo(x2,y2);
    ctx.strokeStyle=i%5===0 ? color : bgColor; ctx.lineWidth=i%5===0?2:1; ctx.stroke();
  }
}

function updateGauges() {
  drawArcGauge(tgCtx, tempGaugeCanvas, currentTemp, 50, '#ffaa00', 'rgba(255,170,0,0.15)');
  drawArcGauge(hgCtx, humGaugeCanvas,  currentHum, 100, '#00f3ff', 'rgba(0,243,255,0.1)');
  requestAnimationFrame(updateGauges);
}
updateGauges();

// ──────────────────────────────────────────
// RADAR DRAWING
// ──────────────────────────────────────────
function draw() {
  const W=canvas.width, H=canvas.height;
  ctx.clearRect(0,0,W,H);

  // Center: bottom-center with enough room for labels at top
  const cx = W/2;
  const cy = H - 55;
  const r  = Math.min(cx - 80, cy - 30);

  // ── Background: faint full-screen grid ──
  ctx.save();
  ctx.strokeStyle='rgba(0,243,255,0.025)';
  ctx.lineWidth=1;
  for(let x=0;x<W;x+=40){ ctx.beginPath();ctx.moveTo(x,0);ctx.lineTo(x,H);ctx.stroke(); }
  for(let y=0;y<H;y+=40){ ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(W,y);ctx.stroke(); }
  ctx.restore();

  // ── Upper area: Yazılar ──
  const ta2cayFontSize = Math.max(32, Math.round(r * 0.24));
  const radarFontSize = Math.max(16, Math.round(r * 0.12));
  // Toplam blok yüksekliği: TA2CAY + boşluk + RADAR + dekoratif çizgi
  const blockH = ta2cayFontSize + 4 + radarFontSize + 14;
  // Bloğu üst alanın ortasına yerleştir
  const topSpace = cy - r - 10; // radarın tepesinden canvas üstüne kadar olan alan
  const blockTop = Math.max(8, (topSpace - blockH) / 2);

  // "TA2CAY" — büyük, parlak
  const titleY = blockTop;
  ctx.save();
  ctx.textAlign='center'; ctx.textBaseline='top';
  ctx.font='900 ' + ta2cayFontSize + 'px Orbitron';
  // Gölge/glow katmanı
  ctx.shadowBlur=35; ctx.shadowColor='rgba(0,243,255,0.8)';
  ctx.fillStyle='rgba(0,243,255,0.75)';
  ctx.fillText('TA2CAY', cx, titleY);
  ctx.shadowBlur=0;

  // "RADAR SİSTEMİ" — orta boy, daha soluk
  const radarY = titleY + ta2cayFontSize + 5;
  ctx.font='700 ' + radarFontSize + 'px Orbitron';
  ctx.fillStyle='rgba(0,180,193,0.55)';
  ctx.shadowBlur=10; ctx.shadowColor='rgba(0,243,255,0.4)';
  ctx.fillText('RADAR SİSTEMİ', cx, radarY);
  ctx.shadowBlur=0;

  // Dekoratif yatay çizgi
  const lineW = Math.min(r * 0.65, 180);
  const lineY  = radarY + radarFontSize + 6;
  const lg = ctx.createLinearGradient(cx-lineW, lineY, cx+lineW, lineY);
  lg.addColorStop(0,'transparent');
  lg.addColorStop(0.45,'rgba(0,243,255,0.55)');
  lg.addColorStop(0.55,'rgba(0,243,255,0.55)');
  lg.addColorStop(1,'transparent');
  ctx.beginPath(); ctx.moveTo(cx-lineW, lineY); ctx.lineTo(cx+lineW, lineY);
  ctx.strokeStyle=lg; ctx.lineWidth=1; ctx.stroke();
  // Küçük merkez nokta üzerinde
  ctx.beginPath(); ctx.arc(cx, lineY, 3, 0, Math.PI*2);
  ctx.fillStyle='rgba(0,243,255,0.6)'; ctx.fill();
  ctx.restore();

  // ── Radar shadow base ──
  const bgGrad = ctx.createRadialGradient(cx,cy,0,cx,cy,r);
  bgGrad.addColorStop(0,'rgba(0,243,255,0.07)');
  bgGrad.addColorStop(0.7,'rgba(0,243,255,0.02)');
  bgGrad.addColorStop(1,'transparent');
  ctx.beginPath(); ctx.arc(cx,cy,r,Math.PI,0);
  ctx.fillStyle=bgGrad; ctx.fill();

  // ── Distance rings ──
  for(let i=1;i<=4;i++){
    const distR=r*(i/4);
    ctx.beginPath(); ctx.arc(cx,cy,distR,Math.PI,0);
    ctx.strokeStyle=`rgba(0,243,255,${0.08+i*0.04})`; ctx.lineWidth=1; ctx.stroke();

    // Ring label
    const lx=cx+distR*Math.cos(Math.PI*0.55);
    const ly=cy-distR*Math.sin(Math.PI*0.55);
    ctx.fillStyle='rgba(0,180,193,0.8)'; ctx.font='11px Share Tech Mono';
    ctx.textAlign='center'; ctx.fillText((i*50)+'cm',lx,ly);
  }

  // ── Axis lines every 30° ──
  for (let a=0;a<=180;a+=30) {
    const rad=(180-a)*Math.PI/180;
    ctx.beginPath(); ctx.moveTo(cx,cy);
    ctx.lineTo(cx+r*1.02*Math.cos(rad), cy-r*1.02*Math.sin(rad));
    ctx.strokeStyle='rgba(0,243,255,0.12)'; ctx.lineWidth=1; ctx.stroke();
  }

  // ── Outer arc glow (bright border) ──
  ctx.beginPath(); ctx.arc(cx,cy,r,Math.PI,0);
  ctx.strokeStyle='rgba(0,243,255,0.6)'; ctx.lineWidth=3;
  ctx.shadowBlur=20; ctx.shadowColor='var(--b)'; ctx.stroke();

  // ── Baseline (horizontal) ──
  ctx.beginPath(); ctx.moveTo(cx-r,cy); ctx.lineTo(cx+r,cy);
  ctx.strokeStyle='rgba(0,243,255,0.4)'; ctx.lineWidth=2;
  ctx.shadowBlur=8; ctx.shadowColor='var(--b)'; ctx.stroke(); ctx.shadowBlur=0;

  // ── Degree tick markers & labels ──
  for(let a=0;a<=180;a+=10){
    const rad=(180-a)*Math.PI/180;
    const big = a%30===0;
    const r1=r, r2=r+(big?18:8);
    const x1=cx+r1*Math.cos(rad), y1=cy-r1*Math.sin(rad);
    const x2=cx+r2*Math.cos(rad), y2=cy-r2*Math.sin(rad);
    ctx.beginPath(); ctx.moveTo(x1,y1); ctx.lineTo(x2,y2);
    ctx.strokeStyle=big?'rgba(0,243,255,0.9)':'rgba(0,180,193,0.5)';
    ctx.lineWidth=big?2:1; ctx.shadowBlur=big?8:0; ctx.shadowColor='var(--b)';
    ctx.stroke(); ctx.shadowBlur=0;

    if(big){
      const lx=cx+(r+32)*Math.cos(rad), ly=cy-(r+32)*Math.sin(rad);
      ctx.fillStyle='rgba(0,243,255,0.9)'; ctx.font='bold 13px Orbitron';
      ctx.textAlign='center'; ctx.textBaseline='middle';
      ctx.fillText(a+'°',lx,ly);
    }
  }

  // ── Origin dot ──
  ctx.beginPath(); ctx.arc(cx,cy,5,0,Math.PI*2);
  ctx.fillStyle='var(--b)'; ctx.shadowBlur=15; ctx.shadowColor='var(--b)'; ctx.fill(); ctx.shadowBlur=0;

  // ── Sweep trail ──
  const sweepRad=(180-sweepAngle)*Math.PI/180;
  ctx.save();
  ctx.beginPath(); ctx.moveTo(cx,cy);
  const trailSpread = sweepDir>0 ? 0.55 : -0.55;
  ctx.arc(cx,cy,r, sweepRad, sweepRad+trailSpread, sweepDir<0);
  ctx.closePath();
  const trailGrad=ctx.createRadialGradient(cx,cy,0,cx,cy,r);
  trailGrad.addColorStop(0,'rgba(0,243,255,0.0)');
  trailGrad.addColorStop(0.5,'rgba(0,243,255,0.08)');
  trailGrad.addColorStop(1,'rgba(0,243,255,0.25)');
  ctx.fillStyle=trailGrad; ctx.fill();
  ctx.restore();

  // ── Detected points ──
  const now=Date.now();
  points=points.filter(p=>now-p.t<5000);
  points.forEach(p=>{
    const alpha=1-(now-p.t)/5000;
    const pRad=(180-p.a)*Math.PI/180;
    const dist=(p.d/200)*r;
    const px=cx+dist*Math.cos(pRad), py=cy-dist*Math.sin(pRad);
    const col=p.d<50?`rgba(255,34,68,${alpha})`:`rgba(0,243,255,${alpha})`;

    // Pulse ring
    const pulseR=8+12*(1-alpha);
    ctx.beginPath(); ctx.arc(px,py,pulseR,0,Math.PI*2);
    ctx.strokeStyle=col; ctx.lineWidth=1.5; ctx.stroke();

    // Core dot
    ctx.beginPath(); ctx.arc(px,py,5,0,Math.PI*2);
    ctx.fillStyle=col; ctx.shadowBlur=20; ctx.shadowColor=col; ctx.fill(); ctx.shadowBlur=0;

    // Distance label on point
    if(alpha>0.5){
      ctx.fillStyle=col; ctx.font='10px Share Tech Mono'; ctx.textAlign='center';
      ctx.fillText(p.d+'cm', px, py-12);
    }
  });

  // ── Sweep line ──
  const lineGrad=ctx.createLinearGradient(cx,cy, cx+r*Math.cos(sweepRad), cy-r*Math.sin(sweepRad));
  lineGrad.addColorStop(0,'rgba(0,243,255,0.1)');
  lineGrad.addColorStop(1,'rgba(0,243,255,1)');
  ctx.beginPath(); ctx.moveTo(cx,cy);
  ctx.lineTo(cx+r*Math.cos(sweepRad), cy-r*Math.sin(sweepRad));
  ctx.strokeStyle=lineGrad; ctx.lineWidth=3;
  ctx.shadowBlur=25; ctx.shadowColor='#00f3ff'; ctx.stroke(); ctx.shadowBlur=0;

  requestAnimationFrame(draw);
}
draw();
</script>
</body>
</html>
)HTMLEOF";
