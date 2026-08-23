(function () {
	var W = 1920;
	var H = 1080;
	var canvas = document.getElementById('rt');
	var overlay = document.getElementById('overlay');
	var stick = document.getElementById('stick');
	var knob = document.getElementById('knob');
	var flyup = document.getElementById('flyup');
	var flydown = document.getElementById('flydown');
	var ctx = canvas.getContext('2d');
	var img = new ImageData(W, H);
	var cam = { x: 0, y: 1.8, z: -13.5, yaw: 0, pitch: 0.03 };
	var keys = {};
	var axX = 0;
	var axY = 0;
	var flyAx = 0;
	var dirty = true;
	var lastQ = 2;
	var moveQ = 2;
	var sentAt = 0;
	var frame = 0;
	var inFlight = false;
	var arrived = 0;
	var lastTime = performance.now();
	var workers = [];
	var bands = [];
	var readyCount = 0;
	var entered = false;
	var dragLook = false;
	var params = new URLSearchParams(location.search);
	var touchMode = params.has('touch')
		|| window.matchMedia('(pointer: coarse)').matches
		|| (navigator.maxTouchPoints > 0 && !window.matchMedia('(pointer: fine)').matches);
	var stickId = -1;
	var lookId = -1;
	var stickOrigin = { x: 0, y: 0 };
	var lookLast = { x: 0, y: 0 };

	var n = Math.min(12, Math.max(2, (navigator.hardwareConcurrency || 4) - 1));
	var rows = Math.ceil(H / n / 4) * 4;
	for (var y = 0; y < H; y += rows)
		bands.push([y, Math.min(y + rows, H)]);

	if (touchMode) {
		document.getElementById('enterhint').textContent = 'tap to enter';
		document.getElementById('controlhint').textContent =
			'left thumb to walk, drag anywhere else to look around';
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
		var s = 7.0 * dt;
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
		cam.x += (f[0] * my - r[0] * mx) * s;
		cam.z += (f[2] * my - r[2] * mx) * s;
		cam.y += fy * s;
		if (cam.y < 0.4) cam.y = 0.4;
		if (cam.y > 60) cam.y = 60;
		dirty = true;
	}

	function dispatch(q) {
		var d = dir();
		var c = [cam.x, cam.y, cam.z, d[0], d[1], d[2]];
		frame++;
		arrived = 0;
		inFlight = true;
		lastQ = q;
		sentAt = performance.now();
		for (var i = 0; i < workers.length; i++)
			workers[i].postMessage({ frame: frame, w: W,
				y0: bands[i][0], y1: bands[i][1], q: q, cam: c });
	}

	function pump() {
		var now = performance.now();
		var dt = Math.min(0.1, (now - lastTime) / 1000);
		lastTime = now;
		applyKeys(dt);
		if (!inFlight) {
			if (dirty) {
				dirty = false;
				dispatch(moveQ);
			} else if (lastQ >= 2)
				dispatch(1);
			else if (lastQ === 1)
				dispatch(0);
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
		if (d.frame !== frame)
			return;
		img.data.set(new Uint8Array(d.buf), d.y0 * W * 4);
		arrived++;
		if (arrived === workers.length) {
			ctx.putImageData(img, 0, 0);
			inFlight = false;
			if (lastQ === 2 && performance.now() - sentAt > 250)
				moveQ = 3;
			if (params.has('debug')) {
				ctx.font = '42px monospace';
				ctx.fillStyle = '#ffffff';
				ctx.fillText('tm=' + touchMode + ' ent=' + entered
					+ ' fly=' + getComputedStyle(flyup).display
					+ ' iw=' + window.innerWidth + 'x' + window.innerHeight,
					W / 2 - 420, H / 2);
			}
		}
	}

	for (var i = 0; i < bands.length; i++) {
		var w = new Worker('worker.js');
		w.onmessage = onBand;
		workers.push(w);
	}

	function enterTouch() {
		entered = true;
		overlay.style.display = 'none';
		flyup.style.display = 'flex';
		flydown.style.display = 'flex';
	}

	function enter() {
		if (touchMode) {
			enterTouch();
			return;
		}
		if (canvas.requestPointerLock) {
			var p = canvas.requestPointerLock();
			if (p && p.catch)
				p.catch(function () { dragLook = true; enterFallback(); });
		} else {
			dragLook = true;
			enterFallback();
		}
	}

	function enterFallback() {
		entered = true;
		overlay.style.display = 'none';
		document.getElementById('controlhint').textContent =
			'drag to look around, walk with WASD, fly with Q and E';
	}

	overlay.addEventListener('click', function () {
		if (!touchMode || !('ontouchstart' in window))
			enter();
	});
	overlay.addEventListener('touchend', function (e) {
		enter();
		e.preventDefault();
	});
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
		stick.style.display = 'block';
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
		stick.style.display = 'none';
	}

	canvas.addEventListener('touchstart', function (e) {
		if (!entered)
			return;
		for (var i = 0; i < e.changedTouches.length; i++) {
			var t = e.changedTouches[i];
			if (stickId === -1 && t.clientX < window.innerWidth * 0.45)
				stickStart(t);
			else if (lookId === -1) {
				lookId = t.identifier;
				lookLast.x = t.clientX;
				lookLast.y = t.clientY;
			}
		}
		e.preventDefault();
	}, { passive: false });
	canvas.addEventListener('touchmove', function (e) {
		for (var i = 0; i < e.changedTouches.length; i++) {
			var t = e.changedTouches[i];
			if (t.identifier === stickId)
				stickMove(t);
			else if (t.identifier === lookId) {
				cam.yaw -= (t.clientX - lookLast.x) * 0.0042;
				cam.pitch -= (t.clientY - lookLast.y) * 0.0042;
				if (cam.pitch > 1.45) cam.pitch = 1.45;
				if (cam.pitch < -1.45) cam.pitch = -1.45;
				lookLast.x = t.clientX;
				lookLast.y = t.clientY;
				dirty = true;
			}
		}
		e.preventDefault();
	}, { passive: false });
	function touchDone(e) {
		for (var i = 0; i < e.changedTouches.length; i++) {
			var t = e.changedTouches[i];
			if (t.identifier === stickId)
				stickEnd();
			else if (t.identifier === lookId)
				lookId = -1;
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

	if (params.has('enter')) {
		if (touchMode)
			enterTouch();
		else
			enterFallback();
	}
})();
