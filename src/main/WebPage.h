#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>MAU Arm Control</title>
<style>
:root{
  --bg:#0e1014;--panel:#161920;--panel2:#1d212a;--line:#272c37;
  --text:#e7e9ee;--muted:#8a92a3;--accent:#4c8dff;--ok:#2fbf71;--warn:#f0a531;--danger:#e5484d;
}
*{box-sizing:border-box}
html,body{margin:0;background:var(--bg);color:var(--text);
  font:14px/1.45 system-ui,-apple-system,"Segoe UI",Roboto,Helvetica,Arial,sans-serif}
header{position:sticky;top:0;z-index:10;display:flex;align-items:center;justify-content:space-between;
  gap:12px;padding:14px 20px;padding-top:calc(14px + env(safe-area-inset-top,0px));
  background:rgba(14,16,20,.94);backdrop-filter:blur(8px);border-bottom:1px solid var(--line)}
.brand{display:flex;flex-direction:column}
h1{font-size:16px;margin:0;font-weight:650;letter-spacing:.2px}
.sub{font-size:12px;color:var(--muted)}
.status{display:flex;gap:16px;align-items:center;font-size:12px;color:var(--muted)}
.pill{display:flex;align-items:center;gap:6px}
.dot{width:8px;height:8px;border-radius:50%;background:var(--muted)}
.dot.ok{background:var(--ok)}.dot.err{background:var(--danger)}.dot.warn{background:var(--warn)}
main{max-width:1120px;margin:0 auto;padding:20px 16px 150px}
.section{display:flex;justify-content:space-between;align-items:baseline;margin:4px 2px 12px}
.section h2{font-size:13px;font-weight:600;color:var(--muted);text-transform:uppercase;letter-spacing:.8px;margin:0}
.hint{font-size:12px;color:var(--muted)}
.grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(300px,1fr));gap:14px}
.card{background:var(--panel);border:1px solid var(--line);border-radius:12px;padding:16px;transition:border-color .15s}
.card.changed{border-color:var(--accent);box-shadow:0 0 0 1px var(--accent) inset}
.head{display:flex;justify-content:space-between;align-items:baseline;gap:8px}
.name{font-weight:600;font-size:15px}
.model{font-size:11px;color:var(--muted);text-transform:uppercase;letter-spacing:.6px;white-space:nowrap}
.vals{display:flex;align-items:baseline;justify-content:space-between;margin:10px 0 8px}
.staged{font-size:30px;font-weight:650;font-variant-numeric:tabular-nums}
.staged small{font-size:16px;color:var(--muted);font-weight:500}
.live{font-size:12px;color:var(--muted);font-variant-numeric:tabular-nums;text-align:right}
.live b{color:var(--text);font-weight:600}
.row{display:flex;align-items:center;gap:8px}
.step{flex:none;width:36px;height:36px;border-radius:8px;border:1px solid var(--line);background:var(--panel2);
  color:var(--text);font-size:18px;line-height:1;cursor:pointer}
