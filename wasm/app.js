(function () {
	var params = new URLSearchParams(location.search);
	var touchMode = params.has('touch')
		|| window.matchMedia('(pointer: coarse)').matches
		|| (navigator.maxTouchPoints > 0 && !window.matchMedia('(pointer: fine)').matches);
	var W = touchMode ? 1280 : 1920;
	var H = touchMode ? 720 : 1080;
	var MOD = (touchMode ? 'minirt720.js' : 'minirt.js') + '?v=11';
	var SCENE = params.get('scene') === 'wonderland' ? 1 : 0;
	var canvas = document.getElementById('rt');
	var overlay = document.getElementById('overlay');
	var stick = document.getElementById('stick');
	var knob = document.getElementById('knob');
	var flyup = document.getElementById('flyup');
	var flydown = document.getElementById('flydown');
	var ctx;
	var img;
	var cam = { x: 0, y: 1.8, z: -13.5, yaw: 0, pitch: 0.03, fov: 72 };
	var SPEED = 7.0;
	var keys = {};
	var axX = 0;
	var axY = 0;
	var flyAx = 0;
	var dirty = true;
	var lastQ = 3;
	var moveQ = 2;
	var frame = 0;
	var pass = null;
	var outstanding = [];
	var msPerRow = { 0: 0, 1: 0, 2: 0, 3: 0 };
	var lastTime = performance.now();
	var workers = [];
	var bands = [];
	var readyCount = 0;
	var entered = false;
	var dragLook = false;
	var stickId = -1;
	var lookId = -1;
	var pinchId = -1;
	var stickOrigin = { x: 0, y: 0 };
	var lookLast = { x: 0, y: 0 };
	var pinchLast = { x: 0, y: 0 };
	var pinchDist0 = 0;
	var pinchFov0 = 72;

	if (SCENE === 0) {
		cam = { x: 0, y: 4.6, z: -9.8, yaw: 0, pitch: -0.35, fov: 58 };
		SPEED = 4.0;
	}
	canvas.width = W;
	canvas.height = H;
	ctx = canvas.getContext('2d');
	img = new ImageData(W, H);

	var n = Math.min(12, Math.max(2, (navigator.hardwareConcurrency || 4) - 1));
	if (touchMode)
		n = Math.min(n, 6);
	var rows = Math.ceil(H / n / 8) * 8;
	for (var y = 0; y < H; y += rows)
		bands.push([y, Math.min(y + rows, H)]);

	if (touchMode) {
		document.getElementById('enterhint').textContent = 'tap to enter';
		document.getElementById('controlhint').textContent =
			'left stick to walk, drag to look around, pinch to zoom';
	}

	function dir() {
		var cp = Math.cos(cam.pitch);
		return [Math.sin(cam.yaw) * cp, Math.sin(cam.pitch),
			Math.cos(cam.yaw) * cp];
	}

	function applyKeys(dt) {
		var d = dir();
		var fl = Math.hypot(d[0], d[2]) || 1;
		var f = [d[0] / fl, 0, d[2] / fl];
		var r = [-f[2], 0, f[0]];
		var s = SPEED * dt;
		var mx = axX;
		var my = axY;
		var fy = flyAx;
		if (keys.w) my += 1;
		if (keys.s) my -= 1;
		if (keys.d) mx += 1;
		if (keys.a) mx -= 1;
		if (keys.e) fy += 1;
		if (keys.q) fy -= 1;
		if (mx === 0 && my === 0 && fy === 0)
			return;
		cam.x += (f[0] * my + r[0] * mx) * s;
		cam.z += (f[2] * my + r[2] * mx) * s;
		cam.y += fy * s;
		if (cam.y < 0.4) cam.y = 0.4;
		if (cam.y > 60) cam.y = 60;
		dirty = true;
	}

	function rowCost(q) {
		if (msPerRow[q])
			return (msPerRow[q]);
		if (q === 3)
			return (0.2);
		return (rowCost(q + 1) * (q === 0 ? 16 : 4));
	}

	function feed(i) {
		var y0 = pass.cursor[i];
		var yEnd = bands[i][1];
		var rows;
		var y1;
		if (y0 >= yEnd)
			return;
		rows = Math.max(8, Math.min(yEnd - y0,
			Math.round(350 / rowCost(pass.q) / 8) * 8));
		y1 = Math.min(y0 + rows, yEnd);
		pass.cursor[i] = y1;
		outstanding[i] = { y0: y0, y1: y1, t: performance.now(), f: pass.frame };
		workers[i].postMessage({ frame: pass.frame, w: W,
			y0: y0, y1: y1, q: pass.q, cam: pass.cam });
	}

	function startPass(q) {
		var d = dir();
		frame++;
		pass = { q: q, frame: frame, t0: performance.now(), rowsDone: 0,
			cam: [cam.x, cam.y, cam.z, d[0], d[1], d[2], cam.fov],
			cursor: bands.map(function (b) { return b[0]; }) };
		for (var i = 0; i < workers.length; i++)
			feed(i);
	}

	function completePass() {
		var dur = performance.now() - pass.t0;
		lastQ = pass.q;
		if (pass.q === 2 && dur > 300)
			moveQ = 3;
		pass = null;
		if (params.has('debug')) {
			ctx.font = '42px monospace';
			ctx.fillStyle = '#ffffff';
			ctx.fillText('tm=' + touchMode + ' ent=' + entered
				+ ' q=' + lastQ + ' fov=' + Math.round(cam.fov)
				+ ' f=' + frame + ' ms=' + Math.round(dur), W / 2 - 400, H / 2);
		}
	}

	function pump() {
		var now = performance.now();
		var dt = Math.min(0.1, (now - lastTime) / 1000);
		lastTime = now;
		applyKeys(dt);
		if (pass) {
			for (var i = 0; i < workers.length; i++) {
				var o = outstanding[i];
				if (o && o.f === pass.frame && now - o.t > 12000) {
					try { workers[i].terminate(); } catch (err) {}
					spawn(i);
					workers[i].postMessage({ init: MOD, scene: SCENE });
					outstanding[i] = { y0: o.y0, y1: o.y1, t: now, f: pass.frame };
					workers[i].postMessage({ frame: pass.frame, w: W,
						y0: o.y0, y1: o.y1, q: pass.q, cam: pass.cam });
				}
			}
		}
		if (dirty) {
			if (!pass) {
				dirty = false;
				startPass(moveQ);
			} else if (pass.q <= 1) {
				pass = null;
				dirty = false;
				startPass(moveQ);
			}
		} else if (!pass) {
			if (lastQ >= 2)
				startPass(1);
			else if (lastQ === 1)
				startPass(0);
		}
		requestAnimationFrame(pump);
	}

	function onBand(e) {
		var d = e.data;
		if (d.ready) {
			readyCount++;
			if (readyCount === workers.length)
				requestAnimationFrame(pump);
			return;
		}
		if (!pass || d.frame !== pass.frame)
			return;
		var bi = 0;
		while (bi < bands.length - 1 && d.y0 >= bands[bi][1])
			bi++;
		var rows = new Uint8Array(d.buf).length / (W * 4);
		img.data.set(new Uint8Array(d.buf), d.y0 * W * 4);
		ctx.putImageData(img, 0, 0, 0, d.y0, W, rows);
		var o = outstanding[bi];
		if (o && o.f === pass.frame) {
			var spent = (performance.now() - o.t) / rows;
			msPerRow[pass.q] = msPerRow[pass.q]
				? msPerRow[pass.q] * 0.6 + spent * 0.4 : spent;
			outstanding[bi] = null;
		}
		pass.rowsDone += rows;
		if (pass.rowsDone >= H)
			completePass();
		else
			feed(bi);
	}

	function spawn(i) {
		var w = new Worker('worker.js?v=11');
		w.onmessage = onBand;
		w.onerror = function () {
			try { w.terminate(); } catch (err) {}
			spawn(i);
			workers[i].postMessage({ init: MOD, scene: SCENE });
		};
		workers[i] = w;
	}

	for (var i = 0; i < bands.length; i++) {
		spawn(i);
		workers[i].postMessage({ init: MOD, scene: SCENE });
	}

	function stickHome() {
		stick.style.left = '24px';
		stick.style.top = (window.innerHeight - 148) + 'px';
		knob.style.transform = 'translate(-50%, -50%)';
	}

	function enterTouch() {
		entered = true;
		overlay.style.display = 'none';
		flyup.style.display = 'flex';
		flydown.style.display = 'flex';
		stick.style.display = 'block';
		stickHome();
	}

	function enterFallback() {
		entered = true;
		overlay.style.display = 'none';
	}

	function enter() {
		if (canvas.requestPointerLock) {
			var p = canvas.requestPointerLock();
			if (p && p.catch)
				p.catch(function () { dragLook = true; enterFallback(); });
		} else {
			dragLook = true;
			enterFallback();
		}
	}

	overlay.addEventListener('click', function () {
		if (entered)
			return;
		if (touchMode)
			enterTouch();
		else
			enter();
	});
	overlay.addEventListener('touchend', function (e) {
		e.preventDefault();
		if (!entered)
			enterTouch();
	}, { passive: false });
	document.addEventListener('pointerlockchange', function () {
		var locked = !!document.pointerLockElement;
		if (locked)
			entered = true;
		if (!touchMode && !dragLook)
			overlay.style.display = locked ? 'none' : '';
	});
	document.addEventListener('pointerlockerror', function () {
		dragLook = true;
		enterFallback();
	});
	document.addEventListener('mousemove', function (e) {
		if (document.pointerLockElement) {
			cam.yaw -= e.movementX * 0.0028;
			cam.pitch -= e.movementY * 0.0028;
		} else if (dragLook && lookId === -2) {
			cam.yaw -= (e.clientX - lookLast.x) * 0.0032;
			cam.pitch -= (e.clientY - lookLast.y) * 0.0032;
			lookLast.x = e.clientX;
			lookLast.y = e.clientY;
		} else
			return;
		if (cam.pitch > 1.45) cam.pitch = 1.45;
		if (cam.pitch < -1.45) cam.pitch = -1.45;
		dirty = true;
	});
	canvas.addEventListener('mousedown', function (e) {
		if (dragLook && entered) {
			lookId = -2;
			lookLast.x = e.clientX;
			lookLast.y = e.clientY;
		}
	});
	window.addEventListener('mouseup', function () {
		if (lookId === -2)
			lookId = -1;
	});
	window.addEventListener('keydown', function (e) {
		var k = e.key.toLowerCase();
		if ('wasdqe'.indexOf(k) !== -1 && k.length === 1) {
			keys[k] = true;
			e.preventDefault();
		}
	});
	window.addEventListener('keyup', function (e) {
		keys[e.key.toLowerCase()] = false;
	});

	function moveKnob(dx, dy) {
		knob.style.transform = 'translate(calc(-50% + ' + dx + 'px), calc(-50% + '
			+ dy + 'px))';
	}

	function stickStart(t) {
		stickId = t.identifier;
		stickOrigin.x = t.clientX;
		stickOrigin.y = t.clientY;
		stick.style.left = (t.clientX - 62) + 'px';
		stick.style.top = (t.clientY - 62) + 'px';
		moveKnob(0, 0);
	}

	function stickMove(t) {
		var dx = t.clientX - stickOrigin.x;
		var dy = t.clientY - stickOrigin.y;
		var len = Math.hypot(dx, dy);
		if (len > 52) {
			dx = dx * 52 / len;
			dy = dy * 52 / len;
		}
		moveKnob(dx, dy);
		axX = dx / 52;
		axY = -dy / 52;
		if (Math.abs(axX) < 0.12) axX = 0;
		if (Math.abs(axY) < 0.12) axY = 0;
	}

	function stickEnd() {
		stickId = -1;
		axX = 0;
		axY = 0;
		stickHome();
	}

	function pinchStart(t) {
		pinchId = t.identifier;
		pinchLast.x = t.clientX;
		pinchLast.y = t.clientY;
		pinchDist0 = Math.hypot(t.clientX - lookLast.x, t.clientY - lookLast.y);
		pinchFov0 = cam.fov;
	}

	function applyPinch() {
		var d = Math.hypot(pinchLast.x - lookLast.x, pinchLast.y - lookLast.y);
		if (d < 20 || pinchDist0 < 20)
			return;
		cam.fov = pinchFov0 * pinchDist0 / d;
		if (cam.fov < 25) cam.fov = 25;
		if (cam.fov > 120) cam.fov = 120;
		dirty = true;
	}

	canvas.addEventListener('touchstart', function (e) {
		if (!entered)
			return;
		for (var i = 0; i < e.changedTouches.length; i++) {
			var t = e.changedTouches[i];
			if (stickId === -1 && t.clientX < window.innerWidth * 0.45
				&& lookId === -1)
				stickStart(t);
			else if (lookId === -1) {
				lookId = t.identifier;
				lookLast.x = t.clientX;
				lookLast.y = t.clientY;
			} else if (pinchId === -1)
				pinchStart(t);
		}
		e.preventDefault();
	}, { passive: false });
	canvas.addEventListener('touchmove', function (e) {
		for (var i = 0; i < e.changedTouches.length; i++) {
			var t = e.changedTouches[i];
			if (t.identifier === stickId)
				stickMove(t);
			else if (t.identifier === lookId) {
				if (pinchId !== -1) {
					lookLast.x = t.clientX;
					lookLast.y = t.clientY;
					applyPinch();
				} else {
					cam.yaw -= (t.clientX - lookLast.x) * 0.0042;
					cam.pitch -= (t.clientY - lookLast.y) * 0.0042;
					if (cam.pitch > 1.45) cam.pitch = 1.45;
					if (cam.pitch < -1.45) cam.pitch = -1.45;
					lookLast.x = t.clientX;
					lookLast.y = t.clientY;
					dirty = true;
				}
			} else if (t.identifier === pinchId) {
				pinchLast.x = t.clientX;
				pinchLast.y = t.clientY;
				applyPinch();
			}
		}
		e.preventDefault();
	}, { passive: false });
	function touchDone(e) {
		for (var i = 0; i < e.changedTouches.length; i++) {
			var t = e.changedTouches[i];
			if (t.identifier === stickId)
				stickEnd();
			else if (t.identifier === lookId) {
				if (pinchId !== -1) {
					lookId = pinchId;
					lookLast.x = pinchLast.x;
					lookLast.y = pinchLast.y;
					pinchId = -1;
				} else
					lookId = -1;
			} else if (t.identifier === pinchId)
				pinchId = -1;
		}
	}
	canvas.addEventListener('touchend', touchDone);
	canvas.addEventListener('touchcancel', touchDone);

	function bindFly(el, val) {
		el.addEventListener('touchstart', function (e) {
			flyAx = val;
			e.preventDefault();
		}, { passive: false });
		el.addEventListener('touchend', function (e) {
			flyAx = 0;
			e.preventDefault();
		}, { passive: false });
		el.addEventListener('touchcancel', function () { flyAx = 0; });
	}
	bindFly(flyup, 1);
	bindFly(flydown, -1);

	window.__rt = {
		state: function () {
			return { frame: frame, passQ: pass ? pass.q : -1, lastQ: lastQ,
				moveQ: moveQ, ready: readyCount, entered: entered,
				touchMode: touchMode, w: W, h: H,
				cost: [msPerRow[0], msPerRow[1], msPerRow[2], msPerRow[3]],
				cam: [cam.x, cam.y, cam.z, cam.yaw, cam.pitch, cam.fov] };
		}
	};

	if (params.has('enter')) {
		if (touchMode)
			enterTouch();
		else
			enterFallback();
	}
})();
