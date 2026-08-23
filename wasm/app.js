(function () {
	var W = 1920;
	var H = 1080;
	var canvas = document.getElementById('rt');
	var overlay = document.getElementById('overlay');
	var ctx = canvas.getContext('2d');
	var img = new ImageData(W, H);
	var cam = { x: 0, y: 1.8, z: -13.5, yaw: 0, pitch: 0.03 };
	var keys = {};
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

	var n = Math.min(12, Math.max(2, (navigator.hardwareConcurrency || 4) - 1));
	var rows = Math.ceil(H / n / 4) * 4;
	for (var y = 0; y < H; y += rows)
		bands.push([y, Math.min(y + rows, H)]);

	function dir() {
		var cp = Math.cos(cam.pitch);
		return [Math.sin(cam.yaw) * cp, Math.sin(cam.pitch),
			Math.cos(cam.yaw) * cp];
	}

	function applyKeys(dt) {
		var d = dir();
		var f = [d[0], 0, d[2]];
		var fl = Math.hypot(f[0], f[2]) || 1;
		var r = [-f[2] / fl, 0, f[0] / fl];
		var s = 7.0 * dt;
		var moved = false;
		f = [f[0] / fl, 0, f[2] / fl];
		if (keys.w) { cam.x += f[0] * s; cam.z += f[2] * s; moved = true; }
		if (keys.s) { cam.x -= f[0] * s; cam.z -= f[2] * s; moved = true; }
		if (keys.d) { cam.x -= r[0] * s; cam.z -= r[2] * s; moved = true; }
		if (keys.a) { cam.x += r[0] * s; cam.z += r[2] * s; moved = true; }
		if (keys.e) { cam.y += s; moved = true; }
		if (keys.q) { cam.y -= s; moved = true; }
		if (moved) {
			if (cam.y < 0.4) cam.y = 0.4;
			if (cam.y > 60) cam.y = 60;
			dirty = true;
		}
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
		}
	}

	for (var i = 0; i < bands.length; i++) {
		var w = new Worker('worker.js');
		w.onmessage = onBand;
		workers.push(w);
	}

	canvas.addEventListener('click', function () {
		canvas.requestPointerLock();
	});
	document.addEventListener('pointerlockchange', function () {
		overlay.style.display = document.pointerLockElement ? 'none' : '';
	});
	document.addEventListener('mousemove', function (e) {
		if (!document.pointerLockElement)
			return;
		cam.yaw -= e.movementX * 0.0028;
		cam.pitch -= e.movementY * 0.0028;
		if (cam.pitch > 1.45) cam.pitch = 1.45;
		if (cam.pitch < -1.45) cam.pitch = -1.45;
		dirty = true;
	});
	window.addEventListener('keydown', function (e) {
		var k = e.key.toLowerCase();
		if ('wasdqe'.indexOf(k) !== -1) {
			keys[k] = true;
			e.preventDefault();
		}
	});
	window.addEventListener('keyup', function (e) {
		keys[e.key.toLowerCase()] = false;
	});
})();