.step:active{background:var(--line)}
input[type=range]{flex:1;min-width:0;accent-color:var(--accent);height:30px;margin:0}
.limits{display:flex;justify-content:space-between;font-size:11px;color:var(--muted);padding:0 44px}
.speed{margin-top:12px;padding-top:12px;border-top:1px solid var(--line)}
.speed label{display:flex;justify-content:space-between;font-size:12px;color:var(--muted);margin-bottom:2px}
.speed label b{color:var(--text);font-weight:600;font-variant-numeric:tabular-nums}
.speed input{accent-color:#7d8597}
.bar{position:fixed;left:0;right:0;bottom:0;z-index:10;background:rgba(14,16,20,.96);backdrop-filter:blur(8px);
  border-top:1px solid var(--line);padding:10px 16px calc(12px + env(safe-area-inset-bottom,0px))}
.pending{font-size:12px;text-align:center;min-height:18px;margin-bottom:8px;color:var(--muted)}
.pending.on{color:var(--accent)}
.actions{display:flex;gap:10px;justify-content:center;flex-wrap:wrap;max-width:1120px;margin:0 auto}
.btn{border-radius:10px;padding:12px 20px;font-size:14px;font-weight:600;cursor:pointer;color:var(--text);
  background:var(--panel2);border:1px solid var(--line);min-width:110px}
.btn:active{transform:translateY(1px)}
.btn.primary{background:var(--accent);border-color:var(--accent);color:#fff}
.btn.danger{background:var(--danger);border-color:var(--danger);color:#fff;min-width:130px}
.btn:disabled{opacity:.4;cursor:default;transform:none}
.toast{position:fixed;left:50%;bottom:140px;transform:translateX(-50%);background:var(--panel2);border:1px solid var(--line);
  padding:8px 14px;border-radius:8px;font-size:13px;opacity:0;transition:opacity .2s;pointer-events:none}
.toast.show{opacity:1}
.toast.err{border-color:var(--danger);color:#ffb4b6}
@media (max-width:520px){
  header{padding-left:16px;padding-right:16px}
  .status{gap:10px}
  .btn{flex:1;min-width:0;padding:12px 10px}
}
</style>
</head>
<body>

<header>
  <div class="brand">
    <h1>MAU Robotic Arm</h1>
    <span class="sub">6-axis control</span>
  </div>
  <div class="status">
    <span class="pill"><span class="dot" id="connDot"></span><span id="connText">Connecting</span></span>
    <span class="pill"><span class="dot" id="moveDot"></span><span id="moveText">Idle</span></span>
  </div>
</header>

<main>
  <div class="section">
    <h2>Joints</h2>
    <span class="hint">Set a pose, then press Go to pose. Speed changes apply instantly.</span>
  </div>
  <div class="grid" id="grid"></div>
</main>

<div class="toast" id="toast"></div>

<div class="bar">
  <div class="pending" id="pending">Pose matches the arm</div>
  <div class="actions">
    <button class="btn" id="revertBtn" onclick="revert()">Reset to arm</button>
    <button class="btn" onclick="home()">Home</button>
    <button class="btn primary" id="goBtn" onclick="go()">Go to pose</button>
    <button class="btn danger" onclick="stop()">STOP</button>
  </div>
</div>

<script>
let joints = [];   // config + live state from the arm
let staged = [];   // pose being edited, not sent until "Go to pose"
let online = false;

const $ = id => document.getElementById(id);
const fmt = v => (Math.round(v * 10) / 10).toFixed(1).replace(/\.0$/, '');

function toast(msg, err) {
  const t = $('toast');
  t.textContent = msg;
  t.className = 'toast show' + (err ? ' err' : '');
  clearTimeout(t._h);
  t._h = setTimeout(() => t.className = 'toast', 1800);
}

async function api(path) {
  const r = await fetch(path, { cache: 'no-store' });
  if (!r.ok) throw new Error(await r.text());
  return r;
}

function build() {
  const grid = $('grid');
  grid.innerHTML = '';
  joints.forEach((j, i) => {
    const c = document.createElement('div');
    c.className = 'card';
    c.id = 'card' + i;
    c.innerHTML = `
      <div class="head"><span class="name">${j.name}</span><span class="model">${j.servo}</span></div>
      <div class="vals">
        <span class="staged" id="st${i}"></span>
        <span class="live">Arm<br><b id="cur${i}"></b>°</span>
      </div>
      <div class="row">
        <button class="step" onclick="nudge(${i},-1)">&minus;</button>
        <input type="range" id="a${i}" min="${j.min}" max="${j.max}" step="1"
               oninput="stage(${i}, +this.value)">
        <button class="step" onclick="nudge(${i},1)">+</button>
      </div>
      <div class="limits"><span>${fmt(j.min)}°</span><span>${fmt(j.max)}°</span></div>
      <div class="speed">
        <label>Speed <span><b id="spv${i}"></b> °/s</span></label>
        <input type="range" id="sp${i}" min="1" max="${j.maxSpd}" step="1" value="${j.spd}"
               oninput="$('spv${i}').textContent=this.value" onchange="setSpeed(${i}, +this.value)">
      </div>`;
    grid.appendChild(c);
    $('spv' + i).textContent = fmt(j.spd);
  });
  renderStaged();
}

function clamp(i, v) {
  return Math.min(joints[i].max, Math.max(joints[i].min, v));
}

function stage(i, v) {
  staged[i] = clamp(i, v);
  renderStaged();
}

function nudge(i, d) {
  stage(i, Math.round(staged[i]) + d);
}

function renderStaged() {
  let changed = 0;
  joints.forEach((j, i) => {
    $('a' + i).value = staged[i];
    $('st' + i).innerHTML = fmt(staged[i]) + '<small>°</small>';
    const diff = Math.abs(staged[i] - j.tgt) > 0.05;
    $('card' + i).classList.toggle('changed', diff);
    if (diff) changed++;
  });
  const p = $('pending');
  p.textContent = changed
    ? `${changed} joint${changed > 1 ? 's' : ''} changed — press Go to pose to move`
    : 'Pose matches the arm';
  p.classList.toggle('on', changed > 0);
  $('goBtn').disabled = !changed;
  $('revertBtn').disabled = !changed;
}

function renderLive(moving) {
  joints.forEach((j, i) => $('cur' + i).textContent = fmt(j.cur));
  $('moveDot').className = 'dot' + (moving ? ' warn' : ' ok');
  $('moveText').textContent = moving ? 'Moving' : 'Idle';
}

function setOnline(on) {
  online = on;
  $('connDot').className = 'dot ' + (on ? 'ok' : 'err');
  $('connText').textContent = on ? 'Connected' : 'Offline';
}

function applyState(s) {
  s.joints.forEach((sj, i) => Object.assign(joints[i], sj));
  renderLive(s.moving);
  renderStaged();
}

async function poll() {
  try {
    const s = await (await api('/state')).json();
    setOnline(true);
    applyState(s);
  } catch (e) {
    setOnline(false);
  }
  setTimeout(poll, 400);
}

async function go() {
  try {
    await api('/pose?a=' + staged.map(v => v.toFixed(1)).join(','));
    joints.forEach((j, i) => j.tgt = staged[i]);
    renderStaged();
    toast('Moving to pose');
  } catch (e) { toast('Pose rejected: ' + e.message, true); }
}

function revert() {
  staged = joints.map(j => j.tgt);
  renderStaged();
}

async function home() {
  try {
    await api('/home');
    joints.forEach((j, i) => { j.tgt = j.home; staged[i] = j.home; });
    renderStaged();
    toast('Going home');
  } catch (e) { toast('Home failed', true); }
}

async function stop() {
  try {
    const s = await (await api('/stop')).json();
    applyState(s);
    staged = joints.map(j => j.tgt);
    renderStaged();
    toast('Stopped');
  } catch (e) { toast('STOP failed — cut power if needed', true); }
}

async function setSpeed(i, v) {
  try {
    await api(`/speed?joint=${i}&dps=${v}`);
    joints[i].spd = v;
  } catch (e) { toast('Speed change failed', true); }
}

async function init() {
  try {
    const s = await (await api('/state')).json();
    joints = s.joints;
    staged = joints.map(j => j.tgt);
    setOnline(true);
    build();
    renderLive(s.moving);
    setTimeout(poll, 400);
  } catch (e) {
    setOnline(false);
    setTimeout(init, 1500);
  }
}

document.addEventListener('keydown', e => {
  if (e.key === 'Escape' || e.key === ' ') { e.preventDefault(); stop(); }
});

init();
</script>
</body>
</html>
)rawliteral";

#endif
