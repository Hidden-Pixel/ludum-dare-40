var scene, camera, renderer;

function init() {
	scene = new THREE.Scene();
	var WIDTH = window.innerWidth;
	var HEIGHT = window.innerHeight;

	renderer = new THREE.WebGLRenderer({
		antialias: true
	});
	renderer.setSize(WIDTH, HEIGHT);
	document.body.appendChild(renderer.domElement);

	camera = new THREE.PerspectiveCamera(45, WIDTH / HEIGHT, 0.1, 20000);
	camera.position.set(0, 6, 0);
	scene.add(camera);

	window.addEventListener('resize', function() {
		var WIDTH = window.innerWidth;
		var HEIGHT = window.innerHeight;
		renderer.setSize(WIDTH, HEIGHT);
		camera.aspect = WIDTH / HEIGHT;
		camera.updateProjectionMatrix();
	});

	renderer.setClearColor(0x333F47, 1);
    	var light = new THREE.PointLight(0xffffff);
    	light.position.set(-100,200,100);
    	scene.add(light);
}

function animate() {
	var geometry = new THREE.BoxBufferGeometry(1, 1, 1);
	var material = new THREE.MeshBasicMaterial({
		color: 0xFFFF00
	});
	var mesh = new THREE.Mesh(geometry, material);
	scene.add(mesh);
	requestAnimationFrame(animate);
	renderer.render(scene, camera);
}

(function main() {
	init();
	animate();
}())
